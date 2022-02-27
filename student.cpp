#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include "def.h"
#include <sstream>
#include <iomanip>
#include <regex>
#include<vector>

using namespace std;

CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];

string CardID2ActMap[CARD_MAX_NUM];

int cardSeq = 12346;

bool checkActValid(string uid) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		return false;
	}
	return true;
}

bool checkCardValid(CardList cardlist) {
	if (cardlist.size == 0) {
		return false;
	}
	return true;
}
void showActMeg(string uid) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	cout << "Name:" << curAct->name << ' ' << "Stuid:" << curAct->stuID << " " << "Status" << curAct->status << endl;
	cout << "CardList:" << endl;
	if (checkCardValid(AllAccount[major][order].cardList)) {
		CardID* curCard = AllAccount[major][order].cardList.tail->pre;
		cout << "cardid:" << curCard->headNum << curCard->serialNum << curCard->checkCode << " " << endl;
		//curCard = curCard->next;
		//if (curCard != AllAccount[major][order].cardList.tail) cout << "��" << endl;
	}
	else {
		cout << "��ǰ�˺�ȫ������Ч" << endl;
	}
	return;
}
void initAccount(string uid, string name, int major, int order, string cztime) {
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->validTime == VALID_TIME) {
		// ����ʱѧ�ŵ�Ψһ�Լ���߼�
		GenerateLog(string("����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:��ѧ���Ѵ����˻�!"));
		return;
	}
	
	curAct->cardList.head = (CardID*)malloc(sizeof(CardID));
	curAct->cardList.tail = (CardID*)malloc(sizeof(CardID));
	curAct->name = name;
	curAct->status = OK;	//��������Ч��
	curAct->stuID = uid;
	curAct->validTime = VALID_TIME;
	curAct->cardList.head->next = curAct->cardList.tail;
	curAct->cardList.tail->pre = curAct->cardList.head;
	curAct->cardList.size = 0;
	GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" �����ɹ�!"));
	return;
}
string generateCardSerial() {
	string ret = to_string(cardSeq);
	cardSeq++;
	return ret;
}
void SendCardID(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:��ѧ�Ŷ�Ӧ�˺Ų�����!"));
		return;
	}
	string cardserial = generateCardSerial();
	CardID* newcard = new CardID;
	CardID2ActMap[stoi(cardserial)] = uid;	
	newcard->headNum = 3;
	newcard->serialNum = cardserial;
	newcard->valid = true;
	newcard->balance = 0;
	int sum = 3;
	for (char it : cardserial) {
		sum += it - '0';
	}

	sum %= 10;
	newcard->checkCode = 9 - sum;
	//�ϳ����µĿ�������Ч��
	AllAccount[major][order].cardList.tail->pre->valid = false;

	AllAccount[major][order].cardList.size++;
	//�ƶ�������β��
	newcard->pre = AllAccount[major][order].cardList.tail->pre;
	AllAccount[major][order].cardList.tail->pre->next = newcard;
	newcard->next = AllAccount[major][order].cardList.tail;
	AllAccount[major][order].cardList.tail->pre = newcard;

	GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����:") + to_string(newcard->headNum) + newcard->serialNum + to_string(newcard->checkCode) + string(" �����ɹ�!"));
	return;
}
void CreateAct(string uid, string name, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	initAccount(uid, name, major, order, cztime);
	SendCardID(uid, cztime);
	//��־������initAct��
	return;
}
void readKHFile() {
	ifstream khfile;
	khfile.open("E:\\classdesign\\kh001.txt", ios::in);
	assert(khfile.is_open());
	string khmeg;
	getline(khfile, khmeg);
	//getline(khfile, khmeg);
	while (getline(khfile, khmeg)) {
		string stuid = khmeg.substr(0, 10);
		string stuname = khmeg.substr(11, khmeg.size() - 12);
		//cout << stuname << endl;
		int major = stoi(stuid.substr(4, 2));
		int order = stoi(stuid.substr(6, 3));
		initAccount(stuid, stuname, major, order, "2021090109220510");
		SendCardID(stuid, "2021090109220510");
		//showActMeg(stuid);
	}
	khfile.close();
	return;
}

void DeleteAct(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:���˻��ѱ�����"));
	}
	else {
		curAct->status = UNVALID;
		GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" �����ɹ�!"));
	}
	return;
}

