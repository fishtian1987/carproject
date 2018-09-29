// getSysteminfo.h: interface for the getSysteminfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETSYSTEMINFO_H__133186F5_1124_401C_9321_B1A76B493611__INCLUDED_)
#define AFX_GETSYSTEMINFO_H__133186F5_1124_401C_9321_B1A76B493611__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		VER_WIN98	0
#define		VER_WIN2K	1
#define		VER_WINXP	2
#define		VER_WIN2K3	3
#define     VER_Vista32 4
#define     VER_Vista64 5
#define		VER_Windows7 6
#define		VER_Windows8 7
#define		VER_Windows81 8
#define		VER_Windows10 9
#define		VER_WindowsXX 99

class getSysteminfo  
{
public:
	getSysteminfo();
	virtual ~getSysteminfo();
public:
	static getSysteminfo* GetInstance();
	static void DestroyInstance();

	bool InitInfo();
	int GetHeartInfo(unsigned char *pBuf);
	int GetSystemType() { return m_SystemType; }

private:

	BOOL GetComputerNm();
	BOOL GetSystemVersion();
	BOOL GetCPU();
	BOOL GetInternetInfo();
	BOOL GetNetworkIp();
	BOOL GetUserInfo();
	BOOL GetTimeZone();
	
	string m_strComputerName;
	string m_strSystemVersion;
	int m_SystemType;
	string m_strCPU;
	string m_strUser;
	string m_strTimeZone;
	string m_strInternetInfo;
	string m_strNetworkIp;
	
	char * SysBuf;

	static getSysteminfo* m_pInstance;
};


/*!
  \fn int GetSystemType(char* buf);
  \brief //获得系统代码
  \param 
  \param 
  \return
  函数详细
*/
//int GetSystemType(WCHAR* buf);

#endif // !defined(AFX_GETSYSTEMINFO_H__133186F5_1124_401C_9321_B1A76B493611__INCLUDED_)
