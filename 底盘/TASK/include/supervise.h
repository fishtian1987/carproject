/*
*********************************************************************************************************
*	                                  
*	模块名称 : 监控处理模块
*	文件名称 : supervise.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __SUPERVISE_H__
#define __SUPERVISE_H__
#include "sys.h"

#define LOST_COUNTER_NUM           7u

//IMU不正常
#define LOST_COUNTER_INDEX_RC                        0u  
#define LOST_COUNTER_INDEX_MOTOR1                    1u  
#define LOST_COUNTER_INDEX_MOTOR2                    2u   
#define LOST_COUNTER_INDEX_MOTOR3                    3u   
#define LOST_COUNTER_INDEX_MOTOR4                    4u   
#define LOST_COUNTER_INDEX_MAG                       5u  
#define LOST_COUNTER_INDEX_NOCALI            		     6u  

#define LOST_COUNTER_INDEX_IMU                       7u    
#define LOST_COUNTER_INDEX_MOTOR5                    8u   
#define LOST_COUNTER_INDEX_MOTOR6                    9u   
#define LOST_COUNTER_INDEX_DEADLOCK                  10u   


#define LOST_ERROR_RC									  (1<<0)	
#define LOST_ERROR_MOTOR1								(1<<1)	
#define LOST_ERROR_MOTOR2								(1<<2)		
#define LOST_ERROR_MOTOR3								(1<<3)		
#define LOST_ERROR_MOTOR4								(1<<4)
#define LOST_ERROR_MAG							  	(1<<5)
#define LOST_ERROR_NOCALI  						  (1<<6)	

#define LOST_ERROR_IMU									(1<<7)		
#define LOST_ERROR_MOTOR5								(1<<8)		
#define LOST_ERROR_MOTOR6								(1<<9)		
#define LOST_ERROR_DEADLOCK							(1<<10)		

void SuperviseTask(void);
uint32_t *GetLostCounter(uint8_t index);
uint8_t Is_Lost_Error_Set(uint32_t err_code);
uint8_t Is_Serious_Error(void);
#endif 
