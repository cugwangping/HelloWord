// Console_GrayPictureCompress.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"GrayCompress.h"
using namespace std;

int main()
{
	bool judge = false;
	/*cout << "请选择需要的操作：非0为压缩，0为解压：" << endl;
	cin >> judge;*/
	//string EncodeFile = "D:\\wenliright.bmp";
	//string DecodeFile = "D:\\DecodeFile.txt";
	string EncodeFile = "D:\\DecodeFile.txt";
	string DecodeFile = "D:\\jieya.bmp";
	/*if (judge)
	{
		cout << "请输入需要压缩的文件：" << endl;
		cin >> EncodeFile;
		cout << "请输入压缩后的文件：" << endl;
		cin >> DecodeFile;
	}
	else
	{
		cout << "请输入需要解压的文件：" << endl;
		cin >> EncodeFile;
		cout << "请输入解压后的文件：" << endl;
		cin >> DecodeFile;
	}*/
	CGrayZip Zip(EncodeFile, DecodeFile);
	if (judge)
		Zip.Compress();
	else
		Zip.UnCompress();
	return 0;
}

