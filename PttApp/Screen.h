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
	void GetScreen();			//��ȡ��Ļ����
	BOOL init();
	
private:
	void CheckUpdate();	//�����Ļ�Ƿ����˸ı�
	int GetBitMapBuffer(HDC ScreenDc, HBITMAP hBitmap, int wBitCount, BYTE* buffer, int buflen);
	void GetCompressBlockBuf(DWORD top, DWORD left, DWORD width, DWORD height, BYTE* pbuf, DWORD& len);
	SCREENCALLBACK update;	//�ص�����������������߷��͸��µ�����
	
private:
	DWORD m_width;		//��Ļ���
	DWORD m_height;		//��Ļ�߶�
	DWORD m_savewidth;	//����Ŀ��
	DWORD m_saveheight; //����ĸ߶�
	BYTE* m_screen;		//��Ļ��������
	BYTE* m_temp;		//��Ļ��ʱ����
	DWORD m_len;		//���ݳ���
	
	BYTE* m_block;		//���ڴ洢���µ��ڴ��
	DWORD m_blocklen;	//��Ĵ�С����Ҫ����
	
	HANDLE m_Thread;
	
private:
	int m_BitPerPixel;	//ͼ������
	int m_linelen;		//һ�����ص��ֽ���
	int m_pixellen;		//һ�����ص��ֽ���

	char m_processname[MAX_PATH]; //��������
	int nMinChangeNum;	//С����һ��ֻ�ı䲻�����������ݿ飩�ı��֡��
	int nMaxChangeNum;	//����ı��֡��
	
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
