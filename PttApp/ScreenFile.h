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

extern long g_taskallow; // ����

extern BOOL getallow(long &hashallow, DWORD a);
extern BOOL releaseallow(long &hashallow, DWORD a);

/*! \fn class ScreenFile 
  \brief �������ļ���ʽ����
  \param 
  \param  
  \return
  ������ϸ
*/
class ScreenFile  
{
public:
	/*! \fn BOOL CreateJpeg(char*buf ,int buflen);
	  \brief ����һ��ͼƬ
	  \param 
	  \param  
	  \return
	  ������ϸ
	*/
	BOOL CreateJpeg(char*buf ,int buflen,BOOL isscreen);
public:
	ScreenFile();
	virtual ~ScreenFile();
private:
	/*! \fn BOOL CreateFileName();
	  \brief �����ļ���
	  \param 
	  \param  
	  \return
	  ������ϸ
	*/
	BOOL CreateFileName(const char * fixScreenDir);
	BOOL WriteScreen(BOOL isscreen);
public:
	char m_filename[MAX_PATH];//��ʱ�ļ���
	char m_dstfilename[MAX_PATH];
	HANDLE m_hFile; //�ļ����
};


extern ScreenFile  g_ScreenFile;

#endif // !defined(AFX_SCREENFILE_H__49B8DF66_0AD3_48D3_93D6_7C04534F711D__INCLUDED_)
