/*
*********************************************************************************************************
*	                                  
*	模块名称 : 遥控器驱动
*	文件名称 : usart_remote.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __USART_REMOTE_H
#define __USART_REMOTE_H
#include "sys.h"

#define USART3_REC_LEN		26	
#define RC_FRAME_LEN      25u		

void UsartRemoteInit(void);
#endif
