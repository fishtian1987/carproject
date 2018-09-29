/************************************************* 
  Copyright (C) 
  Author:          ����ʯ
  Description:     ö���ض���Ŀ¼,�����dirlist �ṹ��
  Others:          �������ݵ�˵��
  Function List:  
*************************************************/




#if !defined(AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_)
#define AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilePublicFun.h"

//����1:filelist Ϊ����ļ��б�Ľṹ
//����2:filetype Ϊ�����ļ�����
/*!
  \fn BOOL CheckDeskTopList(DIRLIST& filelist,char* filetype);
  \brief ��������ĵ�
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL CheckDeskTopList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckRecentFileList(DIRLIST& filelist,char* filetype);
/*!
  \fn BOOL CheckIEFileList(DIRLIST& filelist,char* filetype);
  \brief ���ie����
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL CheckIEFileList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckMyDocList(DIRLIST& filelist,WCHAR* filetype);
/*!
  \fn BOOL CheckMsnFileList(DIRLIST& filelist,char* filetype);
  \brief ���msn Ŀ¼
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL CheckMsnFileList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckUsbFileList(DIRLIST& filelist,WCHAR* filetype);


BOOL CheckRecycleFileList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckFetionFileList(DIRLIST& filelist, char* filetype);
BOOL CheckSkypeFileList(DIRLIST& filelist, char* filetype);

BOOL CheckFeiqFileList(DIRLIST& filelist, char* filetype);

//////////////////////////////////////////////////////////////////////////

enum  querytype
{
	UH_PERSONAL	,//�ĵ�Ŀ¼
	UH_APPDATA	,//Ӧ�ó�������Ŀ¼
	UH_DESKTOP	,	//����Ŀ¼
	UH_STARTUP		//����Ŀ¼
};


#endif // !defined(AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_)
