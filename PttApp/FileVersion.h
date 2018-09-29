// FileVersion.h: interface for the CFileVersion class.  
// 
//////////////////////////////////////////////////////////////////////  
#ifndef __FILEVERSION_H_  
#define __FILEVERSION_H_  
#if _MSC_VER >= 1000  
#pragma once  
#endif // _MSC_VER >= 1000

#pragma warning(disable : 4786)

#include <string>

using namespace std;

class CFileVersion  
{   
	// Construction  
public:   
    CFileVersion();  
	// Operations     
public:   
    BOOL    Open(LPCSTR lpszModuleName);  
    void    Close();  
    string  QueryValue(LPCSTR lpszValueName, DWORD dwLangCharset = 0);  
    string  GetFileDescription()  {return QueryValue("FileDescription"); };  
    string  GetFileVersion()      {return QueryValue("FileVersion");     };  
    string  GetInternalName()     {return QueryValue("InternalName");    };  
    string  GetCompanyName()      {return QueryValue("CompanyName");     };   
    string  GetLegalCopyright()   {return QueryValue("LegalCopyright");  };  
    string  GetOriginalFilename() {return QueryValue("OriginalFilename");};  
    string  GetProductName()      {return QueryValue("ProductName");     };  
    string  GetProductVersion()   {return QueryValue("ProductVersion");  };  
    BOOL    GetFixedInfo(VS_FIXEDFILEINFO& vsffi);  
    string  GetFixedFileVersion();  
    string  GetFixedProductVersion(); 
	void	GetNumFileVersion(DWORD &dwMajorVersion, DWORD &dwMinorVersion);
	// Attributes  
protected:  
    LPBYTE  m_lpVersionData;   
    DWORD   m_dwLangCharset;   
	// Implementation  
public:  
    ~CFileVersion();   
};   
#endif  // __FILEVERSION_H_ 