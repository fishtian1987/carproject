/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ͨ������ģ��
*	�ļ����� : uart_syslink.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
