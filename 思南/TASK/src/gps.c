/*
*********************************************************************************************************
*	                                  
*	模块名称 : GPS驱动模块
*	文件名称 : gps.c
*	版    本 : V1.3
*	说    明 : GPS驱动程序
*
*
*********************************************************************************************************
*/

#include "gps.h" 						   							   
#include "string.h"	 
#include "uart_gps.h"	

nmea_msg gpsx;
/*
*********************************************************************************************************
*	函 数 名: NMEA_Comma_Pos
*	功能说明: 查找逗号所在的位置
*	形    参：buf,字符串
						cx,第几个逗号
*	返 回 值: 0~0XFE,代表逗号所在位置的偏移.
						0XFF,代表不存在第cx个逗号	
*********************************************************************************************************
*/
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

/*
*********************************************************************************************************
*	函 数 名: NMEA_Pow
*	功能说明: m^n函数
*	形    参：m,底
						n,幂
*	返 回 值: m^n次方.
*********************************************************************************************************
*/
uint64_t NMEA_Pow(u8 m,u8 n)
{
	uint64_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*
*********************************************************************************************************
*	函 数 名: NMEA_Str2num
*	功能说明: 字符转换为数字,以','或者'*'结束
*	形    参：buf:数字存储区
*	返 回 值: 转换后的数值
*********************************************************************************************************
*/
int64_t NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int64_t res;
	while(1) 
	{
		if(*p=='-'){mask|=0X02;p++;}
		if(*p==','||(*p=='*'))break;
		if(*p=='.'){mask|=0X01;p++;}
		else if(*p>'9'||(*p<'0'))	
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	
	for(i=0;i<ilen;i++)	 
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	if(flen>7)flen=7;	
	*dx=flen;	 		
	for(i=0;i<flen;i++)	
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 

/*
*********************************************************************************************************
*	函 数 名: NMEA_GPGGA_Analysis
*	功能说明: 分析GPGGA信息
*	形    参：gpsx:nmea信息结构体
						buf:接收到的GPS数据缓冲区首地址
*	返 回 值: 无
*********************************************************************************************************
*/
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;					 
	u8 posx;  
	int64_t temp=0;
	u32 temp1=0;
	int Degree;  	
	u32	Cent;
	
	if((p1=(u8*)strstr((const char *)buf,"GGA"))==NULL)
		return;
	posx=NMEA_Comma_Pos(p1,2);								
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	
		temp1=NMEA_Pow(10,dx+2);	
		Degree=temp/temp1;	
		Cent=temp-Degree*temp1;		
		gpsx->latitude=Degree+Cent/60.0/NMEA_Pow(10,dx);
	}				 
 	posx=NMEA_Comma_Pos(p1,4);								
	if(posx!=0XFF)
	{			
		temp=NMEA_Str2num(p1+posx,&dx);	
		temp1=NMEA_Pow(10,dx+2);	
		Degree=temp/temp1;	
		Cent=temp-Degree*temp1;		
		gpsx->longitude=Degree+Cent/60.0/NMEA_Pow(10,dx);
	}	 
	posx=NMEA_Comma_Pos(p1,6);								
	if(posx!=0XFF)
		gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								
	if(posx!=0XFF)
		gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	gpsx->bflagProce=true;
}

/*
*********************************************************************************************************
*	函 数 名: NMEA_GPTRA_Analysis
*	功能说明: 分析GPTRA信息
*	形    参：gpsx:nmea信息结构体
						buf:接收到的GPS数据缓冲区首地址
*	返 回 值: 无
*********************************************************************************************************
*/
void NMEA_GPTRA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;					 
	u8 posx;  
	u16 temp=0,temp1=0;	   
	u16 Degree;  	
	u16	Cent;
	
	if((p1=(u8*)strstr((const char *)buf,"TRA"))==NULL)
		return;

	posx=NMEA_Comma_Pos(p1,2);								
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	
		temp1=NMEA_Pow(10,dx);	
		Degree=temp/temp1;	
		Cent=temp-Degree*temp1;		
		gpsx->heading=Degree+Cent*1.0/NMEA_Pow(10,dx);
	}				 
// 	posx=NMEA_Comma_Pos(p1,3);								
//	if(posx!=0XFF)
//	{			
//		temp=NMEA_Str2num(p1+posx,&dx);	
//		temp1=NMEA_Pow(10,dx+2);	
//		Degree=temp/temp1;	
//		Cent=temp-Degree*temp1;		
//		gpsx->pitch=Degree+Cent*1.0/NMEA_Pow(10,dx);
//	}	 
//	posx=NMEA_Comma_Pos(p1,4);								
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx);	
//		temp1=NMEA_Pow(10,dx+2);	
//		Degree=temp/temp1;	
//		Cent=temp-Degree*temp1;		
//		gpsx->roll=Degree+Cent*1.0/NMEA_Pow(10,dx);
//	}
	gpsx->bflagProce=true;
}

/*
*********************************************************************************************************
*	函 数 名: NMEA_GPVTG_Analysis
*	功能说明: 分析GPVTG信息
*	形    参：gpsx:nmea信息结构体
						buf:接收到的GPS数据缓冲区首地址
*	返 回 值: 无
*********************************************************************************************************
*/
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;  
	u16 temp=0;
	
	if((p1=(u8*)strstr((const char *)buf,"VTG"))==NULL)
		return;		
	
	posx=NMEA_Comma_Pos(p1,7);								//得到地面速率
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);
		//if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//确保扩大1000倍
		gpsx->speed=temp/3600.0;
	}
}  

/*
*********************************************************************************************************
*	函 数 名: NMEA_Analysis
*	功能说明: 分析GPXXX信息
*	形    参：gpsx:nmea信息结构体
						buf:接收到的GPS数据缓冲区首地址
*	返 回 值: 无
*********************************************************************************************************
*/
void NMEA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	if(!gpsx->bflagProce)
		NMEA_GPGGA_Analysis(gpsx,buf);

	if(!gpsx->bflagProce)
		NMEA_GPTRA_Analysis(gpsx,buf);
	if(!gpsx->bflagProce)
		NMEA_GPVTG_Analysis(gpsx,buf);
}
	
