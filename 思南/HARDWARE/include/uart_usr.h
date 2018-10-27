/*
*********************************************************************************************************
*	                                  
*	ģ������ : GPRSͨ�ų�ʼ��
*	�ļ����� : uart_usr.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef _UART_USR_H
#define _UART_USR_H
#include "sys.h"

#define USART2_REC_LEN  	2048 		

extern u8  USART2_RX_BUF[USART2_REC_LEN]; 
extern u16 USART2_RX_STA;  

void uartUsrInit(void);
void UART2_SendString(char* str);
#endif
