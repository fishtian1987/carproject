/*
*********************************************************************************************************
*	                                  
*	模块名称 : 硬件初始化模块
*	文件名称 : system.h
*	版    本 : V1.3
*	说    明 : 头文件
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
#include "lcd.h"
#include "pwm_motors.h"
#include "w25qxx.h" 
#include "rtc.h"
#include "beep.h" 
#include "sdcard.h"
#include "uart_syslink.h"
#include "usart_remote.h"
#include "remote.h"
#include "atkp.h"
#include "maslink.h"
#include "armlink.h"
#include "stabilizer.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"	
#include "io.h"	
#include "config_param.h"
#include "uart_debug.h"

void systemInit(void);

#endif
