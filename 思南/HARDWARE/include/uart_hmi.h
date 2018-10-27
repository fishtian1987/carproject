/*
*********************************************************************************************************
*	                                  
*	模块名称 : HMI通信初始化
*	文件名称 : uart_hmi.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef _UART_HMI_H
#define _UART_HMI_H
#include "sys.h"

#define USART6_REC_LEN    15

extern char USART6_TX_BUF[USART6_REC_LEN]; 

void uartHmiInit(void);
#endif
