// NdisSocket.h: interface for the NdisSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NDISSOCKET_H__4D482587_E5C4_4114_9831_9B51683E2AA6__INCLUDED_)
#define AFX_NDISSOCKET_H__4D482587_E5C4_4114_9831_9B51683E2AA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//����Ϊ5
typedef struct  _TaskUnit_
{
	WORD   sessionid;//
	int    state;  //״̬  1  ִ��, 0 ֹͣ   
	_TaskUnit_()
	{
		sessionid=0;
		state=0;
	}
}TaskUnit,*lpTaskUnit;

#define  Max_TaskNum  5 

extern long	v_currentSend5;
extern long	v_currentSend13;

BOOL BootSendData(char* buf,int len,BOOL isHeart=FALSE);

BOOL StartProcessCmd(char *buffer, int buflen);

BOOL SendUdpData(char* buf, DWORD bufLen);
BOOL SendTcpData(char* buf, DWORD bufLen);

/*!
  \fn BOOL	SendBuffer2List(
					 char * buf, 
					 int len, 
					 UCHAR PacketType, 
					 USHORT SessionID,
					 int iDelay);
  \brief ������ݰ�������
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL	SendBuffer2List(
					 char * buf, 
					 int len, 
					 UCHAR PacketType, 
					 USHORT SessionID);

/*!
  \fn BOOL SendFileBuffer2List(char * buf, int len, UCHAR PacketType, USHORT	SessionID, int SendIdx);
  \brief �����ļ������ݰ���ӵ�����
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL SendFileBuffer2List(char * buf, int len, UCHAR PacketType, USHORT	SessionID);



/*!
  \fn BOOL SendReFileBuffer2List(
		char * buf, int len, UCHAR PacketType, USHORT	SessionID, int iDelay);
  \brief �ش���������
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL SendReFileBuffer2List(char * buf, int len, UCHAR PacketType, USHORT SessionID);

/*!
  \fn BOOL isPacketBig(UCHAR PacketType);
  \brief �Ƿ�����
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL isPacketBig(UCHAR PacketType);

DWORD SendData(char * buf,int len,UCHAR PacketType,USHORT SessionID);//���ⷢ�Ͱ�

lpTaskUnit AllocTask(WORD sessionid);
BOOL ClearTask(WORD sessionid,BOOL bclearid);

BOOL AddPacket(char * buf, int len, UCHAR PacketType, BOOL bFirstPacket);

BOOL gettaskallow(long &hashallow);
BOOL fat32releaseallow(long &hashallow);
BOOL fat32getallow(long &hashallow);

DWORD Cmd_AdjustSpeed(DWORD* pPara);

#endif // !defined(AFX_NDISSOCKET_H__4D482587_E5C4_4114_9831_9B51683E2AA6__INCLUDED_)
