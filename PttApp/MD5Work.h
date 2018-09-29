// MD5Work.h: interface for the CMD5Work class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5WORK_H__6DA752FD_1CCE_4E51_943D_3F7F41BDB8F9__INCLUDED_)
#define AFX_MD5WORK_H__6DA752FD_1CCE_4E51_943D_3F7F41BDB8F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD SetMD5Filter(DWORD* ppara);
DWORD WINAPI SetMD5FilterThread(LPVOID lp);

DWORD OwnMD5Filter();
DWORD WINAPI OwnMD5FilterThread(LPVOID lp);

class MD5Work
{
public:
	MD5Work();
	virtual ~MD5Work();
public:
	static MD5Work* GetInstance();
	static void DestroyInstance();
	void Init();
	BOOL WriteMD5Buf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);
	BOOL SetPara(const char* timestr, const char * lengthstr, const char * typestr);
	BOOL DealMD5Data();
private:
	BOOL MD5_GetPathStruct(WCHAR * p);
private:
	static MD5Work* m_pInstance;
	char * SendBuf;
	DWORD SendCount;
	int offset;
	
	time_t begtime;
	time_t endtime;
	DWORD minfile;
	DWORD maxfile;
	vector<string> TypeArr;
};

#endif // !defined(AFX_MD5WORK_H__6DA752FD_1CCE_4E51_943D_3F7F41BDB8F9__INCLUDED_)
