/*
*********************************************************************************************************
*	                                  
*	模块名称 : arm通信初始化
*	文件名称 : uart_arm.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef _UART_ARM_H
#define _UART_ARM_H
#include "sys.h"
#include "stdbool.h"

#define UART4_REC_LEN    20

void uartArmInit(void);
bool uartArmGetDataWithTimout(u8 *c);
void UART4_SendString(char* str,u8 size);
#endif
