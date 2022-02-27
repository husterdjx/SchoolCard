#include<iostream>
#include<fstream>
#include "def.h"
#include<cassert>
#include<queue>
#include<time.h>
#include<io.h>
#include <sstream>
using namespace std;
string curTime;
int curWindow;
vector<WindowManager> Windows(100);
double sumMoney;
int sumTimes;

extern string CardID2ActMap[CARD_MAX_NUM];
extern CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];
extern fstream xflogWriter, czlogWriter;

struct cmp
{
	bool operator()(XFMeg *a, XFMeg *b) const
	{
		return a->xfdate + a->xftime > b->xfdate + b->xftime;
	}
};

priority_queue<XFMeg*, vector<XFMeg*>, cmp> minheap;

void initWindows() {
	for (int i = 1; i < 58; ++i) {
		Windows[i].num = i;
		Windows[i].pointer = 0;
	}
}
void read2XF() {
	initWindows();
	cout << "��ʼ���������ļ�" << endl;
	ifstream xffile;
	clock_t start, end;
	start = clock();
	xffile.open("E:\\classdesign\\xf014.txt", ios::in);
	assert(xffile.is_open());
	string xfmeg;
	int curwin = 0;
	getline(xffile, xfmeg);
	while (getline(xffile, xfmeg)) {
		if (xfmeg[0] == 'W') curwin++;
		else {
			Windows[curwin].xflist.push_back(
				XFMeg{
					xfmeg.substr(0, 7),
					xfmeg.substr(8, 8),
					xfmeg.substr(17, 8),
					stod(xfmeg.substr(26, xfmeg.size() - 27)),
					curwin,
					true,
					0
				}
			);
		}
	}
	end = clock();
	cout << "xffile read finish! cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	xffile.close();
}


void setConfig(string curtime, int curwindow) {
	curTime = curtime;
	curWindow = curwindow;
}
bool Consume(int window, string cardid, string date, string time, double val, XFMeg* xfmeg) {//�������ѵĵ����߼�
	string serialnum = cardid.substr(1, 5);
	string uid = CardID2ActMap[stoi(serialnum)];
	if (uid.size() != 10) return false;
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	//����Ƿ�Ϊ��Ч��
	if (!curAct->cardList.size || curAct->cardList.tail->pre->serialNum != serialnum || !curAct->cardList.tail->pre->valid) {
		//xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + "�ÿ�����Ч!", window);
		return false;
	}
	if (curAct->cardList.tail->pre->balance < val) {
		//cout << "������" << endl;
		//xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "������!", window);
		return false;
	}
	/*string curhour = time.substr(1, 2);
	if ((curhour >= "07" && curhour <= "09") || (curhour >= "11" && curhour <= "13") || (curhour >= "17" && curhour <= "19")) {

	}*/
	CardID* curCard = curAct->cardList.tail->pre;
	curAct->cardList.tail->pre->balance -= val;
	xfmeg->balance = curAct->cardList.tail->pre->balance;
	xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance));
	return true;
}

void sConsume(string cardid, double val) {
	string serialnum = cardid.substr(1, 5);
	string uid = CardID2ActMap[stoi(serialnum)];
	if (uid.size() != 10) {
		cout << "�ÿ��Ų�����" << endl;
		return;
	}
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	//����Ƿ�Ϊ��Ч��
	if (!curAct->cardList.size || curAct->cardList.tail->pre->serialNum != serialnum || !curAct->cardList.tail->pre->valid) {
		cout << "�ÿ�����Ч!" << endl;
		return;
	}
	if (curAct->cardList.tail->pre->balance < val) {
		cout << "������" << endl;
		return;
	}
	/*string curhour = time.substr(1, 2);
	if ((curhour >= "07" && curhour <= "09") || (curhour >= "11" && curhour <= "13") || (curhour >= "17" && curhour <= "19")) {

	}*/
	CardID* curCard = curAct->cardList.tail->pre;
	curAct->cardList.tail->pre->balance -= val;
	cout << "�ɹ�����" << val << endl;
	cout << "����:" << cardid << " ���:" << curCard->balance << endl;
	xflog(curTime + "����==>" + "����:" + cardid + " ����:" + to_string(curWindow) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance));
	sumMoney += val;
	sumTimes++;
	return;
}
void SumDay() {
	cout << curTime.substr(0, 8) << "���ۼ��շѽ��Ϊ" << sumMoney << " ����" << curWindow << "���ۼƽ��״���Ϊ" << curTime << endl;
	return;
}

