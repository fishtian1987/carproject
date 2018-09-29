// UpLoadFile.h: interface for the UpLoadFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPLOADFILE_H__4338B68D_C85D_40B0_97EA_B7E4FCA657AD__INCLUDED_)
#define AFX_UPLOADFILE_H__4338B68D_C85D_40B0_97EA_B7E4FCA657AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Struct.h"
class UDPPacket
{
public:
	USHORT			PacketSeq;	//
	unsigned int	Slen;		//	
	char			buf[2000];
};

bool  __stdcall operator<(const UDPPacket& s1, const UDPPacket& s2);
bool  __stdcall operator>(const UDPPacket& s1, const UDPPacket& s2);
// Define a template class list of int
typedef list<UDPPacket, allocator<UDPPacket> > UdpPacketList ;
//Define an iterator for template class list of ints
typedef UdpPacketList::iterator UDPPacketListIt ;

class UploadFile{
public:
	CHAR		filename[500];
	USHORT		sessionid;
	FILETIME	filetime;
	LONG		filesize;
	time_t		tm;
	USHORT		nextseq;
	UdpPacketList udppacketlst;
	UploadFile(){
		memset(filename,0,500);
		sessionid=0;
		memset(&filetime,0,sizeof(filetime));
		filesize=0;
		nextseq=0;
		tm=0;
		udppacketlst.clear();
	}
	int UploadFile1(){
		memset(filename,0,500);
		sessionid=0;
		memset(&filetime,0,sizeof(filetime));
		filesize=0;
		nextseq=0;
		tm=0;
		udppacketlst.clear();
		return 0;
	}
};

class CSetUploadFile
{
public:
	CSetUploadFile();
	~CSetUploadFile();

	static CSetUploadFile* GetInstance();
	static void DestroyInstance();

private:
	static CSetUploadFile* m_pInstance;

	struct  
	{
		WCHAR		szFilePath[500];
		char		szFileMD5[33];
		USHORT		sessionid;
		FILETIME	tmFileTime;
		__int64		nFileSize;
		__int64		nFileOffset;
		bool		bState;
	};

};

/*!
  \fn int		WriteUploadFile(UploadFile & upfile);
  \brief 写上传文件
  \param 
  \param 
  \return
  函数详细
*/
int		WriteUploadFile(UploadFile & upfile);
/*!
  \fn DWORD	UploadFileFun(ThreadParam *pThreadPara,MumaHead	*pMumaHead);
  \brief 上传文件
  \param 
  \param 
  \return
  函数详细
*/
DWORD	UploadFileFun(ThreadParam *pThreadPara);
/*!
  \fn void	CloseUploadFile();
  \brief 关闭文件
  \param 
  \param 
  \return
  函数详细
*/
void	CloseUploadFile();

DWORD SetUpFileFun(ThreadParam *pThreadPara);

#endif // !defined(AFX_UPLOADFILE_H__4338B68D_C85D_40B0_97EA_B7E4FCA657AD__INCLUDED_)
