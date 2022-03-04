#include"def.h"
#include<iostream>
#include<queue>
#include<time.h>
#include<unordered_map>
#include <functional>
using namespace std;

extern fstream xflogReader;
extern vector<WindowManager> Windows;
extern string CardID2ActMap[CARD_MAX_NUM];
extern CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];
extern size_t LogHash[MAX_LOG_NUM];

vector<XFMeg*> QuickSortVector;
/*����multiMergeHeap��Ҫ�����������ʾС����*/
struct cmpp
{
	bool operator()(XFMeg *a, XFMeg *b) const
	{
		return a->xfdate + a->xftime > b->xfdate + b->xftime;
	}
};
/*����DataDig��TimesHeap��Ҫ�����������ʾ�󶥶�*/
struct compare
{
	bool operator()(pair<string, int> a, pair<string, int> b) const
	{
		return b.second < a.second;
	}
};

priority_queue<XFMeg*, vector<XFMeg*>, cmpp> multiMergeHeap;	//��·�鲢������Ҫ��С���ѣ���ʱ������
/*��WindowsManager�����м�¼��������Ч�����Ѽ�¼��������*/
void MultipleMerge() {
	//��·�鲢���򣬽����д��ڵ���Ϣ4
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

		//sortxflog(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(oritime->balance));
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
/*QuickSort�ķ����߼�*/
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
/*��������WindowsManager�����м�¼��������Ч���Ѽ�¼*/
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
		//sortxflog_quick_sort(date + "," + time + "����==>" + "����:" + cardid + " ����:" + to_string(window) + " ���:" + to_string(val) + "���ѳɹ�! ���" + to_string(balance));
	}
	end = clock();
	cout << "�����������Ѽ�¼���! ��ʱ:" << (double)(end - start) / CLOCKS_PER_SEC << "��" << endl;
	return;
}
/*��QuickSortVector���ҳ����ɶ�ʱ��β���ѧ��ƥ������Ѽ�¼��������vector*/
vector<XFMeg*> sumDuration(string startDateTime, string endDateTime, string uid) {	//��ѯ��ֹ����������ѧ�ŵ���������
	vector<XFMeg*> ret;
	int startIndex = 0, endIndex;
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
/*��ʱ��һ��int�����������������ھ�Ƚ���������ʱ��*/
int time2int(string time) {
	int sum = 0;
	sum += time[7] - '0';
	sum += 10 * (time[6] - '0');
	sum += 100 * (time[5] - '0');
	sum += 1000 * (time[4] - '0');
	sum += 6000 * (time[3] - '0');
	sum += 60000 * (time[2] - '0');
	sum += 360000 * (time[1] - '0');
	sum += 3600000 * (time[0] - '0');
	return sum;
}
/*�����ھ��߼����ֻ�ָ��ѧ�ŵ�ÿ�����ѵ�ǰ������ӵ������������ڵ��������Ѽ�¼��Ȼ��ȡ��ߵ�ʮ���ˣ��϶�Ϊ�й�*/
void DataDig(string uid) {
	vector<int> DataDigVector;
	for (int i = 0; i < QuickSortVector.size(); ++i) {
		if (CardID2ActMap[stoi(QuickSortVector[i]->cardid.substr(1, 5))] == uid) DataDigVector.push_back(i);
	}
	unordered_map<string, int> times;
	cout << "��ѧ��һ����" << DataDigVector.size() << "�����Ѽ�¼" << endl;
	for (int i = 0; i < DataDigVector.size(); ++i) {
		int backIndex = DataDigVector[i] - 1, frontIndex = DataDigVector[i] + 1;
		while (backIndex >= 0) {
			if (abs(QuickSortVector[backIndex]->window - QuickSortVector[DataDigVector[i]]->window) <= 2 && time2int(QuickSortVector[DataDigVector[i]]->xftime) - time2int(QuickSortVector[backIndex]->xftime) <= 30000) {
				times[CardID2ActMap[stoi(QuickSortVector[backIndex]->cardid.substr(1, 5))]] ++;
			}
			else if (time2int(QuickSortVector[DataDigVector[i]]->xftime) - time2int(QuickSortVector[backIndex]->xftime) > 30000) {
				break;
			}
			backIndex--;
		}
		while (frontIndex < QuickSortVector.size()) {
			if (abs(QuickSortVector[frontIndex]->window - QuickSortVector[DataDigVector[i]]->window) <= 2 && time2int(QuickSortVector[frontIndex]->xftime) - time2int(QuickSortVector[DataDigVector[i]]->xftime) <= 30000) {
				times[CardID2ActMap[stoi(QuickSortVector[frontIndex]->cardid.substr(1, 5))]] ++;
			}
			else if (time2int(QuickSortVector[frontIndex]->xftime) - time2int(QuickSortVector[DataDigVector[i]]->xftime) > 30000) {
				break;
			}
			frontIndex++;
		}
	}
	priority_queue<pair<string, int>, vector<pair<string, int>>, compare> TimesHeap;
	for (auto &it : times) {
		TimesHeap.push({ it.first, it.second });
		if (TimesHeap.size() > 10) TimesHeap.pop();
	}
	cout << "���������ھ��,�ó��Ŀ����й�ѧ��Ϊ:" << endl;
	while (!TimesHeap.empty()) {
		pair<string, int> st = TimesHeap.top();
		TimesHeap.pop();
		cout << st.first << ":" << st.second << endl;
	}
	cout << endl;
	system("pause");
	return;
}
/*HashУ�鷽ʽ��Hash����*/
size_t MyHash(string s) {
	std::hash<std::string> h;
	size_t n = h(s);
	return n;
}
/*�����ж�ָ����Χ�е����Ѽ�¼�Ƿ񱻴۸Ĳ��ڿ���̨���*/
void checkLines(int startline, int endline) {
	string meg;
	int line = startline;
	while (getline(xflogReader, meg)) {
		size_t meg_hash = MyHash(meg);
		if (meg_hash != LogHash[line++]) {
			cout << "У������֤ʧ��," << line - 1 << "�������ѱ��۸�" << endl;
		}
		else {
			cout << "У������֤�ɹ�," << line - 1 << "������δ���۸�" << endl;
		}
		if (line > endline) break;
	}
	return;
}
/*searchģ��Ľӿ�*/
void search() {
	string choice;
	cout << "����ͳ�Ʋ�ѯģ��,����Ϊ���ܲ˵�:" << endl;
	while (1) {
		cout << "1.�ṩ��������ѧ�Ų�ѯ�˻�(֧��ģ��ƥ��)" << endl;
		cout << "2.�ṩʱ��Ρ�ѧ�š����������ѽ�Χ�����Ѽ�¼��ѯ" << endl;
		cout << "3.��·�鲢����(����)" << endl;
		cout << "4.��������" << endl;
		cout << "5.���������ھ�" << endl;
		cout << "6.����У����˶�" << endl;
		cout << "7.У��������־ָ����Χ��" << endl;
		cout << "8.�˳���ǰģ��" << endl;
		cin >> choice;
		if (choice.size() == 1 && choice[0] == '1') {
			system("cls");
			cout << "�������ѯ��ѧ�Ż�������,������ͷΪ@��ѧ�ſ�ͷΪ&" << endl;
			string user_pattern;
			cin >> user_pattern;
			bool flag = (user_pattern[0] == '@') ? true : false;
			BlurSearch(user_pattern.substr(1), flag);
			system("pause");
		}
		else if (choice[0] == '2') {
			system("cls");
			string meg;
			cout << "�������ѯ��ʱ��Ρ�ѧ�š����������ѽ�Χ,��ʽΪ:��ʼʱ��-��ֹʱ��,ѧ��,����,������ͽ��-������߽��" << endl;
			cin >> meg;
			string startDate = meg.substr(0, 8);
			string startTime = meg.substr(9, 8);
			string endDate = meg.substr(18, 8);
			string endTime = meg.substr(27, 8);
			string uid = meg.substr(36, 10);
			int index1 = meg.find_last_of(",");
			string name = meg.substr(47, index1 - 47);
			int index2 = meg.find_last_of("-");
			double minMoney = stod(meg.substr(index1 + 1, index2 - index1 - 1));
			double maxMoney = stod(meg.substr(index2 + 1));
			cout << "��ѯ��Ϣ����:" << endl;
			cout << startDate << startTime << "��" << endDate << endTime << " ѧ��Ϊ" << uid << " ����Ϊ" << name << " ���ѽ��Ϊ" << minMoney << "-" << maxMoney << endl;
			int major = stoi(uid.substr(4, 2));
			int order = stoi(uid.substr(6, 3));
			CardAct* curAct = &AllAccount[major][order];
			if (!checkActValid(uid) || curAct->name != name) {
				cout << "ѧ����������������" << endl;
				system("pause");
				continue;
			}
			vector<XFMeg*> sumVector = sumDuration(startDate + startTime, endDate + endTime, uid);
			for (auto &it : sumVector) {
				if (it->val >= minMoney && it->val <= maxMoney)
					cout << it->xfdate << "," << it->xftime << "����:" << it->cardid << " ����:" << it->window << " ���ѽ��:" << it->val << " ���:" << it->balance << endl;
			}
			system("pause");
		}
		else if (choice[0] == '3') {
			MultipleMerge();
			system("pause");
		}
		else if (choice[0] == '4') {
			QuickSort();
			system("pause");
		}
		else if (choice[0] == '5') {
			system("cls");
			cout << "��������Ҫ������ѧ��:" << endl;
			string meg;
			cin >> meg;
			DataDig(meg);
		}
		else if (choice[0] == '6') {
			system("cls");
			string meg;
			cout << "��������Ҫ��������Ѽ�¼�������ִ�����" << endl;
			cin >> meg;
			int line = stoi(meg);
			cin.ignore();
			getline(cin, meg);
			//cout << line << endl << meg << endl;
			size_t meg_hash = MyHash(meg);
			if (meg_hash != LogHash[line]) {
				cout << "У������֤ʧ��,���������ѱ��۸�" << endl;
			}
			else {
				cout << "У������֤�ɹ�,��������δ���۸�" << endl;
			}
			system("pause");
		}
		else if (choice[0] == '7') {
			system("cls");
			xflogReader.open("E:\\classdesign\\xflog.txt", ios::in);
			assert(xflogReader.is_open());
			int line1, line2;
			cout << "��������ֹ������е�����,���ո��س��ָ�:" << endl;
			cin >> line1 >> line2;
			cout << "����У��" << line1 << "��" << line2 << "�е��������Ѽ�¼" << endl;
			checkLines(line1, line2);
			xflogReader.close();
			system("pause");
		}
		else if (choice[0] == '8') {
			return;
		}
		system("cls");
	}
	return;
}