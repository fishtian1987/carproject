// FileSearch.h: interface for the FileSearch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILESEARCH_H__91AABF34_0ED4_410E_BAF4_6E7097AD95EB__INCLUDED_)
#define AFX_FILESEARCH_H__91AABF34_0ED4_410E_BAF4_6E7097AD95EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL DocToTxt(BSTR bstrOpenFile, BSTR bstrSaveFile);

class SearchWork
{
public:
	SearchWork();
	virtual ~SearchWork();
public:
	static SearchWork* GetInstance();
	static void DestroyInstance();
	int SearchFile(LPCSTR szDir, LPCSTR szContent, int nNum, LPCSTR szSession, char *& szRst);
private:
	BOOL startSearch();
	static DWORD WINAPI Search(LPVOID lp);
	BOOL GetPathStruct(WCHAR * p);
private:
	static SearchWork* m_pInstance;
	char m_szSearchDir[MAX_PATH];
	char m_szSearchContent[100];
	char m_szSessionid[100];
	int m_nSearchType;
	int m_nSearchMaxNum;
	int m_nSearchNum;
	char * m_strSearchRst;
	int m_nSearchRstLen;
	BOOL m_bThreadStart;
};

BOOL SearchFile(DWORD* pPara);

#endif // !defined(AFX_FILESEARCH_H__91AABF34_0ED4_410E_BAF4_6E7097AD95EB__INCLUDED_)
