// publicvar.h: interface for the publicvar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLICVAR_H__0CEC6AF3_B016_4453_9D1E_D4194EFBC804__INCLUDED_)
#define AFX_PUBLICVAR_H__0CEC6AF3_B016_4453_9D1E_D4194EFBC804__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	 SENDBUFFLEN		10240

#define  SIZE1G				1024*1024*1024

void FileTimeToTime_t(FILETIME ft, time_t *t);

typedef struct  File_STRUCT_FILEINFO
{
	WCHAR		filename[MAX_PATH+1];
	int			filetype; //1   Ϊ�ļ���,0 Ϊ�ļ�,2 Ϊ��Ҫɾ��
	unsigned __int64		filesize;
	FILETIME	filetime;//�޸�ʱ��
	FILETIME	createfiletime;//����ʱ��
	FILETIME	accesstime;//����ʱ��
}FileFILEINFO;

typedef struct  STRUCT_DIRLIST
{
	WCHAR		rootdir[MAX_PATH+1];
	FileFILEINFO*	filelist;
	int			filecount;
	STRUCT_DIRLIST(){
		filelist  = 0;
		filecount = 0;
	}
	~STRUCT_DIRLIST(){
		if(filelist!=0)
			delete []filelist;
	}
}DIRLIST;

BOOL  QueryFileInfo(WCHAR *filename, FILETIME &ftWrite, FILETIME &ftCreate, FILETIME &ftAccess, unsigned __int64 &filesize);

BOOL GetDirList(DIRLIST& dirlist);

BOOL S_GetPathStruct(FILE * fs,WCHAR * p);

#endif // !defined(AFX_PUBLICVAR_H__0CEC6AF3_B016_4453_9D1E_D4194EFBC804__INCLUDED_)