void roundData() {
	for (int i = 1; i < 58; ++i) {
		int sum = 0;
		int n = Windows[i].xflist.size();
		int index = n - 1;
		while (index >= 0 && sum < min(60000, n)) {
			while (index >= 0 && Windows[i].xflist[index].valid != true) index--;
			sum++;
			index--;
		}
		//cout << sum << endl;
		for (int j = 0; j <= index; ++j) {
			Windows[i].xflist[j].valid = false;
		}
	}
	int sum = 0;
	for (int i = 1; i < 58; ++i) {
		int partsum = 0;
		for (auto &it : Windows[i].xflist) {
			if (it.valid) sum++, partsum++;
		}
		//cout << "����" << i << "�����ձ�������Ѽ�¼��" << partsum << "��" << endl;
	}
	cout << "�����������ֵ6W���ƺ�,���ձ�������Ѽ�¼��" << sum << "��" << endl;
}

void batchXF() {
	clock_t start, end;
	ifstream czfile;
	czfile.open("E:\\classdesign\\cz002.txt", ios::in);
	assert(czfile.is_open());
	string czmeg;
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) continue;
		else break;
	}
	//��ʼ9.4��ֵ
	cout << "��ʼ9.4��ֵ" << endl;
	start = clock();
	string cztime = czmeg.substr(0, 16);
	string cztype = czmeg.substr(17, 4);
	string stuid = czmeg.substr(22, 10);
	double val = stoi(czmeg.substr(33));
	int major = stoi(stuid.substr(4, 2));
	int order = stoi(stuid.substr(6, 3));
	Recharge(stuid, double(val), "");
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) break;
		else {
			cztime = czmeg.substr(0, 16);
			if (cztime.substr(4, 4) != "0904") break;
			cztype = czmeg.substr(17, 4);
			stuid = czmeg.substr(22, 10);
			val = stoi(czmeg.substr(33));
			major = stoi(stuid.substr(4, 2));
			order = stoi(stuid.substr(6, 3));
			Recharge(stuid, val, cztime);
		}
	}
	end = clock();
	cout << "9.4��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl << "9.6��ʧ������ʼ:" << endl;
	//����9.4��ֵ����ʼ9.6��ʧ��������
	start = clock();
	cztype = czmeg.substr(17, 4);
	stuid = czmeg.substr(22, 10);
	if (cztype == "��ʧ") {
		ReportLoss(stuid, cztime);
	}
	else if (cztype == "����") {
		MakeupCard(stuid, cztime);
	}
	while (getline(czfile, czmeg)) {
		if (czmeg.length() > 33) break;
		cztime = czmeg.substr(0, 16);
		cztype = czmeg.substr(17, 4);
		stuid = czmeg.substr(22, 10);
		if (cztype == "��ʧ") {
			ReportLoss(stuid, cztime);
		}
		else if (cztype == "����") {
			MakeupCard(stuid, cztime);
		}
	}
	string cardid, xfdate, xftime;
	int window;
	end = clock();
	cout << "9.6��������,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl << "��ʼ10.14ǰ������:" << endl;
	//����9.6��������ʼ��10.13�յ�����
	//search();
	start = clock();
	string path;
	ostringstream ostr;
	for (int i = 1; i < 58; ++i) {
		if (Windows[i].xflist[Windows[i].pointer].xfdate <= "20211013")
			minheap.push(&Windows[i].xflist[Windows[i].pointer++]);
	}
	while (!minheap.empty()) {
		XFMeg* oritime = minheap.top();
		minheap.pop();
		xfdate = oritime->xfdate;
		xftime = oritime->xftime;
		val = oritime->val;
		cardid = oritime->cardid;
		window = oritime->window;
		//Windows[window].pointer++;
		if (!Consume(window, cardid, xfdate, xftime, val, oritime)) {
			Windows[window].xflist[Windows[window].pointer - 1].valid = false;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].xfdate <= "20211013") {
			minheap.push(&Windows[window].xflist[Windows[window].pointer++]);
		}
	}
	end = clock();
	cout << "10.14ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "10.14�賿��ֵ��ʼ" << endl;
	start = clock();
	cztime = czmeg.substr(0, 16);
	cztype = czmeg.substr(17, 4);
	stuid = czmeg.substr(22, 10);
	val = stoi(czmeg.substr(33));
	major = stoi(stuid.substr(4, 2));
	order = stoi(stuid.substr(6, 3));
	Recharge(stuid, double(val), "");
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) break;
		else {
			cztime = czmeg.substr(0, 16);
			if (cztime.substr(4, 4) != "1014") break;
			cztype = czmeg.substr(17, 4);
			stuid = czmeg.substr(22, 10);
			val = stoi(czmeg.substr(33));
			major = stoi(stuid.substr(4, 2));
			order = stoi(stuid.substr(6, 3));
			Recharge(stuid, val, cztime);
		}
	}
	end = clock();
	cout << "10.14��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "��ʼ11.4ǰ������:" << endl;
	start = clock();
	for (int i = 1; i < 58; ++i) {
		if (Windows[i].xflist[Windows[i].pointer].xfdate <= "20211103")
			minheap.push(&Windows[i].xflist[Windows[i].pointer++]);
	}
	while (!minheap.empty()) {
		XFMeg* oritime = minheap.top();
		minheap.pop();
		xfdate = oritime->xfdate;
		xftime = oritime->xftime;
		val = oritime->val;
		cardid = oritime->cardid;
		window = oritime->window;
		if (!Consume(window, cardid, xfdate, xftime, val, oritime)) {
			Windows[window].xflist[Windows[window].pointer - 1].valid = false;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].xfdate <= "20211103") {
			minheap.push(&Windows[window].xflist[Windows[window].pointer++]);
		}
	}
	end = clock();
	cout << "11.4ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "11.4�賿��ֵ��ʼ:" << endl;
	start = clock();
	cztime = czmeg.substr(0, 16);
	cztype = czmeg.substr(17, 4);
	stuid = czmeg.substr(22, 10);
	val = stoi(czmeg.substr(33));
	major = stoi(stuid.substr(4, 2));
	order = stoi(stuid.substr(6, 3));
	Recharge(stuid, double(val), "");
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) break;
		else {
			cztime = czmeg.substr(0, 16);
			if (cztime.substr(4, 4) != "1104") break;
			cztype = czmeg.substr(17, 4);
			stuid = czmeg.substr(22, 10);
			val = stoi(czmeg.substr(33));
			major = stoi(stuid.substr(4, 2));
			order = stoi(stuid.substr(6, 3));
			Recharge(stuid, val, cztime);
		}
	}
	end = clock();
	cout << "11.04��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "��ʼ11.25ǰ������:" << endl;
	start = clock();
	for (int i = 1; i < 58; ++i) {
		if (Windows[i].xflist[Windows[i].pointer].xfdate <= "20211124")
			minheap.push(&Windows[i].xflist[Windows[i].pointer++]);
	}
	while (!minheap.empty()) {
		XFMeg* oritime = minheap.top();
		minheap.pop();
		xfdate = oritime->xfdate;
		xftime = oritime->xftime;
		val = oritime->val;
		cardid = oritime->cardid;
		window = oritime->window;
		if (!Consume(window, cardid, xfdate, xftime, val, oritime)) {
			Windows[window].xflist[Windows[window].pointer - 1].valid = false;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].xfdate <= "20211124") {
			minheap.push(&Windows[window].xflist[Windows[window].pointer++]);
		}
	}
	end = clock();
	cout << "11.25ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "11.25�賿��ֵ��ʼ:" << endl;
	start = clock();
	cztime = czmeg.substr(0, 16);
	cztype = czmeg.substr(17, 4);
	stuid = czmeg.substr(22, 10);
	val = stoi(czmeg.substr(33));
	major = stoi(stuid.substr(4, 2));
	order = stoi(stuid.substr(6, 3));
	Recharge(stuid, double(val), "");
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) break;
		else {
			cztime = czmeg.substr(0, 16);
			if (cztime.substr(4, 4) != "1125") break;
			cztype = czmeg.substr(17, 4);
			stuid = czmeg.substr(22, 10);
			val = stoi(czmeg.substr(33));
			major = stoi(stuid.substr(4, 2));
			order = stoi(stuid.substr(6, 3));
			Recharge(stuid, val, cztime);
		}
	}
	end = clock();
	cout << "11.25��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "��ʼ12.16ǰ������:" << endl;
	start = clock();
	for (int i = 1; i < 58; ++i) {
		if (Windows[i].xflist[Windows[i].pointer].xfdate <= "20211215")
			minheap.push(&Windows[i].xflist[Windows[i].pointer++]);
	}
	while (!minheap.empty()) {
		XFMeg* oritime = minheap.top();
		minheap.pop();
		xfdate = oritime->xfdate;
		xftime = oritime->xftime;
		val = oritime->val;
		cardid = oritime->cardid;
		window = oritime->window;
		if (!Consume(window, cardid, xfdate, xftime, val, oritime)) {
			Windows[window].xflist[Windows[window].pointer - 1].valid = false;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].xfdate <= "20211215") {
			minheap.push(&Windows[window].xflist[Windows[window].pointer++]);
		}
	}
	end = clock();
	cout << "12.16ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "12.16�賿��ֵ��ʼ:" << endl;
	start = clock();
	cztime = czmeg.substr(0, 16);
	cztype = czmeg.substr(17, 4);
	stuid = czmeg.substr(22, 10);
	val = stoi(czmeg.substr(33));
	major = stoi(stuid.substr(4, 2));
	order = stoi(stuid.substr(6, 3));
	Recharge(stuid, double(val), "");
	while (getline(czfile, czmeg)) {
		if (czmeg.length() <= 33) break;
		else {
			cztime = czmeg.substr(0, 16);
			if (cztime.substr(4, 4) != "1216") break;
			cztype = czmeg.substr(17, 4);
			stuid = czmeg.substr(22, 10);
			val = stoi(czmeg.substr(33));
			major = stoi(stuid.substr(4, 2));
			order = stoi(stuid.substr(6, 3));
			Recharge(stuid, val, cztime);
		}
	}
	end = clock();
	cout << "12.16��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	cout << "��ʼ12.31(����)֮ǰ������:" << endl;
	start = clock();
	for (int i = 1; i < 58; ++i) {
		if (Windows[i].xflist[Windows[i].pointer].xfdate <= "20211231")
			minheap.push(&Windows[i].xflist[Windows[i].pointer++]);
	}
	while (!minheap.empty()) {
		XFMeg* oritime = minheap.top();
		minheap.pop();
		xfdate = oritime->xfdate;
		xftime = oritime->xftime;
		val = oritime->val;
		cardid = oritime->cardid;
		window = oritime->window;
		if (!Consume(window, cardid, xfdate, xftime, val, oritime)) {
			Windows[window].xflist[Windows[window].pointer - 1].valid = false;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].xfdate <= "20211231") {	//��ֹԽ��
			minheap.push(&Windows[window].xflist[Windows[window].pointer++]);
		}
	}
	end = clock();
	cout << "12.31(����)ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	czfile.close();
	int sumLog = 0, successLog = 0;
	for (int i = 1; i < 58; ++i) {
		//int sumwindow = 0;
		for (auto &it : Windows[i].xflist) {
			if (it.valid) {
				++successLog;
				//++sumwindow;
			}
			++sumLog;
		}
		//windowslog("����" + to_string(i) + "û�л���ǰ�ĳɹ����Ѽ�¼��" + to_string(sumwindow));
	}
	cout << "sumLog:" << sumLog << " successLog:" << successLog << endl;
	roundData();
	for (int i = 1; i < 58; ++i) {
		int sumwindow = 0;
		for (auto &it : Windows[i].xflist) {
			if (it.valid) {
				++successLog;
				++sumwindow;
			}
			++sumLog;
		}
		windowslog("����" + to_string(i) + "���ƺ�ĳɹ����Ѽ�¼��" + to_string(sumwindow));
	}
	return;
}

