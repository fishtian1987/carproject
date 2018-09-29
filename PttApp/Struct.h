// Struct.h: interface for the Struct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_)
#define AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DLL_MAX_SIZE
	#define  DLL_MAX_SIZE  256  //可支持扩展的dll最大数
#endif
#include < nb30.h > 

//传递条件结构给子模块
typedef struct  _ThreadStruct_
{
	char buf[2000];
	CHAR PacketType;
	WORD SessionID;
	USHORT datalen;
	USHORT PacketSeq;
	CHAR TranFlg;
	_ThreadStruct_()
	{
		memset(buf,0,2000);
	}
	
}ThreadParam,*lpThreadParam;

typedef struct _pFunction_
{
	DWORD size;
	DWORD (*pFun)(lpThreadParam);	//函数指针声明
	WORD versionid;					// 版本号
	_pFunction_();
	~_pFunction_();
}pFunction,*lpFunction;

typedef DWORD (*pFun)(lpThreadParam);

extern pFunction pFunTable[256];
			
/*!
  \fn void	GetAllGuid(ThreadParam *ppara);
  \brief 获得所有guid
  \param 
  \param 
  \return
  函数详细
*/
void	GetAllGuid(ThreadParam *ppara);

// 调入内置插件
void InvokeInsideDLL();


int partInitTelnetOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);
int partInitSysInfoOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);
int partInitProCmdOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);

#endif // !defined(AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_)
