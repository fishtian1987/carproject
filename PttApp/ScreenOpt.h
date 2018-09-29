// Screen.h: interface for the Screen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__C8DAAE49_BF12_4CF4_A0CE_4D63B25C56A1__INCLUDED_)
#define AFX_SCREEN_H__C8DAAE49_BF12_4CF4_A0CE_4D63B25C56A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ScreenFile.h"
int partInitSrceenOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);

typedef DWORD (*pSendData)(char * ,int ,UCHAR ,USHORT)  ;	//发送数据的函数指针声明

extern long g_quality;

//extern int GetAutoSendID();

//int SendFile(CString filename, char ptype, WORD session,int monitortype);

#endif // !defined(AFX_SCREEN_H__C8DAAE49_BF12_4CF4_A0CE_4D63B25C56A1__INCLUDED_)
