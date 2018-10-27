/*
*********************************************************************************************************
*	                                  
*	模块名称 : 文件管理模块
*	文件名称 : exfuns.c
*	版    本 : V1.3
*	说    明 : 文件管理模块
*
*
*********************************************************************************************************
*/

#include "string.h"
#include "exfuns.h"
#include "FreeRTOS.h"

const u8 *FILE_TYPE_TBL[6][13]=
{
{"BIN"},			
{"LRC"},		
{"NES"},			
{"TXT","C","H"},	
{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","ACC","WMA","WAV","MID","FLAC"},
{"BMP","JPG","JPEG","GIF"},
};

FATFS *fs[2];  		
FIL *file;	  	
FIL *ftemp;	  	
UINT br,bw;		
FILINFO fileinfo;	
DIR dir;  
u8 *fatbuf;			

/*
*********************************************************************************************************
*	函 数 名: exfuns_init
*	功能说明: 为exfuns申请内存
*	形    参：无
*	返 回 值: 0,成功
						1,失败
*********************************************************************************************************
*/
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)pvPortMalloc(sizeof(FATFS));//mymalloc(SRAMIN,sizeof(FATFS));	
	fs[1]=(FATFS*)pvPortMalloc(sizeof(FATFS));//mymalloc(SRAMIN,sizeof(FATFS));	
	file=(FIL*)pvPortMalloc(sizeof(FIL));//mymalloc(SRAMIN,sizeof(FIL));
	ftemp=(FIL*)pvPortMalloc(sizeof(FIL));//mymalloc(SRAMIN,sizeof(FIL));	
	fatbuf=(u8*)pvPortMalloc(512);//mymalloc(SRAMIN,512);
	if(fs[0]&&fs[1]&&file&&ftemp&&fatbuf)return 0; 
	else return 1;	
}

/*
*********************************************************************************************************
*	函 数 名: char_upper
*	功能说明: 将小写字母转为大写字母,如果是数字,则保持不变.
*	形    参：无
*	返 回 值: 结果
*********************************************************************************************************
*/
u8 char_upper(u8 c)
{
	if(c<'A')return c;
	if(c>='a')return c-0x20;
	else return c;
}	      

/*
*********************************************************************************************************
*	函 数 名: f_typetell
*	功能说明: 报告文件的类型
*	形    参：fname:文件名
*	返 回 值: 0XFF,表示无法识别的文件类型编号.
       		 其他,高四位表示所属大类,低四位表示所属小类.
*********************************************************************************************************
*/
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;
		fname++;
	}
	if(i==250)return 0XFF;
 	for(i=0;i<5;i++)
	{
		fname--;
		if(*fname=='.'){fname++;attr=fname;break;}
  }
	strcpy((char *)tbuf,(const char*)attr);
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);
	for(i=0;i<6;i++)
	{
		for(j=0;j<13;j++)
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0){return (i<<4)|j;}
		}
	}
	return 0XFF;	   
}	 

/*
*********************************************************************************************************
*	函 数 名: exf_getfree
*	功能说明: 得到磁盘剩余容量
*	形    参：drv:磁盘编号("0:"/"1:")
						total:总容量	 （单位KB）
						free:剩余容量	 （单位KB）
*	返 回 值: 0,正常
						其他,错误代码
*********************************************************************************************************
*/
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
  u32 fre_clust=0, fre_sect=0, tot_sect=0;
  res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
  if(res==0)
	{											   
	  tot_sect=(fs1->n_fatent-2)*fs1->csize;	
	  fre_sect=fre_clust*fs1->csize;	   
#if _MAX_SS!=512				  
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	
		*free=fre_sect>>1;
 	}
	return res;
}		   
