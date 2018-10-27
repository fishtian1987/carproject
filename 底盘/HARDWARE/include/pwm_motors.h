/*
*********************************************************************************************************
*	                                  
*	ģ������ : �������ģ��
*	�ļ����� : pwm_motors.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef _PWM_MOTORS_H
#define _PWM_MOTORS_H
#include "sys.h"

#define PWM_LR_STICK_VALUE_HIGH		 840
#define PWM_LR_STICK_VALUE_MIDDLE  600
#define PWM_LR_STICK_VALUE_LOW		 360
#define PWM_FB_STICK_VALUE_RANGE		 999

typedef struct 
{
	u16 m1;
	u16 m2;
}motorPWM_t;

#define MOTOR_EN PEout(6)
#define MOTOR_FR PAout(8)

void MotorsInit(void);
void MotorsStop(void);
void MotorsStart(void);
void MotorsSetRatio(int16_t rotation,int16_t ithrust);
#endif
