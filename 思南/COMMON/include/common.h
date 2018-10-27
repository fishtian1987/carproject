/*
*********************************************************************************************************
*	                                  
*	模块名称 : 共有部分
*	文件名称 : common.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __COMMON_H
#define __COMMON_H
#include "sys.h"

#define UARTSLK_DATA_TIMEOUT_MS 	1000
#define UARTSLK_DATA_TIMEOUT_TICKS (UARTSLK_DATA_TIMEOUT_MS / portTICK_RATE_MS)
#endif
