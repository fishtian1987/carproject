// Screen.cpp: implementation of the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "tdialog.h"
#include "Screen.h"
#include "NdisSocket.h"
#include "Struct.h"
#include "WinApi.h"
#include "getSysteminfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern long		v_netSpeed;
extern long		v_minSpeed;
extern long		v_maxSpeed;

CScreen g_work;

CScreen::CScreen()
{
	m_width = 0;
	m_height = 0;
	m_savewidth = 1280;
	m_saveheight = 800;
	m_screen = NULL;
	m_temp = NULL;
	m_block = NULL;
	m_blocklen = 0;
	m_len = 0;

	nMinChangeNum = 0;
	nMaxChangeNum = 0;

	m_BitPerPixel = 16;
	m_linelen = 0;
	m_pixellen = 0;
	m_work=FALSE;
	m_Thread=NULL;
	m_lock = FALSE;
}

CScreen::~CScreen()
{
	if(m_screen)
	{
		delete[] m_screen;
		m_screen = NULL;
	}
	if(m_temp)
	{
		delete[] m_temp;
		m_temp = NULL;
	}
	if(m_block)
	{
		delete[] m_block;
		m_block = NULL;
	}
}

// BOOL CScreen::start(DWORD& width, DWORD& height, DWORD& BitPerPixel, SCREENCALLBACK callback, char * pname)
// {
// 	Lock();
// 	if(GetWorkState()==FALSE)
// 	{
// 		int precentagedpi  = 96;
// 
// 		if (getSysteminfo::GetInstance()->GetSystemType() == VER_Windows7  ||
// 			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows8  ||
// 			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows81 ||
// 			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows10)
// 		{
// 			DWORD dwType, dwVal=0;
// 			DWORD dwSize=sizeof(DWORD);
// 			if(PSHGetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", "AppliedDPI",
// 				&dwType, &dwVal, &dwSize)==ERROR_SUCCESS )
// 			{
// 				precentagedpi = dwVal;
// 			}
// 		}
// 		else
// 		{
// 			precentagedpi = 96;
// 		}
// 
// 		HDC ScreenDc = PGetDC(NULL);   
// 		m_width = PGetDeviceCaps(ScreenDc,HORZRES)*precentagedpi/96;
// 		m_height = PGetDeviceCaps(ScreenDc,VERTRES)*precentagedpi/96;
// 
//  //		m_savewidth = ((m_width/5*4)/BLOCK_WIDTH)*BLOCK_WIDTH;
//  //		m_saveheight = ((m_height/5*4)/BLOCK_HEIGHT)*BLOCK_HEIGHT;
// 
// 		PReleaseDC(NULL,ScreenDc) ;	
// 		ScreenDc = NULL;
// 
// 		m_len = ((m_savewidth*m_BitPerPixel+31)/32)*4*m_saveheight;
// 		m_screen = new BYTE[m_len];
// 		m_temp = new BYTE[m_len];
// 		memset(m_screen, 0, m_len);
// 		memset(m_temp, 0, m_len);
// 
// 		m_linelen = m_BitPerPixel*m_savewidth/8;
// 		m_pixellen = m_BitPerPixel/8;
// 		m_blocklen = m_BitPerPixel*BLOCK_WIDTH*BLOCK_HEIGHT/8;
// 		m_block = new BYTE[m_blocklen];
// 
// 		update = callback;
// 		memset(m_processname, 0, MAX_PATH);
// 		strcpy(m_processname, pname);
// 
// 		SetWorkState(TRUE);
// 
// 		if(!m_Thread)
// 		{
// 			DWORD TID;
// 			m_Thread = CreateThread(NULL, 0, WorkThread, this, 0, &TID);
// 		}
//  	}
// 
// 	BitPerPixel = m_BitPerPixel;
// 	width = m_savewidth;
// 	height = m_saveheight;
// 
// 	Unlock();
// 	
// 	return TRUE;
// }

