
/************************************************
		JPGE图像编解码程序库
版本：					1.00
程序：					荣强
日期：					2004-03-09
************************************************/

#ifndef	INCLUDE_JPGLIB_H_2004_03_09
#define INCLUDE_JPGLIB_H_2004_03_09

//typedef (int (*DumpBufferData)(const void*, int)) DUMPBUFFUNCTION;
//1、设置JPG图像压缩时的品质
int SetJpgQuality(int newquality);
//功  能：	设置JPG图像压缩时的品质缺省是50
//入  口：	newquality 新的压缩品质(0-100)
//返回值：	返回被替换下来的品质

//(0-100)(缺省是50)
//2、压缩bmp图像数据
BYTE* CompressToJpg(BYTE *pBmpDat, int width, int height, int& outlen,int quality);
//功  能：	压缩位图到内存
//入  口：	hBit    bmp图像的句柄
//			width	bmp图像的宽度
//			height	bmp图像的高度
//			pBuffer jpg图像数据的缓冲区
//			buflen	jpg图像数据的缓冲区长度(Byte)
//返回值：	成功返回TRUE,失败返回FALSE
void SetDumpFunction(BYTE* (*function)(const void*, unsigned int&, BOOL));

#endif