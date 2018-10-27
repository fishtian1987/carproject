/*
*********************************************************************************************************
*	                                  
*	模块名称 : 校验数据
*	文件名称 : maslink.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __RADIO_H
#define __RADIO_H
#include <stdint.h>
#include <stdbool.h>
#include "atkp.h"


void maslinkInit(void);
void maslinkTask(void *param);
bool radiolinkSendPacket(const atkp_t *p);
bool radiolinkSendPacketBlocking(const atkp_t *p);
#endif 