BOOL CScreen::init()
{
	if(GetWorkState()==FALSE)
	{
		int precentagedpi  = 96;
		
		if (getSysteminfo::GetInstance()->GetSystemType() == VER_Windows7  ||
			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows8  ||
			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows81 ||
			getSysteminfo::GetInstance()->GetSystemType() == VER_Windows10)
		{
			DWORD dwType, dwVal=0;
			DWORD dwSize=sizeof(DWORD);
			if(PSHGetValueA(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", "AppliedDPI",
				&dwType, &dwVal, &dwSize)==ERROR_SUCCESS )
			{
				precentagedpi = dwVal;
			}
		}
		else
		{
			precentagedpi = 96;
		}
		
		HDC ScreenDc = PGetDC(NULL);   
		m_width = PGetDeviceCaps(ScreenDc,HORZRES)*precentagedpi/96;
		m_height = PGetDeviceCaps(ScreenDc,VERTRES)*precentagedpi/96;
		
		//		m_savewidth = ((m_width/5*4)/BLOCK_WIDTH)*BLOCK_WIDTH;
		//		m_saveheight = ((m_height/5*4)/BLOCK_HEIGHT)*BLOCK_HEIGHT;
		
		PReleaseDC(NULL,ScreenDc) ;	
		ScreenDc = NULL;
		
		m_len = ((m_savewidth*m_BitPerPixel+31)/32)*4*m_saveheight;
		m_screen = new BYTE[m_len];
		m_temp = new BYTE[m_len];
		memset(m_screen, 0, m_len);
		memset(m_temp, 0, m_len);
		
		m_linelen = m_BitPerPixel*m_savewidth/8;
		m_pixellen = m_BitPerPixel/8;
		m_blocklen = m_BitPerPixel*BLOCK_WIDTH*BLOCK_HEIGHT/8;
		m_block = new BYTE[m_blocklen];
		
		SetWorkState(TRUE);
		
		if(!m_Thread)
		{
			DWORD TID;
			m_Thread = CreateThread(NULL, 0, WorkThread, this, 0, &TID);
		}
	}

	return TRUE;
}