void Recharge(string uid, double val, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" ��ֵ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:�˻���Ч!"));
		return;
	}
	
	if (!checkCardValid(curAct->cardList)) {	//û�п����Գ�ֵ
		GenerateLog(cztime + string(" ��ֵ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:û����Ч��!"));
		return;
	}
	/*stringstream ss;
	ss << std::setprecision(5) << curAct->cardList.tail->pre->balance;
	string t;
	t << ss.str();
	ss.clear();*/
	if (curAct->cardList.tail->pre->balance + val > MAX_BALANCE) {
		//��ֵʧ�ܵ��߼�
		GenerateLog(cztime + string(" ��ֵ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:��ֵ���ﵽ����!��ֵǰ���:" + to_string(curAct->cardList.tail->pre->balance)));
		return;
	}
	curAct->cardList.tail->pre->balance += val;
	//�����ֵ�ɹ�����־����
	GenerateLog(cztime + string(" ��ֵ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" �����ɹ�!��ֵǰ���:" + to_string(curAct->cardList.tail->pre->balance - val) + string(" ��ֵ����:") + to_string(curAct->cardList.tail->pre->balance)));
}

void ReportLoss(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];

	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" ��ʧ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:�˻���Ч!"));
		return;
	}

	if (!checkCardValid(curAct->cardList)) {	
		GenerateLog(cztime + string(" ��ʧ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:û����Ч��!"));
		return;
	}
	curAct->cardList.tail->pre->valid = false;
	curAct->cardList.size--;
	GenerateLog(cztime + string("��ʧ==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" �ɹ���ʧ����:") + to_string(curAct->cardList.tail->pre->headNum) + curAct->cardList.tail->pre->serialNum + to_string(curAct->cardList.tail->pre->checkCode) + string(" �����ɹ�!"));
	return;
}

void EraseLoss(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];

	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" ���==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:�˻���Ч!"));
		return;
	}

	if (curAct->cardList.tail->pre == curAct->cardList.head) {
		GenerateLog(cztime + string(" ���==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:û�д���ҵĿ�!"));
		return;
	}
	curAct->cardList.tail->pre->valid = true;
	curAct->cardList.size++;
	GenerateLog(cztime + string(" ���==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" �ɹ���ҿ���:") + to_string(curAct->cardList.tail->pre->headNum) + curAct->cardList.tail->pre->serialNum + to_string(curAct->cardList.tail->pre->checkCode) + string(" �����ɹ�!"));
	return;
}

void MakeupCard(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����ʧ��:��ѧ�Ŷ�Ӧ�˺Ų�����!"));
		return;
	}
	string cardserial = generateCardSerial();
	CardID* newcard = new CardID;
	
	CardID2ActMap[stoi(cardserial)] = uid;
	newcard->headNum = 3;
	newcard->serialNum = cardserial;
	newcard->valid = true;
	newcard->balance = 0;
	if (AllAccount[major][order].cardList.tail->pre != AllAccount[major][order].cardList.head) {
		newcard->balance = AllAccount[major][order].cardList.tail->pre->balance;
	}
	int sum = 3;
	for (char it : cardserial) {
		sum += it - '0';
	}

	sum %= 10;
	newcard->checkCode = 9 - sum;
	//�ϳ����µĿ�������Ч��
	AllAccount[major][order].cardList.tail->pre->valid = false;

	AllAccount[major][order].cardList.size++;
	//�ƶ�������β��
	newcard->pre = AllAccount[major][order].cardList.tail->pre;
	AllAccount[major][order].cardList.tail->pre->next = newcard;
	newcard->next = AllAccount[major][order].cardList.tail;
	AllAccount[major][order].cardList.tail->pre = newcard;

	GenerateLog(cztime + string(" ����==>") + string("����:") + curAct->name + string(" ѧ��:") + curAct->stuID + string(" ����:") + to_string(newcard->headNum) + newcard->serialNum + to_string(newcard->checkCode) + string(" �����ɹ�!"));
	return;
}

void readCZFile() {//��������ֵ
	ifstream czfile;
	czfile.open("E:\\classdesign\\cz002.txt", ios::in);
	assert(czfile.is_open());
	string czmeg;
	getline(czfile, czmeg);
	while (getline(czfile, czmeg)) {
		string cztime = czmeg.substr(0, 16);
		string cztype = czmeg.substr(17, 4);
		string stuid = czmeg.substr(22, 10);
		//cout << cztime << cztype << stuid;
		int major = stoi(stuid.substr(4, 2));
		int order = stoi(stuid.substr(6, 3));
		if (cztype == "��ʧ") {
			ReportLoss(stuid, cztime);
		}
		else if (cztype == "���") {
			EraseLoss(stuid, cztime);
		}
		else if (cztype == "����") {
			DeleteAct(stuid, cztime);
		}
		else if (cztype == "��ֵ") {
			break;
			//int val = stoi(czmeg.substr(33));
			//cout << val;
			//Recharge(stuid, double(val), cztime);
		}
		//cout << endl;
		//showActMeg(stuid);
	}
	czfile.close();
	return;
}

regex exchangePattern(string user_pattern, bool nameflag) {
	string pattern;
	for (auto it : user_pattern) {
		if (it == '?') {
			if (nameflag) {
				pattern += "[\\s\\S]{2}";
			}
			else pattern += "[\\s\\S]{1}";
		}
		else if (it == '*') {
			pattern += "[\\s\\S]*";
		}
		else pattern += it;
	}
	cout << "user_pattern:"<< user_pattern << endl << "pattern:" << pattern << endl;
	return regex(pattern);
}

vector<CardAct> BlurSearch(string user_pattern, bool nameflag) {
	vector<CardAct> ret;
	regex pattern = exchangePattern(user_pattern, nameflag);
	for (int i = 0; i < STU_MAJOR_MAX_NUM; ++i) {
		for (int j = 0; j < STU_MAX_NUM_IN_MAJOR; ++j) {
			if (AllAccount[i][j].status == OK && nameflag) {
				if (regex_match(AllAccount[i][j].name, pattern)) ret.push_back(AllAccount[i][j]);
			}
			else if (AllAccount[i][j].status == OK && !nameflag) {
				if (regex_match(AllAccount[i][j].stuID, pattern)) ret.push_back(AllAccount[i][j]);
			}
		}
	}
	for (auto &it : ret) {
		showActMeg(it.stuID);
	}
	return ret;
}