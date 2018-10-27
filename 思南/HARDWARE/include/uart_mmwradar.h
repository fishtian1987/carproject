/*
*********************************************************************************************************
*	                                  
*	模块名称 : 毫米波雷达通信初始化
*	文件名称 : uart_mmwradar.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef _UART_MMWRADAR_H
#define _UART_MMWRADAR_H
#include "sys.h"
#include "stdbool.h"

#define UART4_REC_LEN    20

void uartmmwRadarInit(void);
bool uartmmwRadarGetDataWithTimout(u8 *c);
void UART4_SendString(char* str,u8 size);
#endif
