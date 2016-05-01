#include<windows.h>
#include<iostream>
#include<string>
#include<fstream>
using namespace std;

class CGrayZip
{
private:
	string m_EncodeFile;
	string m_DecodeFile;
	ifstream FileIn;		//input file stream
	ofstream FileOut;		//output file stream
	unsigned char **bitMapInfo2D;		//two dimension bitmap infomation array
	unsigned char *bitMapInfo1D;		//one dimension bitmap information array
	tagBITMAPFILEHEADER bmFH;		//bmp�ļ�ͷ
	tagBITMAPINFOHEADER bmIH;		//bmp��Ϣͷ
	tagRGBQUAD rgbp[256];		//��ɫ��
	int validBits(int Integer);
	unsigned int *lS, *bS;		//�γ�����λ
public:
	CGrayZip(string EncodeFile, string DecodeFile);
	~CGrayZip();
	void Compress();
	void UnCompress();
};
