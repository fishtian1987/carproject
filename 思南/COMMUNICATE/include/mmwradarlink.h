/*
*********************************************************************************************************
*	                                  
*	模块名称 : 毫米波雷达数据接收与发送
*	文件名称 : mmwradarlink.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __MMWRADARLINK_H
#define __MMWRADARLINK_H
#include "sys.h"
#include "atkp.h"

void mmwRadarlinkInit(void);
void mmwRadarlinkTask(void *param);
bool mmwRadarlinkSendPacket(const atkp_t *p);
void mmwRadarlinkReciveTask(void *param);
bool mmwRadarlinkSendPacketBlocking(const atkp_t *p);
#endif
