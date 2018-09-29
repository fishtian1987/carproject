#include "StdAfx.h"
#include "BmpToJpg.h"
#include <Objidl.h>
#include "ScreenFile.h"
#include "WinApi.h"

extern time_t	tPrevSnap;

/*
位图头结构
*/
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER1
{
    unsigned char bfType[2];//文件格式
    unsigned long bfSize;//文件大小
    unsigned short bfReserved1;//保留
    unsigned short bfReserved2;
    unsigned long bfOffBits; //DIB数据在文件中的偏移量
}fileHeader;
#pragma pack()

/*
位图数据信息结构
*/
#pragma pack(1)
typedef struct tagBITMAPINFOHEADER1
{
    unsigned long biSize;//该结构的大小
    long biWidth;//文件宽度
    long biHeight;//文件高度
    unsigned short biPlanes;//平面数
    unsigned short biBitCount;//颜色位数
    unsigned long biCompression;//压缩类型
    unsigned long biSizeImage;//DIB数据区大小
    long biXPixPerMeter;
    long biYPixPerMeter;
    unsigned long biClrUsed;//多少颜色索引表
    unsigned long biClrImporant;//多少重要颜色
}fileInfo;
#pragma pack()
/*
调色板结构
*/
#pragma pack(1)
typedef struct tagRGBQUAD1
{
    unsigned char rgbBlue; //蓝色分量亮度
    unsigned char rgbGreen;//绿色分量亮度
    unsigned char rgbRed;//红色分量亮度
    unsigned char rgbReserved;
}rgbq;
#pragma pack()

WCHAR* BStr2WStr(const char *pSource, int iDstLen)
{
	iDstLen = MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), NULL, 0);
	WCHAR *pWDst = new WCHAR[iDstLen + 1];
	memset(pWDst, 0, sizeof(WCHAR) * (iDstLen + 1));
	MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), pWDst, iDstLen);
	return pWDst;
}

int GetCodecClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;
	
	ImageCodecInfo* pImageCodecInfo = NULL;
	
	GetImageEncodersSize(&num, &size);
	if(size == 0)
	{
		return -1;
	}
	
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
	{
		return -1;
	}
	
	GetImageEncoders(num, size, pImageCodecInfo);
	
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	
	return -1;
}

bool CreateBmp(BYTE*& pPos, BYTE* pBmpData, int width, int height, int len, int iQuality)
{
	if(pPos==NULL || pBmpData==NULL)
	{
		return false;
	}

	////头结构和文件信息
	fileInfo fi;
	fi.biSize           = sizeof(fileInfo);
	fi.biWidth          = width;
	fi.biHeight			= height;
	fi.biPlanes			= 1;
	fi.biBitCount		= ( (iQuality < 0)?8:24 );
	fi.biCompression	= BI_RGB;
	fi.biSizeImage		= 0;
	fi.biXPixPerMeter	= 0;
	fi.biYPixPerMeter   = 0;
	fi.biClrUsed        = 0;
	fi.biClrImporant    = 0;
	fi.biSizeImage		= ( (fi.biWidth*3+3)/4 ) * 4*fi.biHeight;

	fileHeader fh;
	fh.bfType[0] = 0x42;
	fh.bfType[1] = 0x4D;
	fh.bfReserved1 = fh.bfReserved2 = 0;
	fh.bfOffBits = sizeof(fileHeader)+sizeof(fileInfo)+256*sizeof(rgbq);
	fh.bfSize    = fh.bfOffBits + fi.biSizeImage;

	memcpy(pPos,&fh,sizeof(fileHeader));
	pPos += sizeof(fileHeader);

	memcpy(pPos,&fi,sizeof(fileInfo));
	pPos += sizeof(fileInfo);

	//创建调色版
	int i,j;
	rgbq* pRgbq = new rgbq[256];
	for(i=0;i<256;i++)
	{
		pRgbq[i].rgbBlue=pRgbq[i].rgbGreen=pRgbq[i].rgbRed=i;
	}

	memcpy(pPos,pRgbq,256*sizeof(rgbq));
	pPos += 256*sizeof(rgbq);

	delete[] pRgbq;
	pRgbq = NULL;

	//////////////////////////////////////////////////////////////////////////
	//生成灰度图（iQuality<0,灰度图; iQuality>=0,彩图）
	if(iQuality < 0)
	{
		BYTE ImgData[3] = {0};
		for ( i=0;i<fi.biHeight;i++ )
		{
			for(j=0;j<(fi.biWidth+3)/4*4;j++)
			{
				memcpy((char*)ImgData,pBmpData,3);

				pPos[0] = (BYTE)( ( (int)ImgData[0] * 114 + (int)ImgData[1] * 587 + (int)ImgData[2] * 299 + 500 ) / 1000 );
				pPos++;

				pBmpData += 3;
			}
		}
	}
	else
	{
		memcpy(pPos,pBmpData,len);
	}

	return true;
}

