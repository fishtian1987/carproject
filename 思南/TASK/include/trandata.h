/*
*********************************************************************************************************
*	                                  
*	模块名称 : 与上主板通信
*	文件名称 : trandata.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __TRANDATA_H
#define __TRANDATA_H
#include "sys.h" 
#include "stdbool.h" 

typedef __packed struct
{
	double latitude;	
	double longitude;			 
	u8 gpssta;		
	u16 heading;
	u16 speed;
} tranData_t;

void commanderTask(void* param);
#endif
