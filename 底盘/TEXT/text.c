/*
*********************************************************************************************************
*	                                  
*	模块名称 : 字体显示处理模块
*	文件名称 : text.c
*	版    本 : V1.3
*	说    明 : 字体显示处理模块
*
*
*********************************************************************************************************
*/

#include "sys.h" 
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"	
#include "string.h"												    											    

/*
*********************************************************************************************************
*	函 数 名: Get_HzMat
*	功能说明: 从字库中查找出字模
*	形    参：code 字符指针开始
						code 字符串的开始地址,GBK码
						mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
						size:字体大小
*	返 回 值: 无
*********************************************************************************************************
*/
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	u8 csize=(size/8+((size%8)?1:0))*(size);
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)
	{   		    
		for(i=0;i<csize;i++)*mat++=0x00;
		return; 
	}          
	if(ql<0x7f)ql-=0x40;
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	  
	switch(size)
	{
		case 12:
			W25QXX_Read(mat,foffset+ftinfo.f12addr,csize);
			break;
		case 16:
			W25QXX_Read(mat,foffset+ftinfo.f16addr,csize);
			break;
		case 24:
			W25QXX_Read(mat,foffset+ftinfo.f24addr,csize);
			break;		
	}     												    
}   

/*
*********************************************************************************************************
*	函 数 名: Show_Font
*	功能说明: 显示一个指定大小的汉字
*	形    参：x,y :汉字的坐标
						font:汉字GBK码
						size:字体大小
						mode:0,正常显示,1,叠加显示	  
*	返 回 值: 无
*********************************************************************************************************
*/
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[72];   
	u8 csize=(size/8+((size%8)?1:0))*(size);
	if(size!=12&&size!=16&&size!=24)return;	
	Get_HzMat(font,dzk,size);
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];	                       
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size){y=y0;x++;break;}
		}  	 
	}  
}

/*
*********************************************************************************************************
*	函 数 名: Show_Str
*	功能说明: 在指定位置开始显示一个字符串，支持自动换行
*	形    参：(x,y):起始坐标
						width,height:区域
						str  :字符串
						size :字体大小
						mode:0,非叠加方式;1,叠加方式   
*	返 回 值: 无
*********************************************************************************************************
*/
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
	u8 bHz=0;   				    				  	  
	while(*str!=0)
	{ 
		if(!bHz)
		{
			if(*str>0x80)bHz=1;
			else             
			{      
				if(x>(x0+width-size/2)){y+=size;x=x0;}							    
				if(y>(y0+height-size))break;
				if(*str==13){y+=size;x=x0;str++;}  
				else LCD_ShowChar(x,y,*str,size,mode);
				str++; 
				x+=size/2;
			}
		}else
		{     
			bHz=0;
			if(x>(x0+width-size)){y+=size;x=x0;}
			if(y>(y0+height-size))break;			     
			Show_Font(x,y,str,size,mode);
			str+=2; 
			x+=size;
		}						 
	}   
}  			 		 
	  
/*
*********************************************************************************************************
*	函 数 名: Show_Str_Mid
*	功能说明: 在指定宽度的中间显示字符串，如果字符长度超过了len,则用Show_Str显示
*	形    参：(x,y):起始坐标
						str  :字符串
						size :字体大小
						len:指定要显示的宽度			  
*	返 回 值: 无
*********************************************************************************************************
*/
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	u16 strlenth=0;
  strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	  Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}   
