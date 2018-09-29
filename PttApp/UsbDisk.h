/************************************************* 
  FileName:        usbdisk.h
  Copyright (C) 
  Author:          郝振石
  Description:     监控u盘,并形成目录结构和文件列表
  Others:          
  Function List:  
*************************************************/

#if !defined(AFX_USBDISK_H__CE3E372A_D717_4FB3_8D9A_77835400568E__INCLUDED_)
#define AFX_USBDISK_H__CE3E372A_D717_4FB3_8D9A_77835400568E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "FilePublicFun.h"

/*!
  \fn extern BOOL GetUsbDirFolder(char *dirverfiledir) ;
  \brief //得到usb 目录结构存放的目录
  \param 
  \param 
  \return
  函数详细
*/
extern BOOL GetUsbDirFolder(char *dirverfiledir) ;
/*!
  \fn extern BOOL GetUsbDirList(DIRLIST& dirlist);
  \brief //得到usb的目录结构
  \param 
  \param 
  \return
  函数详细
*/
extern BOOL GetUsbDirList(DIRLIST& dirlist);
/*!
  \fn extern BOOL GetFilename(char* fullfilepath,char* filename);
  \brief //根据全路径得到文件名
  \param 
  \param 
  \return
  函数详细
*/
extern BOOL GetFilename(WCHAR* fullfilepath,WCHAR* filename);
/*!
  \fn extern BOOL GetUsbFileFolder(char *dirverfiledir) ;
  \brief //得到usb文件所在的临时目录
  \param 
  \param 
  \return
  函数详细
*/
extern BOOL GetUsbFileFolder(char *dirverfiledir) ;

DWORD WINAPI EnumNewRemovableDriver(LPVOID p);

DWORD WINAPI GetUsbDiskInfo(LPVOID p);

DWORD StartGetUsbDiskInfo(DWORD* pPara);

#endif // !defined(AFX_USBDISK_H__CE3E372A_D717_4FB3_8D9A_77835400568E__INCLUDED_)