bool IStreamToJpg(IStream* pInIStream, char* pSavePath, int iQuality)
{
	if(iQuality < 0)
	{
		iQuality = -iQuality;
	}

	CLSID codecClsid;
	EncoderParameters encoderParameters;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	GetCodecClsid(L"image/jpeg", &codecClsid);
	
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &iQuality;

	WCHAR* pTmpJpg = BStr2WStr(pSavePath,strlen(pSavePath));
	Image* pImg = Image::FromStream(pInIStream);
	Status bStatus = pImg->Save(pTmpJpg, &codecClsid, &encoderParameters);
	delete pImg;
	delete pTmpJpg;

	GdiplusShutdown(gdiplusToken);

	return (bStatus == Ok);
}

bool CreateEncodeJpeg(char* pFile, BOOL isscreen = TRUE)
{
	bool bOk = false;
	FILE* fp = fopen(pFile,"rb");
	if(fp)
	{
		fseek(fp,0,SEEK_END);
		int i = ftell(fp);
		fseek(fp,0,SEEK_SET);
		if(i>0)
		{
			char* pData = new char[i+1];
			if(pData)
			{
				memset(pData,0,i+1);
				int j = fread(pData,1,i,fp);
				if(j == i)
				{
					ScreenFile scFile;
					if(scFile.CreateJpeg(pData, i, isscreen))
					{
						PInterlockedExchange(&tPrevSnap, time(0));
						bOk = true;
					}
				}

				delete[] pData;
				pData = NULL;
			}
		}
		fclose(fp);
	}

	return bOk;
}

// iQuality:图像质量 0~100
bool BMPToJPG(BYTE *pBmpData, int width, int height, int len, int iQuality)
{
	if(pBmpData == NULL)
	{
		return false;
	}

	///用 GlobalAlloc 申请内存，用于创建流
	DWORD dwSize = sizeof(fileHeader)+sizeof(fileInfo)+256*sizeof(rgbq)+len+4;
	if(iQuality < 0 )
	{
		dwSize += len/3;
	}
	else
	{
		dwSize += len;
	}

	HGLOBAL hMem = GlobalAlloc(GMEM_FIXED,dwSize);
	if(hMem == NULL)
	{
		return false;
	}

	BYTE* pMem = (BYTE*)hMem;
	BYTE* pPos = pMem;

	///创建bmp
	CreateBmp(pPos, pBmpData, width, height, len, iQuality);
	
	///临时jpg文件路径
	char flgTempPath[MAX_PATH];
	memset(flgTempPath, 0, MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	

	char jpgFile[MAX_PATH];
	memset(jpgFile, 0, MAX_PATH);
	sprintf(jpgFile, "%s\\%d.jpg", flgTempPath, (int)time(0));

	///生成临时jpg文件
	bool bSuccess = false;
	IStream* pIStream = NULL;
	CreateStreamOnHGlobal(hMem,TRUE,&pIStream);
	if(pIStream)
	{
		bSuccess = IStreamToJpg(pIStream,jpgFile,iQuality);
		pIStream->Release();
	}

	GlobalFree(hMem);

	///生成加密的jpg
	if (bSuccess)
	{
		bSuccess = CreateEncodeJpeg(jpgFile);
	}

	DeleteFileA(jpgFile);
	return bSuccess;
}
