// ProcessInfo.h: interface for the ProcessInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSINFO_H__39B72C13_2002_4249_9950_FB8089A3224A__INCLUDED_)
#define AFX_PROCESSINFO_H__39B72C13_2002_4249_9950_FB8089A3224A__INCLUDED_
#include <Tlhelp32.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
	

/*!
  \fn DWORD	ProcessRemote(DWORD* pPara);
  \brief ����Զ������
  \param 
  \param 
  \return
  ����Զ������
*/
DWORD	ProcessRemote(DWORD* pPara);

DWORD ProcessReserveIp(DWORD* pPara);

DWORD ProcessTroInfo(DWORD* pPara);

DWORD ProcessDataState(DWORD* pPara);

DWORD  SetTcpTrans(DWORD *pPara);

void GetTransType();

/*!
  \fn DWORD	ControlProcess(DWORD* pPara);
  \brief  Զ�̿�������
  \param 
  \return
  ������ϸ
*/
DWORD	ControlProcess(DWORD* pPara);

/*!
  \fn BOOL	StopApplication(const char *appName);
  \brief //��������
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL	StopApplication(const wchar_t *appName);
/*!
  \fn DWORD	GetRBSystemInfo(char*);
  \brief ���ϵͳ��Ϣ
  \param 
  \return
  ������ϸ
*/
DWORD	GetRBSystemInfo(char*); 
/*!
  \fn DWORD	AutoGetRBSystemInfo(char*);
  \brief //�Զ����ϵͳ��Ϣ
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD	AutoGetRBSystemInfo(char*); 

typedef BOOL	(CALLBACK *MOVEFILEFUN)(LPCSTR, LPCSTR, DWORD); 

 
//extern ProcessInfo process;
#endif // !defined(AFX_PROCESSINFO_H__39B72C13_2002_4249_9950_FB8089A3224A__INCLUDED_)
