/*
*********************************************************************************************************
*	                                  
*	模块名称 : 继电器驱动模块
*	文件名称 : relay.h
*	版    本 : V1.3
*	说    明 : 头文件
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
