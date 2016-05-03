#include"stdafx.h"
#include"GrayCompress.h"

CGrayZip::CGrayZip(string EncodeFile, string DecodeFile)
{
	m_EncodeFile = EncodeFile;
	m_DecodeFile = DecodeFile;
}
//ѹ��
void CGrayZip::Compress()
{
	//��ȡԭλͼ�ļ�
	FileIn.open(m_EncodeFile, ios::in | ios::binary);

	FileIn.read((char*)&bmFH, sizeof(bmFH));
	FileIn.read((char*)&bmIH, sizeof(bmIH));
	//int size = bmFH.bfOffBits - sizeof(bmFH)-sizeof(bmIH);
	FileIn.read((char*)&rgbp,sizeof(RGBQUAD)* 256);
	
	int width = bmIH.biWidth;
	width = (width * bmIH.biBitCount / 8 + 3) / 4 * 4;		//ͼ��ÿһ�е��ֽ���������4��������
	int height = bmIH.biHeight;
	unsigned int len = width*height;
	int i, j, k;
	bitMapInfo2D = new unsigned char*[height];
	for (k = 0; k < height; k++)
		bitMapInfo2D[k] = new unsigned char[width];
	bitMapInfo1D = new unsigned char[len + 1];
	for (i = 0; i < height; i++)
	for (j = 0; j < width; j++)
		FileIn.read((char*)&bitMapInfo2D[i][j], sizeof(unsigned char));
	FileIn.close();
	//�����ȡ��������
	j = 0, k = 1;
	for (i = 0; i < height; i++)
	{
		if (j == 0)
		{
			for (; j < width; j++)
				bitMapInfo1D[k++] = bitMapInfo2D[i][j];
			j--;
		}
		else
		{
			for (; j >= 0; j--)
				bitMapInfo1D[k++] = bitMapInfo2D[i][j];
			j++;
		}
	}

	int *minBits;		//ÿ�δ洢����Сλ��
	int *tempStore;		//��ʱ�洢�����������Ķγ���lS���������
	int header = 11;
	int maxLS = 256;		//���εĳ���Ϊ256������ʱ�ʹ�256�����
	len = width*height;
	lS = new unsigned int[len + 1];		//�γ���
	bS = new unsigned int[len + 1];		//��λ��
	minBits = new int[len + 1];
	tempStore = new int[len + 1];		//�洢���ŷֶεĶγ���Ϊ����洢
	minBits[0] = 0;         //minBits[i]����ǰi��Ԫ��ѹ����洢���������λ��
	//��̬�滮Ѱ�����ŷֶβ���
	for (i = 1; i <= len; i++)
	{
		//int lsum = 1;
		bS[i] = validBits(int(bitMapInfo1D[i]));           //��¼��i��Ԫ�ص���Ч����b[i]
		int bmax = bS[i];
		minBits[i] = minBits[i - 1] + bmax;                 //��ǰѹ����洢��������λ��(������11λ�Ķα��⣩
		lS[i] = 1;	//��ǰ���ض�Ϊ1
		int k;		//��i����256ʱ��k���ֻ��ȡ256
		if (i > maxLS)
			k = maxLS;
		else
			k = i;
		for (int j = 2; j <= k; j++)
		{
			//ö�ٵ�ǰ��iλ����֮ǰ���ܼ�j��Ԫ�����һ�ε���Сλ��
			if (bmax<bS[i - j + 1])           //��ǰλ��С��֮ǰ��λ��
				bmax = bS[i - j + 1];
			if (minBits[i]>minBits[i - j] + j*bmax)
			{
				//��ǰ�洢�����λ������ ��ǰԪ����֮ǰ���һ�κ������λ��
				minBits[i] = minBits[i - j] + j*bmax;
				lS[i] = j;		//j������
			}
		}
		minBits[i] = minBits[i] + header;    //��ǰ�����Ž���϶α����11λ
	}
	//����������ŷֶεĶγ�
	int Segment = 1;
	int n = len;
	while (n > 0)
	{
		tempStore[Segment++] = lS[n];
		n -= lS[n];
	}
	//��������lS
	for (int k = 1; k < Segment; k++)
		lS[k] = tempStore[Segment - k];

	//��ѹ���������д���ļ�
	FileOut.open(m_DecodeFile, ios::out | ios::binary);
	//ѹ���ļ���Ϣͷ
	FileOut.write((char*)&bmFH, sizeof(bmFH));
	FileOut.write((char*)&bmIH, sizeof(bmIH));
	FileOut.write((char*)&rgbp, sizeof(RGBQUAD)*256);
	//ѹ��bmp����
	unsigned int writeBuffer = 0, bitCount = 0;
	i = 1;
	unsigned int temp = 0;
	int bits, L;
	for (int k = 1; k < Segment; k++)
	{
		//д��8λ���طֶ���
		lS[k] -= 1;		//�Զ�����д���ļ�ʱ��lS[k]ֻ�ܱ�ʾ0-7,bS[k]ֻ�ܱ�ʾ0-2;
		bS[k] -= 1;
		bits = 8;
		if (bitCount + bits < 32)
		{
			writeBuffer <<= bits;
			writeBuffer |= lS[k];
			bitCount += bits;
		}
		else if (bitCount + bits == 32)
		{
			writeBuffer <<= bits;
			writeBuffer |= lS[k];
			FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
			writeBuffer = 0;
			bitCount = 0;
		}
		else
		{
			temp = lS[k];
			writeBuffer <<= (32 - bitCount);
			temp >>= bits - (32 - bitCount);
			writeBuffer |= temp;
			FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
			writeBuffer = (lS[k] << (64 - bits - bitCount)) >> (64 - bits - bitCount);
			bitCount = bits - (32 - bitCount);
		}
		//д��3λ���طֶ�λ��
		bits = 3;
		if (bitCount + bits < 32)
		{
			writeBuffer <<= bits;
			writeBuffer |= bS[k];
			bitCount += bits;
		}
		else if (bitCount + bits == 32)
		{
			writeBuffer <<= bits;
			writeBuffer |= bS[k];
			FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
			writeBuffer = 0;
			bitCount = 0;
		}
		else
		{
			temp = bS[k];
			writeBuffer <<= (32 - bitCount);
			temp >>= bits - (32 - bitCount);
			writeBuffer |= temp;
			FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
			writeBuffer = (bS[k] << (64 - bits - bitCount)) >> (64 - bits - bitCount);
			bitCount = bits - (32 - bitCount);
		}
		//д��ѹ�������������
		L = i + lS[k] + 1;
		for (; i < L; i++)
		{
			bits = bS[k] + 1;
			if (bitCount + bits < 32)
			{
				writeBuffer <<= bits;
				writeBuffer |= int(bitMapInfo1D[i]);
				bitCount += bits;
			}
			else if (bitCount + bits == 32)
			{
				writeBuffer <<= bits;
				writeBuffer |= int(bitMapInfo1D[i]);
				FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
				writeBuffer = 0;
				bitCount = 0;
			}
			else
			{
				temp = int(bitMapInfo1D[i]);
				writeBuffer <<= (32 - bitCount);
				temp >>= bits - (32 - bitCount);
				writeBuffer |= temp;
				FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
				writeBuffer = (int(bitMapInfo1D[i]) << (64 - bits - bitCount)) >> (64 - bits - bitCount);
				bitCount = bits - (32 - bitCount);
			}
		}
	}
	if (bitCount > 0)		//����32λ�Ĳ�0
	{
		writeBuffer <<= 32 - bitCount;
		FileOut.write((char*)&writeBuffer, sizeof(unsigned int));
	}
	FileOut.close();
	delete[]tempStore;
	delete[]minBits;
	delete[]bS;
	delete[]lS;
	delete[]bitMapInfo1D;
	for (int k = 0; k < height; k++)
		delete[]bitMapInfo2D[k];
	delete[]bitMapInfo2D;
}

