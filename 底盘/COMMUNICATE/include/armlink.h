/*
*********************************************************************************************************
*	                                  
*	模块名称 : arm数据接收与发送
*	文件名称 : armlink.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __ARMLINK_H
#define __ARMLINK_H
#include "sys.h"
#include "atkp.h"

void armlinkInit(void);
void armlinkTask(void *param);
bool armlinkSendPacket(const atkp_t *p);
void armlinkReciveTask(void *param);
bool armlinkSendPacketBlocking(const atkp_t *p);
#endif
