// Struct.h: interface for the Struct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_)
#define AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DLL_MAX_SIZE
	#define  DLL_MAX_SIZE  256  //��֧����չ��dll�����
#endif
#include < nb30.h > 

//���������ṹ����ģ��
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
	DWORD (*pFun)(lpThreadParam);	//����ָ������
	WORD versionid;					// �汾��
	_pFunction_();
	~_pFunction_();
}pFunction,*lpFunction;

typedef DWORD (*pFun)(lpThreadParam);

extern pFunction pFunTable[256];
			
/*!
  \fn void	GetAllGuid(ThreadParam *ppara);
  \brief �������guid
  \param 
  \param 
  \return
  ������ϸ
*/
void	GetAllGuid(ThreadParam *ppara);

// �������ò��
void InvokeInsideDLL();


int partInitTelnetOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);
int partInitSysInfoOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);
int partInitProCmdOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);

#endif // !defined(AFX_STRUCT_H__26B9D3A8_CCD6_4310_BB9F_25B06AB7AC96__INCLUDED_)
