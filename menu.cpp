#include<iostream>
#include<windows.h>
#include <graphics.h>
#include<conio.h>
using namespace std;
/*�ı�console���λ��*/
void gotoxy(int x, int y) {
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
/*���ÿ���̨����������ɫ�ͱ���ɫ*/
int color(int c) {
	//SetConsoleTextAttribute��API���ÿ���̨����������ɫ�ͱ���ɫ�ĺ���
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);        //����������ɫ
	return 0;
}

void output_board(int i, int j, int chang, int kuan) {
	//(i,j)��ʾ�߿�����Ͻ�
	//i����ڼ���,j����ڼ��� 
	int last_line = i + kuan - 1;
	int last_col = j + chang - 1;
	int first_j = j;
	int first_i = i;
	for (; i <= last_line; i++) {
		//�ȱ����� 
		j = first_j;
		for (; j <= last_col; j++) {
			//�ٱ����� 
			gotoxy(j, i);
			if (i == first_i || i == last_line) {
				printf("-");
			}
			else if (j == first_j || j == last_col) {
				printf("|");
			}
		}
	}

	gotoxy(0, 0);//����̨���λ�ù���
}
/*��ʾWelcome����*/
void Welcome() {
	initgraph(640, 480, EW_SHOWCONSOLE);

	IMAGE bg;
	loadimage(&bg, _T("./Hustseals.png"));
	putimage(150, 50, &bg);
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 20;
	settextstyle(&f);
	char wel_1[] = "WELCOME TO HERE!";
	char wel_2[] = "--by husterdjx";
	outtextxy(230, 350, wel_1);
	outtextxy(390, 370, wel_2);
	Sleep(2000);
	cleardevice();
	closegraph();
}
/*��ʼ���˵�*/
int menu() {
	//��ʾ�˵�
	system("title У԰������ϵͳ");
	int i, j;
	color(2);

	gotoxy(43, 18);
	//����ɫ���������
	color(11);
	printf("У԰������ϵͳ");

	color(14);
	//��ɫ������߿�
	output_board(20, 27, 47, 6);

	color(10);
	//��ɫ,���ѡ�� 
	gotoxy(35, 22);
	printf("1.����У԰������ϵͳ");
	gotoxy(55, 22);
	printf("0.�˳�");

	gotoxy(29, 27);
	//��ɫ�����ѡ�� 
	color(3);
	printf("��ѡ��[1 0]:[ ]");

	//��������
	color(7);
	gotoxy(90, 27);
	printf("by  husterdjx");

	HANDLE hout;
	COORD coord;
	coord.X = 42;
	coord.Y = 27;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hout, coord);

	int choice;
	cin >> choice;
	color(7);       // �ָ�����̨����ɫ
	gotoxy(0, 0);    // �ָ�����̨�����λ��
	system("color f0");
	return choice;
}
