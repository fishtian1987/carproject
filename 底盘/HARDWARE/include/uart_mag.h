/*
*********************************************************************************************************
*	                                  
*	模块名称 : 电子罗盘通信驱动
*	文件名称 : uart_mag.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __USART_MAG_H
#define __USART_MAG_H
#include "sys.h"

#define DOWN6_BYTE 0x77
#define DOWN6_ADDR 0x00
#define USART6_REC_LEN		14

void UsartCompassInit(void);
void UART6_SendString(u8* str,u8 size);
#endif
