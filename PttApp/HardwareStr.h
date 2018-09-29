// HardwareStr.h: interface for the HardwareStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HARDWARESTR_H__7B77DD3A_1993_4236_81F7_65DA49546997__INCLUDED_)
#define AFX_HARDWARESTR_H__7B77DD3A_1993_4236_81F7_65DA49546997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HardStrWrite
{
public:
	HardStrWrite();
	~HardStrWrite();
	
protected:
	BOOL WriteOneData(const char *szHardStr, const char *szType);
	BOOL WriteEnd();
	
private:
	BOOL WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);

	char * SendBuf;
	int SendCount;
	int offset;
};

class HardStrFun : public HardStrWrite
{
public:
	HardStrFun();
	~HardStrFun();
	
	BOOL Deal();
	
private:

};

DWORD ThreadGetHardStrStart(char * pPara);

#endif // !defined(AFX_HARDWARESTR_H__7B77DD3A_1993_4236_81F7_65DA49546997__INCLUDED_)
