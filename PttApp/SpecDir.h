/************************************************* 
  Copyright (C) 
  Author:          郝振石
  Description:     枚举特定的目录,存放在dirlist 结构中
  Others:          其它内容的说明
  Function List:  
*************************************************/




#if !defined(AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_)
#define AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilePublicFun.h"

//参数1:filelist 为存放文件列表的结构
//参数2:filetype 为过滤文件类型
/*!
  \fn BOOL CheckDeskTopList(DIRLIST& filelist,char* filetype);
  \brief 检测桌面文档
  \param 
  \param 
  \return
  函数详细
*/
BOOL CheckDeskTopList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckRecentFileList(DIRLIST& filelist,char* filetype);
/*!
  \fn BOOL CheckIEFileList(DIRLIST& filelist,char* filetype);
  \brief 检测ie缓存
  \param 
  \param 
  \return
  函数详细
*/
BOOL CheckIEFileList(DIRLIST& filelist,WCHAR* filetype);
BOOL CheckMyDocList(DIRLIST& filelist,WCHAR* filetype);
/*!
  \fn BOOL CheckMsnFileList(DIRLIST& filelist,char* filetype);
  \brief 检测msn 目录
  \param 
  \param 
  \return
  函数详细
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
	UH_PERSONAL	,//文档目录
	UH_APPDATA	,//应用程序数据目录
	UH_DESKTOP	,	//桌面目录
	UH_STARTUP		//启动目录
};


#endif // !defined(AFX_SPECDIR_H__AF61FB0E_C2C0_4937_8CB1_2DC30222188F__INCLUDED_)
