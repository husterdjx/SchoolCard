#include<iostream>
#include<fstream>
#include "def.h"
#include<cassert>
#include<queue>
#include<time.h>
#include<unordered_map>
#include<io.h>
#include <sstream>
using namespace std;
string curTime;
int curWindow;
vector<WindowManager> Windows(100);
double sumMoney;
double sumMoneySec;
int sumTimes;
bool flag;
int batchPreDur;
string batchPreDate;
unordered_map<string, vector<double>> CardFlag;		//����Ƿ񳬹���������
double daysum[60];	//�������ͳ��
extern string CardID2ActMap[CARD_MAX_NUM];		//��cardidת��Ϊ��Ӧ��У԰���˻�ѧ��
extern CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];		//����У԰���˻������飬��һά��ʾרҵ�ţ��ڶ�ά��ʾרҵ�ڵ���ˮ��
extern fstream xflogWriter, czlogWriter;		//���Ѽ�¼�Ͳ�����¼��Writer������д��־
extern vector<XFMeg*> QuickSortVector;			//��ʱ��˳��ĳɹ����Ѽ�¼
/*��time��Ӧ��ʱ���ת��Ϊ������±�*/
int change(string time) {
	if (time >= "07" && time <= "09") return 0;
	else if (time >= "11" && time <="13") return 1;
	else if (time >= "17" && time <= "19") return 2;
	else return 3;
}
/*����CardFlag��ÿ�����Ѷ���Ҫ���¼�飬���ж��Ƿ���Ҫ����*/
void checkFlag(string cardid, string date, string time, double val, XFMeg* meg) {
	if (!CardFlag.count(cardid)) {
		CardFlag[cardid] = vector<double>{0, 0, 0};
		int index = change(time.substr(0, 2));
		if (index == 3) {
			cout << "ʱ��δ���" << endl;
			system("pause");
			return;
		}
		CardFlag[cardid][index] += val;
		if (CardFlag[cardid][index] > 20) {
			meg->secretneed = true;
			return;
		}
	}
	else {
		int index = change(time.substr(0, 2));
		if (index == 3) {
			cout << "ʱ��δ���" << endl;
			system("pause");
			return;
		}
		CardFlag[cardid][index] += val;
		if (CardFlag[cardid][index] > 20) {
			meg->secretneed = true;
			return;
		}
	}
}
/*����minheapС���ѵ������������˳��ִ�в�ͬ�ֽ������Ѽ�¼*/
struct cmp
{
	bool operator()(XFMeg *a, XFMeg *b) const
	{
		return a->xfdate + a->xftime > b->xfdate + b->xftime;
	}
};
/*����˳��ִ�в�ͬ�ֽ������Ѽ�¼*/
priority_queue<XFMeg*, vector<XFMeg*>, cmp> minheap;
/*��ʼ�����д���Manager��Ϣ*/
void initWindows() {
	for (int i = 1; i < 58; ++i) {
		Windows[i].num = i;
		Windows[i].pointer = 0;
		Windows[i].xflist.clear();
	}
}
/*��xf014.txt��ȡ����*/
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
					0,
					false
				}
			);
		}
	}
	end = clock();
	cout << "xffile read finish! cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	xffile.close();
}
/*�������õ�ǰ�ֹ����������ʱ������Ѵ���*/
void setConfig(string curtime, int curwindow) {
	curTime = curtime;
	curWindow = curwindow;
}
/*�������ѵĵ����߼�*/
bool Consume(int window, string cardid, string date, string time, double val, XFMeg* xfmeg) {//�������ѵĵ����߼�
	if (date != batchPreDate || change(time) != batchPreDur) {
		CardFlag.clear();
		batchPreDate = date;
		batchPreDur = change(time);
	}
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
	checkFlag(cardid, date, time, val, xfmeg);
	CardID* curCard = curAct->cardList.tail->pre;
	curAct->cardList.tail->pre->balance -= val;
	xfmeg->balance = curAct->cardList.tail->pre->balance;
	if (xfmeg->secretneed) {
		xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance) + "��Ҫ��������!");
		int index = change(time.substr(0, 2));
		CardFlag[cardid][index] = 0;
	}
	else {
		xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance));
	}
	return true;
}
/*�ֹ��������ѵĵ����߼��������Ҫ����־��д����*/
bool sConsume(int window, string cardid, string date, string time, double val, XFMeg* xfmeg) {

	string serialnum = cardid.substr(1, 5);
	string uid = CardID2ActMap[stoi(serialnum)];
	if (uid.size() != 10) return false;
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	//����Ƿ�Ϊ��Ч��
	if (!curAct->cardList.size || curAct->cardList.tail->pre->serialNum != serialnum || !curAct->cardList.tail->pre->valid) {
		cout << "�ÿ�����Ч" << endl;
		//xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + "�ÿ�����Ч!", window);
		return false;
	}
	if (curAct->cardList.tail->pre->balance < val) {
		cout << "������" << endl;
		//xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "������!", window);
		return false;
	}
	checkFlag(cardid, date, time, val, xfmeg);
	CardID* curCard = curAct->cardList.tail->pre;
	curAct->cardList.tail->pre->balance -= val;
	xfmeg->balance = curAct->cardList.tail->pre->balance;
	if (xfmeg->secretneed) {
		cout << "��ǰʱ������ѳ���20Ԫ,������6λ����" << endl;
		string secret;
		while (cin >> secret) {
			if (secret.size() == 6) {
				cout << "����������ȷ" << endl;
				break;
			}
			else {
				cout << "�������,����������" << endl;
			}
		}
		xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance) + "��Ҫ��������!");
		int index = change(time.substr(0, 2));
		CardFlag[cardid][index] = 0;
	}
	else {
		xflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(curAct->cardList.tail->pre->balance));
	}
	return true;
}
/*������������������ݻ��ƣ��ӿ�ͷ���������validֵ��0*/
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
/*�������ѵ���Ҫ����������ʱ��˳��ִ�в�ͬ�ֶε����ѡ���ֵ������������*/
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
	//cout << "��ʼ9.4��ֵ" << endl;
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
	//end = clock();
	//cout << "9.4��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl << "9.6��ʧ������ʼ:" << endl;
	//����9.4��ֵ����ʼ9.6��ʧ��������
	//start = clock();
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
	//end = clock();
	//cout << "9.6��������,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl << "��ʼ10.14ǰ������:" << endl;
	//����9.6��������ʼ��10.13�յ�����
	//search();
	//start = clock();
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
	//end = clock();
	//cout << "10.14ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "10.14�賿��ֵ��ʼ" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "10.14��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "��ʼ11.4ǰ������:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "11.4ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "11.4�賿��ֵ��ʼ:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "11.04��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "��ʼ11.25ǰ������:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "11.25ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "11.25�賿��ֵ��ʼ:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "11.25��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "��ʼ12.16ǰ������:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "12.16ǰ���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "12.16�賿��ֵ��ʼ:" << endl;
	//start = clock();
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
	//end = clock();
	//cout << "12.16��ֵ����,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
	//cout << "��ʼ12.31(����)֮ǰ������:" << endl;
	//start = clock();
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
	cout << "���ѽ���,cost:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
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
	system("pause");
	return;
}
/*ʳ�ù���ģ��Ľӿ�*/
void ShowCartoon() {
	int choice;
	cout << "����ʳ�ù���ģ��,��ѡ������Ҫ�Ĳ���:" << endl;
	while (1) {
		cout << "1.����У԰��������" << endl;
		cout << "2.��������(���ӵ�һ��ģ�����������������)" << endl;
		cout << "3.�˳�����" << endl;
		cin >> choice;
		if (choice == 1) {
			system("cls");
			cout << "�����ֹ���������ģʽ" << endl;
			string predate;
			bool flag = 0;
			while (1) {
				cout << "�����뵱ǰʱ��ʹ���,ʱ��Ϊ��һ��,����Ϊ�ڶ���,�˳�����������/��ĸ��+�س�" << endl;
				string time;
				int window;
				cin >> time;
				if (time.size() == 1) {
					cout << "�˳�!" << endl;
					system("pause");
					break;
				}
				cin >> window;
				setConfig(time, window);
				cout << "���õ�ǰʱ��Ϊ" << curTime << " ��ǰ����Ϊ" << curWindow << endl;
				string cardid;
				double val;
				cout << "�����뿨�ź����ѽ��,�����ڵ�һ��,���ѽ���ڵڶ���" << endl;
				cin >> cardid >> val;
				XFMeg curMeg = {
					cardid,
					curTime.substr(0, 8),
					curTime.substr(9, 8),
					val,
					curWindow,
					true,
					0,
					false
				};
				Windows[curWindow].xflist.push_back(curMeg);
				if (!sConsume(curWindow, cardid, curTime.substr(0, 8), curTime.substr(9, 8), val, &curMeg)) {
					curMeg.valid = false;
				}
				if (!flag && curMeg.valid) {
					flag = 1;
					predate = curMeg.xfdate;
					daysum[curMeg.window] += curMeg.val;
				}
				else if (flag && curMeg.xfdate == predate && curMeg.valid) {
					daysum[curMeg.window] += curMeg.val;
				}
				else if (flag && curMeg.valid && curMeg.xfdate != predate) {
					for (int i = 1; i < 58; ++i) {
						if (daysum[i] > 0) cout << "����" << i << "������" << predate << "���ܳɹ����ѽ��Ϊ" << daysum[i] << endl;
						daysum[i] = 0;
					}
					daysum[curMeg.window] += curMeg.val;
					predate = curMeg.xfdate;
				}
				system("pause");
			}
			QuickSort();
			int sumSuccessLog = QuickSortVector.size();
			for (int j = 1; j < 58; ++j) {
				if (daysum[j] > 0) {
					cout << "����" << j << "������" << QuickSortVector[QuickSortVector.size() - 1]->xfdate << "�������Ѽ�¼���Ϊ" << daysum[j] << endl;
					daysum[j] = 0;
				}
			}
			cout << "����" << sumSuccessLog << "�ɹ������Ѽ�¼" << endl;
			system("cls");
			/*string predate = QuickSortVector[0]->xfdate;
			for (int i = 0; i < QuickSortVector.size(); ++i) {
				if (!QuickSortVector[i]->valid) continue;
				else {
					if (QuickSortVector[i]->xfdate != predate) {
						for (int j = 1; j < 58; ++j) {
							if (daysum[j] > 0) {
								cout << "����" << j << "������" << predate << "�������Ѽ�¼���Ϊ" << daysum[j] << endl;
								daysum[j] = 0;
							}
						}
						predate = QuickSortVector[i]->xfdate;
						daysum[QuickSortVector[i]->window] = QuickSortVector[i]->val;
					}
					else {
						daysum[QuickSortVector[i]->window] += QuickSortVector[i]->val;
					}
				}
			}
			for (int j = 1; j < 58; ++j) {
				if (daysum[j] > 0) {
					cout << "����" << j << "������" << QuickSortVector[QuickSortVector.size() - 1]->xfdate << "�������Ѽ�¼���Ϊ" << daysum[j] << endl;
					daysum[j] = 0;
				}
			}
			*/
		}
		else if (choice == 2) {
			system("cls");
			QuickSortVector.clear();
			initWindows();
			read2XF();
			batchXF();
			cout << "�˳�ʳ�ù���ģ��" << endl;
			system("pause");
			system("cls");
			return;
		}
		else if (choice == 3) {
			cout << "�˳�ʳ�ù���ģ��" << endl;
			system("pause");
			system("cls");
			return;
		}
	}
}