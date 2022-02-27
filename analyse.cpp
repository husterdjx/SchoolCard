#include"def.h"
#include<iostream>
#include<queue>
#include<time.h>
using namespace std;

extern vector<WindowManager> Windows;
extern string CardID2ActMap[CARD_MAX_NUM];
extern CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];

vector<XFMeg*> QuickSortVector;
struct cmpp
{
	bool operator()(XFMeg *a, XFMeg *b) const
	{
		return a->xfdate + a->xftime > b->xfdate + b->xftime;
	}
};
priority_queue<XFMeg*, vector<XFMeg*>, cmpp> multiMergeHeap;
void MultipleMerge() {
	clock_t start, end;
	start = clock();
	for (int i = 0; i < 58; ++i) {
		Windows[i].pointer = 0;
	}
	for (int i = 1; i < 58; ++i) {
		while (!Windows[i].xflist[Windows[i].pointer].valid) Windows[i].pointer++;	//û���жϱ߽��ǰ�ȫ��
		multiMergeHeap.push(&Windows[i].xflist[Windows[i].pointer]);
		Windows[i].pointer++;
	}
	int window;
	string date, time, cardid;
	double val;
	CardAct* curAct;
	while (!multiMergeHeap.empty()) {
		XFMeg* oritime = multiMergeHeap.top();
		multiMergeHeap.pop();

		window = oritime->window;
		cardid = oritime->cardid;
		time = oritime->xftime;
		date = oritime->xfdate;
		val = oritime->val;

		string serialnum = cardid.substr(1, 5);
		string uid = CardID2ActMap[stoi(serialnum)];
		if (uid.size() != 10) {
			cout << "Error!" << cardid << endl;
			return;
		}
		int major = stoi(uid.substr(4, 2));
		int order = stoi(uid.substr(6, 3));
		curAct = &AllAccount[major][order];

		sortxflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(oritime->balance));
		while (Windows[window].pointer < Windows[window].xflist.size() && !(Windows[window].xflist[Windows[window].pointer].valid)) {
			Windows[window].pointer++;
		}
		if (Windows[window].pointer < Windows[window].xflist.size() && Windows[window].xflist[Windows[window].pointer].valid) {
			multiMergeHeap.push(&Windows[window].xflist[Windows[window].pointer]);
			Windows[window].pointer++;
		}
	}
	end = clock();
	cout << "��·�鲢�������Ѽ�¼���! ��ʱ:" << (double)(end - start) / CLOCKS_PER_SEC << "��" << endl;
	return;
}

void quick_sort(int l, int r)
{
	if (l >= r) return;

	int i = l - 1, j = r + 1;
	XFMeg* x = QuickSortVector[l + r >> 1];
	while (i < j)
	{
		do i++; while (QuickSortVector[i]->xfdate + QuickSortVector[i]->xftime < x->xfdate + x->xftime);
		do j--; while (QuickSortVector[j]->xfdate + QuickSortVector[j]->xftime > x->xfdate + x->xftime);
		if (i < j) swap(QuickSortVector[i], QuickSortVector[j]);
	}
	quick_sort(l, j), quick_sort(j + 1, r);
}

void QuickSort() {
	clock_t start, end;
	start = clock();
	for (int i = 1; i < 58; ++i) {
		for (auto &it : Windows[i].xflist) {
			if (it.valid) QuickSortVector.push_back(&it);
		}
	}
	quick_sort(0, QuickSortVector.size() - 1);
	string date, time, cardid;
	double val, balance;
	int window;
	for (auto &it : QuickSortVector) {
		date = it->xfdate;
		time = it->xftime;
		cardid = it->cardid;
		val = it->val;
		window = it->window;
		balance = it->balance;
		sortxflog_quick_sort(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(balance));
	}
	end = clock();
	cout << "�����������Ѽ�¼���! ��ʱ:" << (double)(end - start) / CLOCKS_PER_SEC << "��" << endl;
	return;
}

vector<XFMeg*> sumDuration(string startDateTime, string endDateTime, string uid) {
	vector<XFMeg*> ret;
	int startIndex = 0, endIndex;
	XFMeg* curMeg;
	while (QuickSortVector[startIndex]->xfdate + QuickSortVector[startIndex]->xftime < startDateTime) {
		startIndex++;
	}
	endIndex = startIndex;
	while (QuickSortVector[endIndex]->xfdate + QuickSortVector[endIndex]->xftime <= endDateTime) {
		endIndex = min(endIndex + 1, int(QuickSortVector.size()) - 1);
		if (endIndex == int(QuickSortVector.size()) - 1) break;
	}
	while (startIndex <= endIndex) {
		string serialnum = QuickSortVector[startIndex]->cardid.substr(1, 5);
		string curuid = CardID2ActMap[stoi(serialnum)];
		if (curuid != uid) {	//��֤�Ƿ��ǲ�ѯ��ѧ�Ŷ�Ӧ�Ŀ�
			startIndex++;
			continue;
		}
		ret.push_back(QuickSortVector[startIndex]);
		startIndex++;
	}
	return ret;
}

void search() {
	string choice;
	cout << "�����ѯģ��,����Ϊ���ܲ˵�:" << endl;
	cout << "1.�ṩ��������ѧ�Ų�ѯ�˻�(֧��ģ��ƥ��)" << endl;
	cout << "2.�ṩʱ��Ρ�ѧ�š����������ѽ�Χ�����Ѽ�¼��ѯ" << endl;
	cin >> choice;
	if (choice.size() == 1 && choice[0] == '1') {
		system("clear");
		cout << "�������ѯ��ѧ�Ż�������,������ͷΪ@��ѧ�ſ�ͷΪ&" << endl;
		string user_pattern;
		cin >> user_pattern;
		bool flag = (user_pattern[0] == '@') ? true : false;
		BlurSearch(user_pattern.substr(1), flag);
	}
	else if (choice[0] == '2') {
		system("clear");
		string meg;
		cout << "�������ѯ��ʱ��Ρ�ѧ�š����������ѽ�Χ,��ʽΪ:��ʼʱ��-��ֹʱ��,ѧ��,����,������ͽ��-������߽��" << endl;
		cin >> meg;
		string startDate = meg.substr(0, 8);
		string startTime = meg.substr(9, 8);
		string uid = meg.substr(18, 8);
		string name = meg.substr(27, 8);
		int index = meg.find_last_of("-");
		double minMoney = stod(meg.substr(36, index - 36));
		double maxMoney = stod(meg.substr(index + 1));
		cout << startDate << " " << startTime << " " << uid << " " << name << " " << minMoney << "-" << maxMoney << endl;
	}
	return;
}