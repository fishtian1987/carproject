/*
*********************************************************************************************************
*	                                  
*	ģ������ : LED������ģ��
*	�ļ����� : led.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef _LED_H
#define _LED_H
#include "sys.h"
#include "stdbool.h"

#define LED0 PEout(3)
#define LED1 PEout(4)
#define LED2 PGout(9)
#define LED3 PEout(2)

#define LED0_TOGGLE()  GPIO_ToggleBits(GPIOE, GPIO_Pin_3)
#define LED1_TOGGLE()  GPIO_ToggleBits(GPIOE, GPIO_Pin_4)
#define LED2_TOGGLE()  GPIO_ToggleBits(GPIOG, GPIO_Pin_9)

void ledInit(void);
#endif
