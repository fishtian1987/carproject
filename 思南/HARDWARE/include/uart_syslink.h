/*
*********************************************************************************************************
*	                                  
*	模块名称 : 下主板通信驱动模块
*	文件名称 : uart_syslink.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __UART_SYSLINK_H
#define __UART_SYSLINK_H
#include <stdbool.h>
#include "sys.h"

void uartslkInit(void);
bool uartslkGetDataWithTimout(u8 *c);
void UART3_SendString(char* str,u8 size);
#endif 
