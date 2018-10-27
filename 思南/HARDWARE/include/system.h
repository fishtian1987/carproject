/*
*********************************************************************************************************
*	                                  
*	ģ������ : Ӳ����ʼ��ģ��
*	�ļ����� : system.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef __SYSTEM_H
#define __SYSTEM_H
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "config.h"
#include "nvic.h"
#include "exti.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "relay.h" 
#include "beep.h" 
#include "gps.h"
#include "hmi.h"
#include "atkp.h"
#include "uart_gps.h"
#include "trandata.h"
#include "uart_usr.h"
#include "uart_hmi.h"
#include "maslink.h"

void systemInit(void);

#endif
