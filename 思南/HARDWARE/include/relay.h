/*
*********************************************************************************************************
*	                                  
*	ģ������ : �̵�������ģ��
*	�ļ����� : relay.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef __RELAY_H
#define __RELAY_H	 
#include "sys.h" 
#include "stdbool.h" 

void relayInit(void);	 	
void switch_bluetooth(u8 *sw);
void switch_alarm(u8 *sw);
void switch_tail_light(u8 *sw);
void switch_brake_light(u8 *sw);
void switch_left_light(u8 *sw);
void switch_right_light(u8 *sw);
#endif
