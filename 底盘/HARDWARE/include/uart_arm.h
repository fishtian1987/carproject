/*
*********************************************************************************************************
*	                                  
*	ģ������ : armͨ�ų�ʼ��
*	�ļ����� : uart_arm.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