BOOL CScreen::start(DWORD& width, DWORD& height, DWORD& BitPerPixel, SCREENCALLBACK callback, char * pname)
{
	if (m_len != 0)
	{
		memset(m_screen, 0, m_len);

		update = callback;
		memset(m_processname, 0, MAX_PATH);
		strcpy(m_processname, pname);
			
		BitPerPixel = m_BitPerPixel;
		width = m_savewidth;
		height = m_saveheight;
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CScreen::getfull()
{
	if (m_len != 0)
	{
		memset(m_screen, 0, m_len);
		return 1;
	}
	else
	{
		return 0;
	}

}

// BOOL CScreen::stop()
// {
// 	Lock();
// 	
// 	SetWorkState(FALSE);
// 	PSleep(2000);
// 	
// 	m_width = 0;
// 	m_height = 0;
// 	if(m_screen)
// 	{
// 		delete[] m_screen;
// 		m_screen = NULL;
// 	}
// 	if(m_temp)
// 	{
// 		delete[] m_temp;
// 		m_temp = NULL;
// 	}
// 	if(m_block)
// 	{
// 		delete[] m_block;
// 		m_block = NULL;
// 	}
// 	m_len = 0;
// 	
// 	update = NULL;
// 	
// 	Unlock();
// 	return FALSE;
// }

BOOL CScreen::stop()
{
	Lock();
	
	update = NULL;
	
	Unlock();
	return FALSE;
}

int CScreen::GetBitMapBuffer(HDC ScreenDc, HBITMAP hBitmap, int wBitCount, BYTE* buffer, int buflen)
{
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD           dwPaletteSize=0,dwBmBitsSize;
	BITMAP          Bitmap;               
	//位图文件头结构
	BITMAPINFOHEADER   bi;                    
	//指向位图信息头结构
    HANDLE          hPal;
	HPALETTE     hOldPal=NULL;
	
	
	//计算调色板大小
// 	if (wBitCount <= 8)
// 		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);
	
	//设置位图信息头结构
	PGetObjectA(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;
	
	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;

	if((DWORD)buflen<dwBmBitsSize)
		return 0;
	
	// 处理调色板   
	hPal = PGetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hOldPal=PSelectPalette(ScreenDc,(HPALETTE)hPal,FALSE);
		PRealizePalette(ScreenDc);
	}
	// 获取该调色板下新的像素值
	PGetDIBits(ScreenDc,hBitmap,0,(UINT)Bitmap.bmHeight,buffer, (BITMAPINFO *)&bi,DIB_RGB_COLORS);
	//恢复调色板   
	if (hOldPal)
	{
		PSelectPalette(ScreenDc, hOldPal, TRUE);
		PRealizePalette(ScreenDc);
	}
	
	//清除   
	
	return dwBmBitsSize;
}

void CScreen::GetScreen()
{
	if(!m_work)
		return;

	HDC ScreenDc = PGetDC(NULL);
	HDC MemDC = PCreateCompatibleDC(ScreenDc);    
	HBITMAP bitmap = PCreateCompatibleBitmap(ScreenDc, m_savewidth, m_saveheight);
	PSelectObject(MemDC, bitmap);
	if(PStretchBlt(MemDC, 0, 0, m_savewidth, m_saveheight, ScreenDc, 0, 0, m_width, m_height, SRCCOPY|CAPTUREBLT))
	{
		int len = GetBitMapBuffer(ScreenDc, bitmap, 16, m_temp, m_len);

		if(len==m_len)
		{
			CheckUpdate();
		}
	}

	PDeleteObject(bitmap);			
	PDeleteDC(MemDC);	
	PReleaseDC(NULL,ScreenDc) ;	
	
	ScreenDc = NULL;
	MemDC = NULL;
	bitmap = NULL;

	return;
}

void CScreen::CheckUpdate()
{
	if(!m_work)
		return;

	DWORD totaltm = 0;
	DWORD totalcount=0;
	DWORD sendcount=0;

	DWORD bMustSendCount=0;
	static DWORD bMustSendheight=0;
	static DWORD bMustSendWidth=0;
	int nUpNum = 0;

	static BYTE* sendbuf = new BYTE[MAX_SEND_BUF];
	int sendlen = 0;
	
	BOOL bUpdate=FALSE;
	for(DWORD m=0; m<m_saveheight; m+=BLOCK_HEIGHT)
	{
		for(DWORD n=0; n<m_savewidth; )
		{
			for (DWORD m1=m; (m1<m+BLOCK_HEIGHT && m1<m_saveheight); m1++)
			{
				if(!m_work)
					return;
				//判断每个像素点是否有变化
				if(memcmp(m_screen+m1*m_linelen+n*m_pixellen, m_temp+m1*m_linelen+n*m_pixellen, m_pixellen)!=0)
				{
					bUpdate=TRUE;
					nUpNum++;
					break;
				}
			}

			//每次默认刷新三小块
			if(bMustSendheight==m && n>=bMustSendWidth)
			{
				if(bMustSendCount<=2)
				{
					bMustSendWidth = n + BLOCK_WIDTH;
					if (m_savewidth <=bMustSendWidth)
					{
						bMustSendWidth=0;
						bMustSendheight+=BLOCK_HEIGHT;
						if(bMustSendheight>=m_saveheight)
							bMustSendheight = 0;
					}
					bUpdate = TRUE;
					bMustSendCount++;
				}
			}

			//屏幕发生了变化
			if(bUpdate)
			{
				bUpdate=FALSE;
				DWORD tempwidth, tempheight;
				if(n+BLOCK_WIDTH<=m_savewidth && m+BLOCK_HEIGHT<=m_saveheight)
				{
					tempwidth = BLOCK_WIDTH;
					tempheight = BLOCK_HEIGHT;
				}
				else if(m+BLOCK_HEIGHT<=m_saveheight)
				{
					tempwidth = m_savewidth-n;
					tempheight = BLOCK_HEIGHT;
				}
				else if(n+BLOCK_WIDTH<=m_savewidth)
				{
					tempwidth = BLOCK_WIDTH;
					tempheight = m_saveheight-m;
				}
				else
				{
					tempwidth = m_savewidth-n;
					tempheight = m_saveheight-m;
				}

				DWORD bgtm = PGetTickCount();

				DWORD blocklen = m_blocklen;
				GetCompressBlockBuf(m, n, tempwidth, tempheight, m_block, blocklen);


				totaltm += (PGetTickCount()-bgtm);
				totalcount += 1;

				if(sendlen+blocklen+sizeof(BLOCKHEADER)>MAX_SEND_BUF)
				{
					sendcount++;

					if(update)
						update(sendbuf, sendlen);

					sendlen = 0;
				}

				if(blocklen<=MAX_SEND_BUF-sizeof(BLOCKHEADER))
				{
					BLOCKHEADER header;
					header.top = m;
					header.left = n;
					header.width = tempwidth;
					header.height = tempheight;
					header.len = blocklen;

					memcpy(sendbuf+sendlen, &header, sizeof(BLOCKHEADER));
					sendlen+=sizeof(BLOCKHEADER);
					memcpy(sendbuf+sendlen, m_block, blocklen);
					sendlen+=blocklen;
				}

				n+=tempwidth;
			}
			else
			{
				n++;
			}
		}
	}

	if(sendlen>0)
	{
		sendcount++;

		if(update)
			update(sendbuf, sendlen);
		
		sendlen = 0;
	}

	//每次只改变不超过两个数据块时，认为改变较小
	if (nUpNum <= 2)
	{
		nMinChangeNum++;
		nMaxChangeNum = 0;

		//连续30帧都是小改变时，认为电脑无操作
		if (nMinChangeNum > 30 && v_netSpeed == v_minSpeed)
		{
			v_netSpeed = v_maxSpeed;
		}
	}
	else
	{
		nMaxChangeNum++;
		nMinChangeNum = 0;

		//连续3帧都是大改变时，认为电脑有操作
		if (nMaxChangeNum > 3 && v_netSpeed == v_maxSpeed)
		{
			v_netSpeed = v_minSpeed;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//压缩结构  像素1 数量1 像素2 数量2
//  其中像素用2个字节表示（0x8000位标识是否存在数量，1表示不存在， 0表示存在），
//  数量用1个字节表示
//////////////////////////////////////////////////////////////////////////

UCHAR ConvertPix(USHORT pix)
{
	UCHAR b = (pix & 0x1F);
	UCHAR g = (pix>>5) & 0x1F;
	UCHAR r = (pix>>10) & 0x1F;

	b >>= 3;
	r >>= 3;
	g >>= 3;

	UCHAR ret = (r<<4)+(g<<2)+b;

	return ret;
}

void CScreen::GetCompressBlockBuf(DWORD top, DWORD left, DWORD width, DWORD height, BYTE* pbuf, DWORD& len)
{
	if(!m_work)
		return;

	UCHAR prepix=0;
	UCHAR precount = 0;
	DWORD templen=0;
	DWORD templinelen = width*m_pixellen;

	for(DWORD m=0; m<height; m++)
	{
		if(len-templen>=templinelen)
		{
			USHORT* pline = (USHORT*)(m_temp+(top+m)*m_linelen+left*m_pixellen);

			for(DWORD n=0; n<width; n++)
			{
				if(precount>=0xFF)
				{
					memcpy(pbuf+templen, &prepix, 1);
					templen+=1;

					pbuf[templen]=0xFF;
					templen+=1;
					
					precount=0;
				}

				BYTE currentPix=ConvertPix(*pline);

				if(prepix==currentPix)
					precount++;
				else 
				{
					if(precount)
					{
						if(precount==1)
							prepix |= 0x80;

						memcpy(pbuf+templen, &prepix, 1);
						templen+=1;

						if(precount>1)
						{
							pbuf[templen]=precount;
							templen+=1;
						}
					}

					prepix = currentPix;

					precount=1;
				}

				pline++;
			}

		//	memcpy(pbuf+templen, m_temp+(top+m)*m_linelen+left*m_pixellen, templinelen);
			
			//更新基础库
			memcpy(m_screen+(top+m)*m_linelen+left*m_pixellen, m_temp+(top+m)*m_linelen+left*m_pixellen, templinelen);
		}
		else
		{
			break;
		}
	}

	if(precount)
	{
		if(precount==1)
			prepix |= 0x80;
			
		memcpy(pbuf+templen, &prepix, 1);
		templen+=1;
		
		if(precount>1)
		{
			pbuf[templen]=precount;
			templen+=1;
		}
	}

	len = templen;
	return;
}

void CScreen::SetWorkState(BOOL bwork)
{
	m_work = bwork;
}

BOOL CScreen::GetWorkState()
{
	return m_work;
}

void CScreen::Lock()
{
	while (m_lock)
	{
		PSleep(10);
	}
	m_lock = TRUE;
}

void CScreen::Unlock()
{
	m_lock = FALSE;
}

DWORD GetPidFormName(char * pname)
{
// 	HANDLE hSnapshot = PCreateToolhelp32Snapshot(
// 		TH32CS_SNAPPROCESS,
// 		0);
// 	struct tagPROCESSENTRY32W pe;
// 	::ZeroMemory(&pe, sizeof(pe));
// 	pe.dwSize = sizeof(pe);
// 
// 	BOOL bMore = PProcess32FirstW(hSnapshot, &pe);
// 	
// 	for(; bMore; bMore = PProcess32NextW(hSnapshot, &pe))
// 	{
// 		_wcslwr(pe.szExeFile);
// 		BOOL re=FALSE;
// 		wchar_t procexefile[260];
// 		ZeroMemory(procexefile, sizeof(procexefile));
// 		if(wcslen(pe.szExeFile) >= 260)
// 			continue;
// 		wcscpy(procexefile, pe.szExeFile);
// 		wchar_t * pfile = procexefile;
// 		while(1){
// 			wchar_t *pTmpfile = 0;
// 			if((pTmpfile = wcsstr(pfile, L"\\")) == 0){
// 				break;
// 			}
// 			else{
// 				pfile = pTmpfile+1;
// 				continue;
// 			}
// 		}
// 		
// 		if(wcsstr(pfile, pname) > 0)
// 		{
// 			return pe.th32ProcessID;
// 		}
// 	}// end while
	return 0;
}

DWORD WINAPI WorkThread(LPVOID p)
{
	DWORD begtm, tmlen;

	while(TRUE)
	{
		begtm = PGetTickCount();

		if(g_work.GetWorkState())
			g_work.GetScreen();
		tmlen = PGetTickCount()-begtm;

		//1帧 /s
		if(tmlen<1000)
		{
			PSleep(1000-tmlen);
		}

//		char log[100];
//		sprintf(log, "tm:%d", tmlen);
//		OutputDebugStringA(log);
	}

	return 0;
}

void ScreenUpdate(BYTE* pbuf, DWORD len)
{
	static char* tmpbuf = new char[10*1024];
	int buflen = sprintf(tmpbuf, 
		"window-title: %s\r\n"
		"content-length: %d\r\n"
		"content-type: monitor\r\n\r\n",
		GetUtf8WindowTitle(), len);
	
	if(pbuf)
	{
		memcpy(tmpbuf+buflen, pbuf, len);
		buflen+= len;
	}
	
	SendData(tmpbuf, buflen, PTYPE_SCREEN_MULUPDATE2, 170);

	return;
}

DWORD StartScreen(DWORD* pPara)
{
	ThreadParam *ppara=(ThreadParam*)pPara;
	char *field=0;
	char *value=0;
	char *nextline=0;
	char *inputline=ppara->buf;
	
	char  strPname[MAX_PATH];
	memset(strPname, 0, sizeof(strPname));
	
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re){
		if(stricmp(field, "pname")==0)
		{
			strcpy(strPname, value);
		}
		
		if(re==2)
			break;
		inputline=nextline;
		re=LineParser(inputline,&nextline,&field,&value);
	}

	DWORD width, height, BitPerPixel;
	g_work.start(width, height, BitPerPixel, ScreenUpdate, strPname);
	
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);
	
	char tmpbuf[1024];
	int len = sprintf(tmpbuf, "content-length: 0\r\n"
		"content-type: monitor\r\n"
		"window-title: %s\r\n"
		"width: %d\r\n"
		"height: %d\r\n"
		"bitperpixel: %d\r\n\r\n",
		GetUtf8WindowTitle(), width, height, BitPerPixel);
	
	SendData(tmpbuf, len, PTYPE_SCREEN_START, 170);

	return 0;
}

DWORD StopScreen(DWORD* pPara)
{
	ThreadParam *para = (ThreadParam *)pPara;
	g_work.stop();
	
	char tmpbuf[1024];
	int len = sprintf(tmpbuf, "content-length: 2\r\n"
		"content-type: monitor\r\n\r\nok");
	
	SendData(tmpbuf, len, para->PacketType, para->SessionID);
	
	return 0;
}

DWORD GetFull(DWORD* pPara)
{
	g_work.getfull();
	return 0;
}

