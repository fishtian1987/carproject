/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���ײ��״�ͨ�ų�ʼ��
*	�ļ����� : uart_mmwradar.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
