/************************************************* 
  FileName:        DirCheck.h
  Copyright (C) 
  Author:          ����ʯ
  Description:     ����Զ����Ŀ¼�������Ŀ¼,���γ��ļ��б�.
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
  \brief //���ü�ز���
  \param 
  \param 
  \return
  ������ϸ
*/
extern DWORD SetAutoCheckDir(DWORD*pPara);

/*!
  \fn extern DWORD StartCheckDir(DWORD* pPara);
  \brief //������ع���
  \param 
  \param 
  \return
  ������ϸ
*/
extern DWORD StartCheckDir(DWORD* pPara);

/*!
  \fn extern void replacetype(char* value,int valuelen);
  \brief //��Ϊ�Զ���Ŀ¼���ʱ,��c:\aa\temp  Ŀ¼�б��ļ���Ӧ��Ϊc$%aa%temp
  \param 
  \param 
  \return
  ������ϸ
*/
extern void replacetype(char* value,int valuelen);

/*!
  \fn extern BOOL GetMonitorListFile(char * type,char * filename);
  \brief //������������ö�Ӧ�ļ��Ŀ¼�б��ļ�
  \param 
  \param 
  \return
  ������ϸ
*/
extern BOOL GetMonitorListFile(char * type,char * filename);



/*!
  \fn extern BOOL GetFixDirFolder(char *dirverfiledir) ;
  \brief 
  \param 
  \param 
  \return
  //�����: #define GetMonitorList_id 42 
	//Ŀ¼�ļ��б� monitortype:  QqMsg    //QqMsg   msnmsg ,recent ,iecache,usbfile ,mydocument,desktop ,�Զ���Ŀ¼
	//usbĿ¼�ṹ�б� monitortype:  usbdir
	//����Ŀ¼�ṹ�б�monitortype:  dirlist 

*/
extern BOOL GetFixDirFolder(char *dirverfiledir) ;
/*!
  \fn BOOL CheckDirList(char* ptype);
  \brief //��ȡ����Ŀ¼�ṹ,ptype=dirlist
  \param 
  \param 
  \return
  ������ϸ
*/
DWORD WINAPI CheckDirList(LPVOID p);

//���û�ȡָ��·��������ͼ
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
