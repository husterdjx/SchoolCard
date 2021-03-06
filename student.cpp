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

CardAct AllAccount[STU_MAJOR_MAX_NUM][STU_MAX_NUM_IN_MAJOR];	//所有账户的内存位置

string CardID2ActMap[CARD_MAX_NUM];		//流水号索引

int cardSeq = 12346;
/*检查校园卡账户是否有效*/
bool checkActValid(string uid) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		return false;
	}
	return true;
}
/*检查校园卡历史卡中最新的一张是否有效*/
bool checkCardValid(CardList cardlist) {
	if (cardlist.size == 0 || !cardlist.tail->pre->valid) {
		return false;
	}
	return true;
}
/*展示包括历史卡号在内的校园卡账户所有信息*/
void showActMeg_ALL(string uid) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	cout << "Name:" << curAct->name << ' ' << "Stuid:" << curAct->stuID << " " << "Status" << curAct->status << endl;
	cout << "CardList:" << endl;
	CardID* curCard = AllAccount[major][order].cardList.head->next;
	while (curCard != AllAccount[major][order].cardList.tail) {
		cout << "cardid:" << curCard->headNum << curCard->serialNum << curCard->checkCode << " 最终余额" << curCard->balance << endl;
		curCard = curCard->next;
		if (curCard != AllAccount[major][order].cardList.tail) cout << "↓" << endl;
	}
	return;
}
/*展示只包含最新卡号的校园卡账户所有信息*/
void showActMeg(string uid) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	cout << "Name:" << curAct->name << ' ' << "Stuid:" << curAct->stuID << " " << "Status" << curAct->status << endl;
	cout << "CardList:" << endl;
	if (checkCardValid(AllAccount[major][order].cardList)) {
		CardID* curCard = AllAccount[major][order].cardList.tail->pre;
		cout << "cardid:" << curCard->headNum << curCard->serialNum << curCard->checkCode << " 最终余额" << curCard->balance << endl;
		//curCard = curCard->next;
		//if (curCard != AllAccount[major][order].cardList.tail) cout << "↓" << endl;
	}
	else {
		cout << "当前账号全部卡无效" << endl;
	}
	return;
}
/*开户的主要逻辑函数*/
void initAccount(string uid, string name, int major, int order, string cztime) {
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->validTime == VALID_TIME) {
		// 开户时学号的唯一性检查逻辑
		GenerateLog(string("开户==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:该学号已存在账户!"));
		return;
	}
	
	curAct->cardList.head = (CardID*)malloc(sizeof(CardID));
	curAct->cardList.tail = (CardID*)malloc(sizeof(CardID));
	curAct->name = name;
	curAct->status = OK;	//销户和有效期
	curAct->stuID = uid;
	curAct->validTime = VALID_TIME;
	curAct->cardList.head->next = curAct->cardList.tail;
	curAct->cardList.tail->pre = curAct->cardList.head;
	curAct->cardList.size = 0;
	GenerateLog(cztime + string(" 开户==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作成功!"));
	return;
}
/*产生全局的流水号，用于发卡时分配流水号*/
string generateCardSerial() {
	string ret = to_string(cardSeq);
	cardSeq++;
	return ret;
}
/*发卡逻辑*/
void SendCardID(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" 发卡==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:该学号对应账号不可用!"));
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
	//废除最新的卡，即有效卡
	AllAccount[major][order].cardList.tail->pre->valid = false;

	AllAccount[major][order].cardList.size++;
	//移动至链表尾部
	newcard->pre = AllAccount[major][order].cardList.tail->pre;
	AllAccount[major][order].cardList.tail->pre->next = newcard;
	newcard->next = AllAccount[major][order].cardList.tail;
	AllAccount[major][order].cardList.tail->pre = newcard;

	GenerateLog(cztime + string(" 发卡==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 卡号:") + to_string(newcard->headNum) + newcard->serialNum + to_string(newcard->checkCode) + string(" 操作成功!"));
	return;
}
/*开户逻辑*/
void CreateAct(string uid, string name, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	initAccount(uid, name, major, order, cztime);
	SendCardID(uid, cztime);
	//日志操作在initAct中
	return;
}
/*读取开户文件并存入内存*/
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
/*销户逻辑*/
void DeleteAct(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" 销户==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:该账户已被销号"));
	}
	else {
		curAct->status = UNVALID;
		GenerateLog(cztime + string(" 销户==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作成功!"));
	}
	return;
}
/*充值逻辑*/
void Recharge(string uid, double val, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" 充值==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:账户无效!"));
		return;
	}
	
	if (!checkCardValid(curAct->cardList)) {	//没有卡可以充值
		GenerateLog(cztime + string(" 充值==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:没有有效卡!"));
		return;
	}

	if (curAct->cardList.tail->pre->balance + val > MAX_BALANCE) {
		//充值失败的逻辑
		GenerateLog(cztime + string(" 充值==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:充值金额达到上限!充值前金额:" + to_string(curAct->cardList.tail->pre->balance)));
		return;
	}
	curAct->cardList.tail->pre->balance += val;
	//加入充值成功的日志操作
	GenerateLog(cztime + string(" 充值==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作成功!充值前金额:" + to_string(curAct->cardList.tail->pre->balance - val) + string(" 充值后金额:") + to_string(curAct->cardList.tail->pre->balance)));
}
/*挂失逻辑*/
void ReportLoss(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));		//解析学号
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];

	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" 挂失==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:账户无效!"));
		return;
	}

	if (!checkCardValid(curAct->cardList)) {	
		GenerateLog(cztime + string(" 挂失==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:没有有效卡!"));
		return;
	}
	curAct->cardList.tail->pre->valid = false;
	curAct->cardList.size--;
	GenerateLog(cztime + string("挂失==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 成功挂失卡号:") + to_string(curAct->cardList.tail->pre->headNum) + curAct->cardList.tail->pre->serialNum + to_string(curAct->cardList.tail->pre->checkCode) + string(" 操作成功!"));
	return;
}
/*解挂逻辑*/
void EraseLoss(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];

	if (!checkActValid(uid)) {
		GenerateLog(cztime + string(" 解挂==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:账户无效!"));
		return;
	}

	if (curAct->cardList.tail->pre == curAct->cardList.head) {
		GenerateLog(cztime + string(" 解挂==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:没有待解挂的卡!"));
		return;
	}
	curAct->cardList.tail->pre->valid = true;
	curAct->cardList.size++;
	GenerateLog(cztime + string(" 解挂==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 成功解挂卡号:") + to_string(curAct->cardList.tail->pre->headNum) + curAct->cardList.tail->pre->serialNum + to_string(curAct->cardList.tail->pre->checkCode) + string(" 操作成功!"));
	return;
}
/*补卡逻辑*/
void MakeupCard(string uid, string cztime) {
	int major = stoi(uid.substr(4, 2));
	int order = stoi(uid.substr(6, 3));
	CardAct* curAct = &AllAccount[major][order];
	if (curAct->status == UNVALID) {
		GenerateLog(cztime + string(" 补卡==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 操作失败:该学号对应账号不可用!"));
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
	//废除最新的卡，即有效卡
	AllAccount[major][order].cardList.tail->pre->valid = false;

	AllAccount[major][order].cardList.size++;
	//移动至链表尾部
	newcard->pre = AllAccount[major][order].cardList.tail->pre;
	AllAccount[major][order].cardList.tail->pre->next = newcard;
	newcard->next = AllAccount[major][order].cardList.tail;
	AllAccount[major][order].cardList.tail->pre = newcard;

	GenerateLog(cztime + string(" 补卡==>") + string("姓名:") + curAct->name + string(" 学号:") + curAct->stuID + string(" 卡号:") + to_string(newcard->headNum) + newcard->serialNum + to_string(newcard->checkCode) + string(" 操作成功!"));
	return;
}
/*打开操作文件读入操作（不包含充值）*/
void readCZFile() {//不包含充值
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
		if (cztype == "挂失") {
			ReportLoss(stuid, cztime);
		}
		else if (cztype == "解挂") {
			EraseLoss(stuid, cztime);
		}
		else if (cztype == "销户") {
			DeleteAct(stuid, cztime);
		}
		else if (cztype == "充值") {
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
/*将用户传入的表达式转化为标准的正则表达式*/
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
/*模糊查找逻辑，返回所有匹配的账号*/
vector<CardAct> BlurSearch(string user_pattern, bool nameflag) {
	vector<CardAct> ret;
	regex pattern = exchangePattern(user_pattern, nameflag);
	for (int i = 0; i < STU_MAJOR_MAX_NUM; ++i) {		//遍历所有的可能账户位置，并判断是否匹配
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
		showActMeg_ALL(it.stuID);
	}
	return ret;
}