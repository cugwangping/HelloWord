// Console_GrayPictureCompress.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include"GrayCompress.h"
using namespace std;

int main()
{
	bool judge = false;
	/*cout << "��ѡ����Ҫ�Ĳ�������0Ϊѹ����0Ϊ��ѹ��" << endl;
	cin >> judge;*/
	//string EncodeFile = "D:\\wenliright.bmp";
	//string DecodeFile = "D:\\DecodeFile.txt";
	string EncodeFile = "D:\\DecodeFile.txt";
	string DecodeFile = "D:\\jieya.bmp";
	/*if (judge)
	{
		cout << "��������Ҫѹ�����ļ���" << endl;
		cin >> EncodeFile;
		cout << "������ѹ������ļ���" << endl;
		cin >> DecodeFile;
	}
	else
	{
		cout << "��������Ҫ��ѹ���ļ���" << endl;
		cin >> EncodeFile;
		cout << "�������ѹ����ļ���" << endl;
		cin >> DecodeFile;
	}*/
	CGrayZip Zip(EncodeFile, DecodeFile);
	if (judge)
		Zip.Compress();
	else
		Zip.UnCompress();
	return 0;
}

