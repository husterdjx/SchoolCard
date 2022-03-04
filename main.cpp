#include<iostream>
#include<string>
#include<fstream>
#include "def.h"
using namespace std;
fstream czlogWriter, xflogWriter, sortxflogWriter, sortxflogWriter_quick_sort, windowlogWriter;
fstream xflogReader;
extern int xflogLine;
/*�����������߼�*/
void batchKH() {
	readKHFile();
	return;
}
/*��������������������ֵ�Ϳ�����*/
void batchCZ() {
	readCZFile();
	return;
}
/*��ʾ�û��Ƿ�ȷ��ִ�в���*/
bool ack() {
	cout << "�Ƿ����?" << endl;
	int flag;
	cin >> flag;
	if (flag) {
		return true;
	}
	else {
		cout << "��������" << endl;
		return false;
	}
}
/*��Ƭ����ģ��Ľӿ�*/
void CZ() {
	string meg;
	cout << "����ѧ�š���������Ϣ,��ʽΪ ��������(,ѧ��,����еĻ�)(,����,����еĻ�)(,������Ϣ,���ֵ���)" << endl;
	cout << "���������б�:" << endl;
	cout << "1 ���� 2 ���� 3 ���� 4 ��ʧ 5 ��� 6 ���� 7 ��ֵ 8 ���벢������������(��������������) 0 �˳�" << endl;
	while (cin >> meg) {
		if (meg[0] == '0') break;
		if (meg[0] == '8') {
			batchKH();
			batchCZ();
			cout << "�����������,��������������" << endl;
			system("pause");
			break;
		}
		string stuid = meg.substr(2, 10);
		char type = meg[0];
		if (type == '1') {
			string name = meg.substr(13, meg.size() - 13);
			cout << "��ֵѧ��" << stuid << " ����:" << name << endl;
			CreateAct(stuid, name, "2021090109220510");
		}
		else if (type == '2') {
			showActMeg(stuid);

			if (ack()) DeleteAct(stuid, "");
		}
		else if (type == '3') {
			showActMeg(stuid);
			if (ack()) SendCardID(stuid, "");
		}
		else if (type == '4') {
			showActMeg(stuid);
			if (ack()) ReportLoss(stuid, "");
		}
		else if (type == '5') {
			showActMeg(stuid);
			if (ack()) EraseLoss(stuid, "");
		}
		else if (type == '6') {
			showActMeg(stuid);
			if (ack()) MakeupCard(stuid, "");
		}
		else if (type == '7') {
			double val = stod(meg.substr(13));
			showActMeg(stuid);
			cout << "�Ƿ��ֵ" << val << "Ԫ?" << endl;
			int flag;
			cin >> flag;
			if (flag) {
				Recharge(stuid, double(val), "");
			}
			else {
				cout << "������ֵ" << endl;
			}
		}
		else {
			cout << "Valid type!" << endl;
		}
		cout << "��ִ����ϣ���������һ�β���" << endl;
	}
	cout << "������Ƭ����" << endl;
	system("pause");
	system("cls");
	return;
}
/*�����е�Writer������־д��*/
void openWriter() {
	windowlogWriter.open("E:\\classdesign\\windowslog.txt", ios::out);
	czlogWriter.open("E:\\classdesign\\czlog.txt", ios::out);
	xflogWriter.open("E:\\classdesign\\xflog.txt", ios::out);
	sortxflogWriter.open("E:\\classdesign\\xflog(sorted by multimerge).txt", ios::out);
	sortxflogWriter_quick_sort.open("E:\\classdesign\\xflog(sorted by quicksort).txt", ios::out);
}
/*�ر����е�Writer��֤��ȫ*/
void closeWriter() {
	czlogWriter.close();
	xflogWriter.close();
	sortxflogWriter.close();
	sortxflogWriter_quick_sort.close();
	windowlogWriter.close();
}
/*������ʧ/��������ֵ�����ѵ��߼�*/
void batchCard() {

	batchKH();
	batchCZ();
	initWindows();
	read2XF();
	batchXF();
	MultipleMerge();
	QuickSort();
}

int main() {
	
	Welcome();		//��ʼ������
	if (menu() == 0) {		//�û��ж��Ƿ����ϵͳ
		system("cls");
		cout << "�˳�ϵͳ!" << endl;
		return 0;
	}
	else {
		system("cls");
		cout << "����ϵͳ!" << endl;
		system("pause");
	}
	system("cls");
	openWriter();
	while (1) {
		cout << "************************��ӭ����У԰������ϵͳ!************************" << endl;
		cout << "1.���뿨Ƭ����ģ��" << endl;
		cout << "2.����ʳ�ù���ģ��" << endl;
		cout << "3.�����ѯͳ��ģ��" << endl;
		cout << "4.�˳�ϵͳ" << endl;
		int choice;
		cin >> choice;
		if (choice == 1) {
			system("cls");
			CZ();
		}
		else if (choice == 2) {
			system("cls");
			ShowCartoon();
		}
		else if (choice == 3) {
			system("cls");
			search();
		}
		else if (choice == 4) {
			cout << "�˳�ϵͳ!" << endl;
			return 0;
		}
		else {
			cout << "�Ƿ����룬������" << endl;
		}
		system("cls");
	}
	closeWriter();
	
	return 0;
}