int CGrayZip::validBits(int Integer)
{
	int bits = 1;
	while (Integer>1)
	{
		Integer /= 2;
		bits++;
	}
	return bits;
}

//��ѹ
void CGrayZip::UnCompress()
{
	//m_DecodeFile = DecodeFile;
	//��ѹ���ļ��ж�ȡ����ѹ������
	FileIn.open(m_EncodeFile, ios::binary);
	//��ѹ���ļ��ж�ȡbmp�ļ���Ϣͷ
	FileIn.read((char*)&bmFH, sizeof(bmFH));
	FileIn.read((char*)&bmIH, sizeof(bmIH));
	//int size = bmFH.bfOffBits - sizeof(bmFH)-sizeof(bmIH);
	FileIn.read((char*)&rgbp, sizeof(RGBQUAD)*256);
	FileOut.open(m_DecodeFile, ios::binary);
	//��λͼ��Ϣͷд���ļ�
	FileOut.write((char*)&bmFH, sizeof(bmFH));
	FileOut.write((char*)&bmIH, sizeof(bmIH));
	FileOut.write((char*)&rgbp, sizeof(RGBQUAD)*256);
	int width = bmIH.biWidth;
	width = (width * (bmIH.biBitCount) / 8 + 3) / 4 * 4;		//ͼ��ÿһ�е��ֽ���������4��������
	int height = bmIH.biHeight;
	cout << width << endl << height << endl;
	unsigned int len = width*height;
	cout << len << endl;
	unsigned int readBuffer = 0;
	int i = 1, bitCount = 0;
	//��ѹ���������д���ļ�
	unsigned int temp = 0;
	int bits, L;
	bitMapInfo2D = new unsigned char*[height];
	int k;
	for (k = 0; k < height; k++)
		bitMapInfo2D[k] = new unsigned char[width];
	lS = new unsigned int[len + 1];
	bS = new unsigned int[len + 1];
	bitMapInfo1D = new unsigned char[len + 1];

	k = 1;
	while (!FileIn.eof())
	{
		//�ȶ�ȡ�γ����ٶ�ȡ�γ��е�λ��������ȡλͼ��Ϣ
		if (bitCount == 0)
		{
			FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			bitCount = 32;
		}
		if (bitCount > 0 && readBuffer == 0)
			break;		//��ȡ����ĩβ���㴦���˳�ѭ��
		//��ȡ8λ�γ�
		bits = 8;
		if (bitCount - bits > 0)
		{
			temp = readBuffer;
			lS[k] = temp >> (bitCount - bits);
			bitCount -= bits;
			readBuffer = (readBuffer << (32 - bitCount)) >> (32 - bitCount);
		}
		else if (bitCount - bits == 0)
		{
			lS[k] = readBuffer;
			FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			bitCount = 32;
		}
		else
		{
			lS[k] = readBuffer;
			lS[k] <<= (bits - bitCount);
			FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			temp = readBuffer;
			temp = temp >> (32 - bits + bitCount);
			lS[k] |= temp;
			readBuffer = (readBuffer << (bits - bitCount)) >> (bits - bitCount);
			bitCount = 32 - bits + bitCount;
		}
		//��ȡ�öε�λ������������ת��Ϊunsigned int bS
		bits = 3;
		if (bitCount - bits > 0)
		{
			temp = readBuffer;
			bS[k] = temp >> (bitCount - bits);
			bitCount -= bits;
			readBuffer = (readBuffer << (32 - bitCount)) >> (32 - bitCount);
		}
		else if (bitCount - bits == 0)
		{
			bS[k] = readBuffer;
			FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			bitCount = 32;
		}
		else
		{
			bS[k] = readBuffer;
			bS[k] <<= (bits - bitCount);
			FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			temp = readBuffer;
			temp = temp >> (32 - bits + bitCount);
			bS[k] |= temp;
			readBuffer = (readBuffer << (bits - bitCount)) >> (bits - bitCount);
			bitCount = 32 - bits + bitCount;
		}
		//��ȡbmpѹ������
		bits = bS[k] + 1;		//���ݵ�λ��
		L = i + lS[k] + 1;
		for (; i < L; i++)
		{
			if (bitCount>bits)
			{
				temp = readBuffer;
				bitMapInfo1D[i] = temp >> (bitCount - bits);
				bitCount -= bits;
				readBuffer = (readBuffer << (32 - bitCount)) >> (32 - bitCount);
			}
			else if (bitCount == bits)
			{
				bitMapInfo1D[i] = readBuffer;
				bitCount = 32;
				FileIn.read((char*)&readBuffer, sizeof(unsigned int));
			}
			else
			{
				bitMapInfo1D[i] = readBuffer;
				bitMapInfo1D[i] <<= (bits - bitCount);
				FileIn.read((char*)&readBuffer, sizeof(unsigned int));
				temp = readBuffer;
				temp = temp >> (32 - bits + bitCount);
				bitMapInfo1D[i] |= temp;
				readBuffer = (readBuffer << (bits - bitCount)) >> (bits - bitCount);
				bitCount = 32 - bits + bitCount;
			}
		}
		k++;
	}
	FileIn.close();
	int j = 0;
	k = 1;
	for (i = 0; i < height; i++)
	{
		if (j == 0)
		{
			for (; j < width; j++)
				bitMapInfo2D[i][j] = bitMapInfo1D[k++];
			j--;
		}
		else
		{
			for (; j >= 0; j--)
				bitMapInfo2D[i][j] = bitMapInfo1D[k++];
			j++;
		}
	}
	for (i = 0; i < height; i++)
	for (j = 0; j < width; j++)
		FileOut.write((char*)&bitMapInfo2D[i][j], sizeof(unsigned char));
	FileOut.close();
	//�ͷŶ�̬����Ŀռ�
	delete[]bitMapInfo1D;
	delete[]bS;
	delete[]lS;
	for (int k = 0; k < height; k++)
		delete[]bitMapInfo2D[k];
	delete[]bitMapInfo2D;
}

CGrayZip::~CGrayZip()
{

}