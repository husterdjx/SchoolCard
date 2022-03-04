#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<cassert>
using namespace std;
#define OK "OK"
#define UNVALID "UNVALID"
#define STU_MAJOR_MAX_NUM 100
#define STU_MAX_NUM_IN_MAJOR 1000 
#define CARD_MAX_NUM 100000
#define VALID_TIME "20240715"
#define MAX_BALANCE 999.99
#define MAX_LOG_NUM 1800000
extern fstream czlogWriter, xflogWriter, sortxflogWriter, sortxflogWriter_quick_sort, windowlogWriter;
extern FILE *xflogWriter1;

struct CardID {
	int			headNum;    //�������ţ�Ĭ��Ϊ3
	string		serialNum;  //��ˮ��
	int			checkCode;	//У����
	bool		valid;		//��ʾ�ÿ��Ƿ���Ч
	double		balance;	//���
	CardID*		pre;		//˫��������ͬ
	CardID*		next;
};
struct CardList {	//˫������manager
	CardID* head;
	CardID* tail;
	int		size;
};
struct CardAct {			//У԰���˻�����
	string		stuID;		//У԰������
	CardList    cardList;	//��ʾ��ʷ����˫������
	string      name;		//У԰���˻���������
	string      validTime;	//����ʱ��
	string      status;		//�˻�״̬
};

struct XFMeg {			//���Ѽ�¼
	string cardid;		//����
	string xfdate;		//��������
	string xftime;		//����ʱ��
	double val;			//���ѽ��
	int window;			//���Ѵ���
	bool valid;			//�����Ƿ���Ч
	double balance;		//���Ѻ�ĸÿ���
	bool secretneed;	//��ʾ�Ƿ���Ҫ��������
};
struct WindowManager {		//����manager
	int num;				//���ں�
	int pointer;			//����ʱ��Ҫ��λ�ö�λΪ
	vector<XFMeg> xflist;	//��¼���ڶ�Ӧ����������
};
//student.cpp
void readKHFile(void);
void readCZFile(void);
vector<CardAct> BlurSearch(string, bool);
void CreateAct(string, string, string);
void DeleteAct(string, string);
void EraseLoss(string, string);
void MakeupCard(string, string);
void Recharge(string, double, string);
void SendCardID(string, string);
void ReportLoss(string, string);
void showActMeg(string);
bool checkActValid(string);
//log.cpp
void GenerateLog(string);
void xflog(string);
void sortxflog(string);
void sortxflog_quick_sort(string);
void windowslog(string);
//cartoon.cpp
void ShowCartoon(void);
void read2XF(void);
void initWindows(void);
void read2XF(void);
void batchXF(void);
void setConfig(string, int);
void ShowCartoon(void);
//analyse.cpp
void search(void);
void MultipleMerge(void);
void QuickSort(void);
size_t MyHash(string);
//menu.cpp
int menu(void);
void Welcome(void);

