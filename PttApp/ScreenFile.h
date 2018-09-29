// ScreenFile.h: interface for the ScreenFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENFILE_H__49B8DF66_0AD3_48D3_93D6_7C04534F711D__INCLUDED_)
#define AFX_SCREENFILE_H__49B8DF66_0AD3_48D3_93D6_7C04534F711D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _JPGBUFFER_
{
	char*	pbuffer;
	int		pos;
	int		buflen;
	_JPGBUFFER_() {pbuffer=NULL; pos=0; buflen=0;}
}SJpgBuffer;

extern long g_taskallow; // 互斥

extern BOOL getallow(long &hashallow, DWORD a);
extern BOOL releaseallow(long &hashallow, DWORD a);

/*! \fn class ScreenFile 
  \brief 截屏以文件方式发送
  \param 
  \param  
  \return
  函数详细
*/
class ScreenFile  
{
public:
	/*! \fn BOOL CreateJpeg(char*buf ,int buflen);
	  \brief 创建一幅图片
	  \param 
	  \param  
	  \return
	  函数详细
	*/
	BOOL CreateJpeg(char*buf ,int buflen,BOOL isscreen);
public:
	ScreenFile();
	virtual ~ScreenFile();
private:
	/*! \fn BOOL CreateFileName();
	  \brief 产生文件名
	  \param 
	  \param  
	  \return
	  函数详细
	*/
	BOOL CreateFileName(const char * fixScreenDir);
	BOOL WriteScreen(BOOL isscreen);
public:
	char m_filename[MAX_PATH];//临时文件名
	char m_dstfilename[MAX_PATH];
	HANDLE m_hFile; //文件句柄
};


extern ScreenFile  g_ScreenFile;

#endif // !defined(AFX_SCREENFILE_H__49B8DF66_0AD3_48D3_93D6_7C04534F711D__INCLUDED_)
