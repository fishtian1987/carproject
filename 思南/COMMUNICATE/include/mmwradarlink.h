/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���ײ��״����ݽ����뷢��
*	�ļ����� : mmwradarlink.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
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
