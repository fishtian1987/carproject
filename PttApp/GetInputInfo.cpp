// GetInputInfo.cpp: implementation of the GetInputInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GetInputInfo.h"
#include "NdisSocket.h"
#include "WinApi.h"

#pragma warning(disable : 4786)

#include <string>
#include <map>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef map<string, string> LanguageMap;
typedef LanguageMap::iterator LanguageIter;
typedef map<string, bool> KeyboardMap;
typedef KeyboardMap::iterator KeyboardIter;


void EnumLanguage(LanguageMap &LMap)
{
	HKEY hkey;
	unsigned long SubValueCnt;
	unsigned long i;

	char SubKeyName[MAX_PATH], SubValueName[MAX_PATH];
	string strKeyName, strValueName;
	unsigned long keylen, valuelen;
	char *pos;
	char TmpDeStr[100];

	LMap.clear();

	//MIME\\Database\\Rfc1766
	if(PRegOpenKeyExA(HKEY_CLASSES_ROOT, SysinfStringA(TmpDeStr, 100, "4U4F\\CzpzGzS8\\w2Ajlvv"), 0, KEY_READ, &hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,NULL,NULL,NULL,&SubValueCnt,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0; i<SubValueCnt; i++)
			{
				keylen = valuelen = MAX_PATH;
				memset(SubKeyName, 0, sizeof(SubKeyName));
				memset(SubValueName, 0, sizeof(SubValueName));
				if(PRegEnumValueA(hkey, i, SubKeyName, &keylen, NULL, NULL, (unsigned char *)SubValueName, &valuelen) != ERROR_SUCCESS)
					continue;
				
				pos = strstr(SubValueName, ";@");
				if (pos != NULL)
				{
					pos[0] = '\0';
					strKeyName = strlwr(SubKeyName);
					strValueName = SubValueName;
					LMap.insert(make_pair(strKeyName, strValueName));
				}
			}
		}

		PRegCloseKey(hkey);
	}

}

void EnumKeyboardLayout(KeyboardMap &Preload)
{
	HKEY hkey;
	unsigned long SubValueCnt;
	unsigned long i;
	
	char SubKeyName[MAX_PATH], SubValueName[MAX_PATH];
	string strValueName;
	unsigned long keylen, valuelen;
	char TmpDeStr[100];
	
	Preload.clear();

	//Keyboard Layout\\Preload
	if(PRegOpenKeyExA(HKEY_CURRENT_USER, SysinfStringA(TmpDeStr, 100, "P81GLzXO dz1Lop\\TX8rLzO"), 0, KEY_READ, &hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,NULL,NULL,NULL,&SubValueCnt,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0; i<SubValueCnt; i++)
			{
				keylen = valuelen = MAX_PATH;
				memset(SubKeyName, 0, sizeof(SubKeyName));
				memset(SubValueName, 0, sizeof(SubValueName));
				if(PRegEnumValueA(hkey, i, SubKeyName, &keylen, NULL, NULL, (unsigned char *)SubValueName, &valuelen) != ERROR_SUCCESS)
					continue;
				
				strValueName = strlwr(SubValueName);
				Preload.insert(make_pair(strValueName, true));
			}
		}
		
		PRegCloseKey(hkey);
	}
}

