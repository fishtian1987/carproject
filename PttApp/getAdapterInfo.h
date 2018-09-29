// getAdapterInfo.h: interface for the getAdapterInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETADAPTERINFO_H__01E788FA_A5AF_41D9_9D12_8DCEEE7954B9__INCLUDED_)
#define AFX_GETADAPTERINFO_H__01E788FA_A5AF_41D9_9D12_8DCEEE7954B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WriteAdapter  
{
public:
	WriteAdapter();
	virtual ~WriteAdapter();
protected:
	BOOL WriteOneData(ULONG lCardIP, const char *szCardMAC, const char *szCardName, const char *szDesc, ULONG lGateIP, const char *szGateMAC);
	BOOL WriteEnd();
	
private:
	BOOL WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);
	
	char * SendBuf;
	int SendCount;
	int offset;
};

class GetAdapterInfo : public WriteAdapter
{
public:
	GetAdapterInfo();
	virtual ~GetAdapterInfo();
	
	BOOL GetData();
	
private:
	BOOL GetMac(DWORD remoteAddr, char * strbuf);
};

DWORD ThreadAdapterInfoStart(char * pPara);

#endif // !defined(AFX_GETADAPTERINFO_H__01E788FA_A5AF_41D9_9D12_8DCEEE7954B9__INCLUDED_)
