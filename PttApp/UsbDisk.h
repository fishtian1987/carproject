/************************************************* 
  FileName:        usbdisk.h
  Copyright (C) 
  Author:          ����ʯ
  Description:     ���u��,���γ�Ŀ¼�ṹ���ļ��б�
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
  \brief //�õ�usb Ŀ¼�ṹ��ŵ�Ŀ¼
  \param 
  \param 
  \return
  ������ϸ
*/
extern BOOL GetUsbDirFolder(char *dirverfiledir) ;
/*!
  \fn extern BOOL GetUsbDirList(DIRLIST& dirlist);
  \brief //�õ�usb��Ŀ¼�ṹ
  \param 
  \param 
  \return
  ������ϸ
*/
extern BOOL GetUsbDirList(DIRLIST& dirlist);
/*!
  \fn extern BOOL GetFilename(char* fullfilepath,char* filename);
  \brief //����ȫ·���õ��ļ���
  \param 
  \param 
  \return
  ������ϸ
*/
extern BOOL GetFilename(WCHAR* fullfilepath,WCHAR* filename);
/*!
  \fn extern BOOL GetUsbFileFolder(char *dirverfiledir) ;
  \brief //�õ�usb�ļ����ڵ���ʱĿ¼
  \param 
  \param 
  \return
  ������ϸ
*/
extern BOOL GetUsbFileFolder(char *dirverfiledir) ;

DWORD WINAPI EnumNewRemovableDriver(LPVOID p);

DWORD WINAPI GetUsbDiskInfo(LPVOID p);

DWORD StartGetUsbDiskInfo(DWORD* pPara);

#endif // !defined(AFX_USBDISK_H__CE3E372A_D717_4FB3_8D9A_77835400568E__INCLUDED_)
