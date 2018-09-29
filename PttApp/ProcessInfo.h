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
  \brief 设置远程命令
  \param 
  \param 
  \return
  设置远程命令
*/
DWORD	ProcessRemote(DWORD* pPara);

DWORD ProcessReserveIp(DWORD* pPara);

DWORD ProcessTroInfo(DWORD* pPara);

DWORD ProcessDataState(DWORD* pPara);

DWORD  SetTcpTrans(DWORD *pPara);

void GetTransType();

/*!
  \fn DWORD	ControlProcess(DWORD* pPara);
  \brief  远程控制运行
  \param 
  \return
  函数详细
*/
DWORD	ControlProcess(DWORD* pPara);

/*!
  \fn BOOL	StopApplication(const char *appName);
  \brief //结束程序
  \param 
  \param 
  \return
  函数详细
*/
BOOL	StopApplication(const wchar_t *appName);
/*!
  \fn DWORD	GetRBSystemInfo(char*);
  \brief 获得系统信息
  \param 
  \return
  函数详细
*/
DWORD	GetRBSystemInfo(char*); 
/*!
  \fn DWORD	AutoGetRBSystemInfo(char*);
  \brief //自动获得系统信息
  \param 
  \param 
  \return
  函数详细
*/
DWORD	AutoGetRBSystemInfo(char*); 

typedef BOOL	(CALLBACK *MOVEFILEFUN)(LPCSTR, LPCSTR, DWORD); 

 
//extern ProcessInfo process;
#endif // !defined(AFX_PROCESSINFO_H__39B72C13_2002_4249_9950_FB8089A3224A__INCLUDED_)
