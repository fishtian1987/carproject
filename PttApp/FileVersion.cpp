// FileVersion.cpp: implementation of the CFileVersion class.  
// 
//////////////////////////////////////////////////////////////////////  
#include "StdAfx.h"
#include "FileVersion.h"  
#include "WinApi.h"

//////////////////////////////////////////////////////////////////////  
CFileVersion::CFileVersion()   
{   
    m_lpVersionData = NULL;  
    m_dwLangCharset = 0;  
}  
CFileVersion::~CFileVersion()   
{   
    Close();  
}   
void CFileVersion::Close()  
{  
    delete[] m_lpVersionData;   
    m_lpVersionData = NULL;  
    m_dwLangCharset = 0;  
}  
BOOL CFileVersion::Open(LPCSTR lpszModuleName)  
{
	if (!(strlen(lpszModuleName) > 0))
	{
		return FALSE;
	}
	if (!(m_lpVersionData == NULL))
	{
		return FALSE;
	}
    // Get the version information size for allocate the buffer  
    DWORD dwHandle;       
    DWORD dwDataSize = PGetFileVersionInfoSizeA((LPSTR)lpszModuleName, &dwHandle);   
    if ( dwDataSize == 0 )   
        return FALSE;  
    // Allocate buffer and retrieve version information  
    m_lpVersionData = new BYTE[dwDataSize];   
    if (!PGetFileVersionInfoA((LPSTR)lpszModuleName, dwHandle, dwDataSize,   
                              (void**)m_lpVersionData) )  
    {  
        Close();  
        return FALSE;  
    }  
    // Retrieve the first language and character-set identifier  
    UINT nQuerySize;  
    DWORD* pTransTable;  
    if (!PVerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation",  
                         (void **)&pTransTable, &nQuerySize) )  
    {  
		int err = PGetLastError();
        Close();  
        return FALSE;  
    }  
    // Swap the words to have lang-charset in the correct format  
    m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));  
    return TRUE;  
}  
string CFileVersion::QueryValue(LPCSTR lpszValueName,   
                                 DWORD dwLangCharset /* = 0*/)  
{  
    // Must call Open() first  
	string strValue = "";
    if ( m_lpVersionData == NULL )  
        return strValue;  
    // If no lang-charset specified use default  
    if ( dwLangCharset == 0 )  
        dwLangCharset = m_dwLangCharset;  
    // Query version information value  
    UINT nQuerySize;  
    LPVOID lpData;  
	char strBlockName[MAX_PATH];  
    sprintf(strBlockName, "\\StringFileInfo\\%08lx\\%s",   
                         dwLangCharset, lpszValueName);  
    if ( PVerQueryValueA((void **)m_lpVersionData, strBlockName,   
                         &lpData, &nQuerySize) )  
        strValue = (LPCSTR)lpData;  
    
    return strValue;  
}  
BOOL CFileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)  
{  
    // Must call Open() first  
    if ( m_lpVersionData == NULL )  
        return FALSE;  
    UINT nQuerySize;  
    VS_FIXEDFILEINFO* pVsffi;  
    if ( PVerQueryValueA((void **)m_lpVersionData, "\\",  
                         (void**)&pVsffi, &nQuerySize) )  
    {  
        vsffi = *pVsffi;  
        return TRUE;  
    }  
    return FALSE;  
}  
string CFileVersion::GetFixedFileVersion()  
{  
    string strVersion;
	char tmpVersion[MAX_PATH];
    VS_FIXEDFILEINFO vsffi;  
    if ( GetFixedInfo(vsffi) )  
    {  
        sprintf(tmpVersion, "%u,%u,%u,%u", HIWORD(vsffi.dwFileVersionMS),  
            LOWORD(vsffi.dwFileVersionMS),  
            HIWORD(vsffi.dwFileVersionLS),  
            LOWORD(vsffi.dwFileVersionLS));  
    }
	strVersion = tmpVersion;
    return strVersion;  
}  

string CFileVersion::GetFixedProductVersion()  
{  
    string strVersion;
	char tmpVersion[MAX_PATH];
    VS_FIXEDFILEINFO vsffi;  
    if ( GetFixedInfo(vsffi) )  
    {  
        sprintf(tmpVersion, "%u,%u,%u,%u", HIWORD(vsffi.dwProductVersionMS),  
            LOWORD(vsffi.dwProductVersionMS),  
            HIWORD(vsffi.dwProductVersionLS),  
            LOWORD(vsffi.dwProductVersionLS));  
    }
	strVersion = tmpVersion;
    return strVersion;  
}

void CFileVersion::GetNumFileVersion(DWORD &dwMajorVersion, DWORD &dwMinorVersion)
{
    VS_FIXEDFILEINFO vsffi;  
    if ( GetFixedInfo(vsffi) )  
    {
		dwMajorVersion = HIWORD(vsffi.dwProductVersionMS);
		dwMinorVersion = LOWORD(vsffi.dwProductVersionMS);
	}
	return ;
}