/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ͨ��
*	�ļ����� : trandata.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
