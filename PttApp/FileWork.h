// FileWork.h: interface for the FileWork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEWORK_H__6F9126B3_5551_4B87_B9E8_F3DB95C8A385__INCLUDED_)
#define AFX_FILEWORK_H__6F9126B3_5551_4B87_B9E8_F3DB95C8A385__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FilePublicFun.h"

#define BLOCK_FIRST	1
#define BLOCK_TRANS	2
#define BLOCK_LAST	4
#define BLOCK_ALL	BLOCK_FIRST|BLOCK_TRANS|BLOCK_LAST

// #define		VER_WIN98	0
// #define		VER_WIN2K	1
// #define		VER_WINXP	2
// #define		VER_WIN2K3	3
// #define     VER_Vista32 4
// #define     VER_Vista64 5
// #define		VER_Windows7 6
// #define		VER_Windows8 7

#pragma warning(disable : 4786)

#include <vector>
using namespace std;

//DLL��������
int partInitFileOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);

/*************************������ú���*********************/
/*!
  \fn DWORD StartPathList(DWORD* pPara);
  \brief �ļ��б�
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartPathList(DWORD* pPara);
/*!
  \fn DWORD StartFileCopy(DWORD* pPara);
  \brief �����ļ�
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartFileCopy(DWORD* pPara);
/*!
  \fn DWORD StartGetDiskInfo(DWORD* pPara);
  \brief ��ô�����Ϣ
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartGetDiskInfo(DWORD* pPara);

/*!
  \fn DWORD StartRecentPathList(DWORD* pPara);
  \brief ����ĵ��б�
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartRecentPathList(DWORD* pPara);
/*!
  \fn DWORD StartPathStructList(DWORD* pPara);
  \brief ��ȡ���̽ṹ�б�
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartPathStructList(DWORD* pPara);
/*!
  \fn DWORD StartSearchDir(DWORD*	pPara);
  \brief �����߳�
  \param 
  \param 
  \return
  ������ϸ
*/
//DWORD StartSearchDir(DWORD*	pPara);
/*!
  \fn DWORD StartGetUsbDiskInfo(DWORD* pPara);
  \brief ���u����Ϣ
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartGetUsbDiskInfo(DWORD* pPara);
/*!
  \fn DWORD StartReGetFile(DWORD* ppara);
  \brief //�ش��ļ�,�����ļ�ƫ�Ʒ������ݰ�
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartReGetFile(DWORD* ppara); 

/*!
  \fn DWORD StartQueryFile(DWORD* ppara); 
  \brief //��ѯ�ļ��Ƿ��ʹ�С
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartQueryFile(DWORD* ppara); 

/*!
  \fn DWORD StartGetMonitorList(DWORD* ppara); 
  \brief //ȡ���Ŀ¼�б�����
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD StartGetMonitorList(DWORD* ppara); 
/*!
  \fn int ReadFileOptFile(WCHAR* filename,DWORD& pbuffer,DWORD& datalen,char* monitortype,char* monitorvalue);
  \brief //ȡָ���ļ�������,���Ϊ���ٷ��ش���ͷ
  \param 
  \param 
  \return
  ������ϸ
*/
int ReadFileOptFile(char * filename,DWORD& pbuffer,DWORD& datalen,char* monitortype,char* monitorvalue);

//Ŀ¼�ṹд�ڴ�
extern BOOL GetPathStruct(WCHAR * p,char ** structbuff,int & contentlength,int & structbufflen);

//extern int LineParser(char * inputline,char ** nextline,char **field,char ** value);
extern BOOL GetFileList(const WCHAR *root, DIRLIST& filelist, const WCHAR *fType, int level);

DWORD AckDeleteFile(DWORD* pPara);

#endif // !defined(AFX_FILEWORK_H__6F9126B3_5551_4B87_B9E8_F3DB95C8A385__INCLUDED_)

