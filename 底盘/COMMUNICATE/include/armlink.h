/*
*********************************************************************************************************
*	                                  
*	ģ������ : arm���ݽ����뷢��
*	�ļ����� : armlink.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
