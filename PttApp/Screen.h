// Screen.h: interface for the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__FCB7AC5B_6F76_471A_A8CD_8316B2859217__INCLUDED_)
#define AFX_SCREEN_H__FCB7AC5B_6F76_471A_A8CD_8316B2859217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void (* SCREENCALLBACK)(BYTE* pbuf, DWORD len);

#define MAX_SEND_BUF	1100
#define BLOCK_WIDTH		60
#define BLOCK_HEIGHT	18

#pragma pack(push)
#pragma pack(1)

struct BLOCKHEADER{
	unsigned short magic;
	unsigned short top;
	unsigned short left;
	unsigned short width;
	unsigned short height;
	unsigned short len;
	
	BLOCKHEADER()
	{
		magic = 0xAAAA;
	}
};

#pragma pack(pop)

class CScreen  
{
public:
	CScreen();
	virtual ~CScreen();
	
public:
	BOOL start(DWORD& width, DWORD& height, DWORD& BitPerPixel, SCREENCALLBACK callback, char * pname);
	int getfull();
	BOOL stop(); 
	BOOL GetWorkState();		
	void GetScreen();			//获取屏幕数据
	BOOL init();
	
private:
	void CheckUpdate();	//检查屏幕是否发生了改变
	int GetBitMapBuffer(HDC ScreenDc, HBITMAP hBitmap, int wBitCount, BYTE* buffer, int buflen);
	void GetCompressBlockBuf(DWORD top, DWORD left, DWORD width, DWORD height, BYTE* pbuf, DWORD& len);
	SCREENCALLBACK update;	//回调函数，用于向调用者发送更新的数据
	
private:
	DWORD m_width;		//屏幕宽度
	DWORD m_height;		//屏幕高度
	DWORD m_savewidth;	//保存的宽度
	DWORD m_saveheight; //保存的高度
	BYTE* m_screen;		//屏幕基础数据
	BYTE* m_temp;		//屏幕临时数据
	DWORD m_len;		//数据长度
	
	BYTE* m_block;		//用于存储更新的内存块
	DWORD m_blocklen;	//块的大小，不要更改
	
	HANDLE m_Thread;
	
private:
	int m_BitPerPixel;	//图像质量
	int m_linelen;		//一行像素的字节数
	int m_pixellen;		//一个像素的字节数

	char m_processname[MAX_PATH]; //进程名称
	int nMinChangeNum;	//小幅（一次只改变不超过两个数据块）改变的帧数
	int nMaxChangeNum;	//大幅改变的帧数
	
private:
	void SetWorkState(BOOL bwork);
	BOOL m_work;
	
private:
	BOOL m_lock;
	void Lock();
	void Unlock();
	
};

DWORD WINAPI WorkThread(LPVOID);

extern CScreen g_work;

DWORD StartScreen(DWORD* pPara);
DWORD StopScreen(DWORD* pPara);
DWORD GetFull(DWORD* pPara);

#endif // !defined(AFX_SCREEN_H__FCB7AC5B_6F76_471A_A8CD_8316B2859217__INCLUDED_)
