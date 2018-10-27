/*
*********************************************************************************************************
*	                                  
*	模块名称 : RTC时钟管理
*	文件名称 : rtc.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 

void My_RTC_Init(void);					
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);			
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week); 		
#endif
