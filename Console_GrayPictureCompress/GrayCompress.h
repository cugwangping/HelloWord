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
	tagBITMAPFILEHEADER bmFH;		//bmp文件头
	tagBITMAPINFOHEADER bmIH;		//bmp信息头
	tagRGBQUAD rgbp[256];		//调色板
	int validBits(int Integer);
	unsigned int *lS, *bS;		//段长，段位
public:
	CGrayZip(string EncodeFile, string DecodeFile);
	~CGrayZip();
	void Compress();
	void UnCompress();
};
