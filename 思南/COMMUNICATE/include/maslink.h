/*
*********************************************************************************************************
*	                                  
*	ģ������ : У������
*	�ļ����� : maslink.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
