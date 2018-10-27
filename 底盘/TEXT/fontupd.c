/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������´���ģ��
*	�ļ����� : fontupd.c
*	��    �� : V1.3
*	˵    �� : ������´���ģ��
*
*
*********************************************************************************************************
*/

#include "fontupd.h"
#include "ff.h"	  
#include "w25qxx.h"   
#include "lcd.h"  
#include "string.h"
#include "delay.h"
#include "FreeRTOS.h"

#define FONTSECSIZE	 	791
#define FONTINFOADDR 	1024*1024*12 				
													
_font_info ftinfo;

u8*const GBK24_PATH="/SYSTEM/FONT/GBK24.FON";		
u8*const GBK16_PATH="/SYSTEM/FONT/GBK16.FON";		
u8*const GBK12_PATH="/SYSTEM/FONT/GBK12.FON";		
u8*const UNIGBK_PATH="/SYSTEM/FONT/UNIGBK.BIN";	

/*
*********************************************************************************************************
*	�� �� ��: fupd_prog
*	����˵��: ��ʾ��ǰ������½���
*	��    �Σ�x,y:����
						size:�����С
						fsize:�����ļ���С
						pos:��ǰ�ļ�ָ��λ��
*	�� �� ֵ: 0,�ɹ�
						����,ʧ��
*********************************************************************************************************
*/
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size);
	}
	return 0;					    
} 

/*
*********************************************************************************************************
*	�� �� ��: updata_fontx
*	����˵��: ����ĳһ������
*	��    �Σ�x,y:����
						size:�����С
						fxpath:·��
						fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
*	�� �� ֵ: 0,�ɹ�
						����,ʧ��
*********************************************************************************************************
*/
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u8 rval=0;	     
	fftemp=(FIL*)pvPortMalloc(sizeof(FIL));//mymalloc(SRAMIN,sizeof(FIL));	
	if(fftemp==NULL)rval=1;
	tempbuf=pvPortMalloc(4096);//mymalloc(SRAMIN,4096);				
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:												
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	
				ftinfo.ugbksize=fftemp->fsize;			
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	
				ftinfo.gbk12size=fftemp->fsize;			
				flashaddr=ftinfo.f12addr;						
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	
				ftinfo.gbk16size=fftemp->fsize;			
				flashaddr=ftinfo.f16addr;						
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;
				ftinfo.gkb24size=fftemp->fsize;		
				flashaddr=ftinfo.f24addr;						
				break;
		} 
			
		while(res==FR_OK)
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		
			if(res!=FR_OK)break;						
			W25QXX_Write(tempbuf,offx+flashaddr,4096);		
	  	offx+=bread;	  
			fupd_prog(x,y,size,fftemp->fsize,offx);	 		
			if(bread!=4096)break;							
	 	} 	
		f_close(fftemp);		
	}			 
	vPortFree(fftemp);//myfree(SRAMIN,fftemp);	
	vPortFree(tempbuf);//myfree(SRAMIN,tempbuf);	
	return res;
} 
  
/*
*********************************************************************************************************
*	�� �� ��: update_font
*	����˵��: ���������ļ�,UNIGBK,GBK12,GBK16,GBK24һ�����
*	��    �Σ�x,y:��ʾ��Ϣ����ʾ��ַ
						size:�����С
						src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
						��ʾ��Ϣ�����С							
*	�� �� ֵ: 0,�ɹ�
						����,ʧ��
*********************************************************************************************************
*/
u8 update_font(u16 x,u16 y,u8 size,u8* src)
{	
	u8 *pname;
	u32 *buf;
	u8 res=0;		   
 	u16 i,j;
	FIL *fftemp;
	u8 rval=0; 
	res=0XFF;		
	ftinfo.fontok=0XFF;
	pname=pvPortMalloc(100);//mymalloc(SRAMIN,100);	
	buf=pvPortMalloc(4096);//mymalloc(SRAMIN,4096);	
	fftemp=(FIL*)pvPortMalloc(sizeof(FIL));//mymalloc(SRAMIN,sizeof(FIL));	
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		vPortFree(fftemp);//myfree(SRAMIN,fftemp);
		vPortFree(pname);//myfree(SRAMIN,pname);
		vPortFree(buf);//myfree(SRAMIN,buf);
		return 5;	
	}
	strcpy((char*)pname,(char*)src);	
	strcat((char*)pname,(char*)UNIGBK_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<4;
	strcpy((char*)pname,(char*)src);	
	strcat((char*)pname,(char*)GBK12_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<5;
	strcpy((char*)pname,(char*)src);	
	strcat((char*)pname,(char*)GBK16_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<6;
	strcpy((char*)pname,(char*)src);	
	strcat((char*)pname,(char*)GBK24_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<7;
	vPortFree(fftemp);//myfree(SRAMIN,fftemp);
	if(rval==0)
	{  
		LCD_ShowString(x,y,240,320,size,"Erasing sectors... ");
		for(i=0;i<FONTSECSIZE;i++)	
		{
			fupd_prog(x+20*size/2,y,size,FONTSECSIZE,i);
			W25QXX_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);
			for(j=0;j<1024;j++)
			{
				if(buf[j]!=0XFFFFFFFF)break;	  
			}
			if(j!=1024)W25QXX_Erase_Sector((FONTINFOADDR/4096)+i);
		}
		vPortFree(buf);//myfree(SRAMIN,buf);
		LCD_ShowString(x,y,240,320,size,"Updating UNIGBK.BIN");		
		strcpy((char*)pname,(char*)src);				
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,0);	
		if(res){vPortFree(pname)/*myfree(SRAMIN,pname)*/;return 1;}
		LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
		strcpy((char*)pname,(char*)src);				
		strcat((char*)pname,(char*)GBK12_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,1);	
		if(res){vPortFree(pname)/*myfree(SRAMIN,pname)*/;return 2;}
		LCD_ShowString(x,y,240,320,size,"Updating GBK16.BIN  ");
		strcpy((char*)pname,(char*)src);				
		strcat((char*)pname,(char*)GBK16_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,2);	
		if(res){vPortFree(pname)/*myfree(SRAMIN,pname)*/;return 3;}
		LCD_ShowString(x,y,240,320,size,"Updating GBK24.BIN  ");
		strcpy((char*)pname,(char*)src);				
		strcat((char*)pname,(char*)GBK24_PATH); 
		res=updata_fontx(x+20*size/2,y,size,pname,3);	
		if(res){vPortFree(pname)/*myfree(SRAMIN,pname)*/;return 4;}
		ftinfo.fontok=0XAA;
		W25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	
	}
	vPortFree(pname);//myfree(SRAMIN,pname);
	vPortFree(buf);//myfree(SRAMIN,buf);
	return rval;		 
} 

/*
*********************************************************************************************************
*	�� �� ��: font_init
*	����˵��: ��ʼ������
*	��    �Σ���						
*	�� �� ֵ: 0,�ֿ����.
						����,�ֿⶪʧ
*********************************************************************************************************
*/
u8 font_init(void)
{		
	u8 t=0;
	W25QXX_Init();  
	while(t<10)
	{
		t++;
		W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));
		if(ftinfo.fontok==0XAA)break;
		delay_ms(20);
	}
	if(ftinfo.fontok!=0XAA)return 1;
	return 0;		    
}