int EnumInput(char* buf)
{
	LanguageMap LMap;
	EnumLanguage(LMap);

	KeyboardMap PreloadMap;
	EnumKeyboardLayout(PreloadMap);

	HKEY hkey, hsubkey;          
	unsigned long SubKeyCnt;
	char szlayout[100], szLanguageID[100];
	char *playout;
	unsigned long type;
	unsigned long i;

	char tmpbuf[4*MAX_PATH], szLayoutText[3*MAX_PATH], szLanguage[MAX_PATH], szState[10];
	int tmpbuflen;
	WCHAR wLayoutText[MAX_PATH];
	unsigned long len;
	int res;
	int offset=0;
	char TmpDeStr[100];
	
	//SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts
	int count = 0;
	if(PRegOpenKeyExA(HKEY_LOCAL_MACHINE, SysinfStringA(TmpDeStr, 100, "fVfgF4\\9oXX8Wp9LWpXLrf8p\\9LWpXLr\\P81GLzXO dz1LopS"), 0, KEY_READ,&hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,&SubKeyCnt,NULL,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0; i<SubKeyCnt; i++)
			{
				len=2*MAX_PATH;
				if(PRegEnumKeyExA(hkey,i,szlayout,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
					continue;
				
				if(PRegOpenKeyExA(hkey,szlayout,0,KEY_QUERY_VALUE,&hsubkey)!=ERROR_SUCCESS) //打开键失败
					continue;
				
				playout = strlwr(szlayout);

				//判断是否安装
				KeyboardIter kIter = PreloadMap.find(playout);
				if (kIter != PreloadMap.end())
				{
					strcpy(szState, "1");
				}
				else
				{
					continue;
				}

				memset(wLayoutText, 0, sizeof(wLayoutText));
				len=2*MAX_PATH;
				//Layout Text
				res=PRegQueryValueExW(hsubkey, L"Layout Text", 0, &type, (unsigned char *)wLayoutText, &len);
				
				if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
				{
					PRegCloseKey(hsubkey);
					continue;
				}
				
				PRegCloseKey(hsubkey);

				Unicode2UTF_8(wLayoutText, szLayoutText, 3*MAX_PATH);

				//语言编号
				strcpy(szLanguageID, playout+4);
				LanguageIter lIter = LMap.find(szLanguageID);
				if (lIter != LMap.end())
				{
					strcpy(szLanguage, lIter->second.c_str());
				}
				else
				{
					memset(szLanguage, 0, sizeof(szLanguage));
				}
				
				memset(tmpbuf, 0, sizeof(tmpbuf));
				tmpbuflen = sprintf(tmpbuf, "%d<t>%s<t>%s<t>%s\n", count,szLayoutText, szLanguage, szState);
				memcpy(buf+offset, tmpbuf, tmpbuflen);
				offset += tmpbuflen;
				count++;
			}

			memset(tmpbuf, 0, sizeof(tmpbuf));
			tmpbuflen = sprintf(tmpbuf, "%d", count);
			memcpy(buf+offset, tmpbuf, tmpbuflen);
			offset += tmpbuflen;
			count++;
		}
		
		PRegCloseKey(hkey);
	}	

	return offset;
}

DWORD WINAPI ThreadGetInput(LPVOID lp)
{
	char *p=0;//目录名
	char tmpbuf[1024];
	//取补丁信息
	/*		char *UpdateInfo;*/
	char* SendBuf;
	int offset = 1024;
	
	SendBuf = new char[102400];
	ZeroMemory(SendBuf,102400);
	
	int tmp = EnumInput(SendBuf+offset);
	offset+=tmp;
	
	// 生成描述信息
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	char inputinfofilename[MAX_PATH];
	char inputinfofilea[MAX_PATH];
	char FixDir[10];
	strcpy(FixDir, GetTmpPrevFix(FIX_SYSTEMINFO));
	CreateFileName(FixDir, inputinfofilename, inputinfofilea);		// 得到目标文件名
	
	int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: inputinfo_new\r\n\r\n",
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond, inputinfofilea, offset-1024);
	
	int datalen = offset-1024 + len;
	memcpy(SendBuf+1024-len, tmpbuf, len);
	
	char *pBuffer = new char[datalen+10];
	memset(pBuffer, 0, datalen+10);
		memcpy(pBuffer, SendBuf+1024-len, datalen);

	myWriteFile(pBuffer, datalen, inputinfofilename, inputinfofilea);
	
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

DWORD ThreadGetInputStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadGetInput, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}
