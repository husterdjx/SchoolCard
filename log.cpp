#include<iostream>
#include<fstream>
#include<io.h>
#include"def.h"
using namespace std;
size_t LogHash[MAX_LOG_NUM];	//������־У����hash�ı��洦���������ڴ�
int xflogLine;	//����������־У������ڴ汣��
/*������־Write����*/
void GenerateLog(string meg) {
	//czlogWriter.open("E:\\����\\log.txt", ios::out | ios::app);

	czlogWriter << (char*)meg.c_str() << endl;
	//czlogWriter.flush();
	return;
}
/*������־Write����*/
void xflog(string meg) {
	xflogWriter << (char*)meg.c_str() << endl;
	LogHash[++xflogLine] = MyHash(meg);
	//xflogWriter.flush();
	//fputs((char*)meg.c_str(), xflogWriter1);
	return;
}
/*��·�鲢��������־��Write����*/
void sortxflog(string meg) {
	sortxflogWriter << (char*)meg.c_str() << endl;
	//xflogWriter.flush();
	//fputs((char*)meg.c_str(), xflogWriter1);
	return;
}
/*������������־Write����*/
void sortxflog_quick_sort(string meg) {
	sortxflogWriter_quick_sort << (char*)meg.c_str() << endl;
	//xflogWriter.flush();
	//fputs((char*)meg.c_str(), xflogWriter1);
	return;
}
/*Windows��¼���ջ��Ƴɹ���������Ŀ����־Write����*/
void windowslog(string meg) {
	windowlogWriter << (char*)meg.c_str() << endl;
	//xflogWriter.flush();
	//fputs((char*)meg.c_str(), xflogWriter1);
	return;
}