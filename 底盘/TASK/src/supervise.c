/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��ش���ģ��
*	�ļ����� : supervise.c
*	��    �� : V1.3
*	˵    �� : ��ش���ģ��
*
*
*********************************************************************************************************
*/

#include "supervise.h"
#include "LostCounter.h"
#include "io.h"	
#include "led.h"

LostCounter_t lost_counter[LOST_COUNTER_NUM] = { 2000, 2000, 2000, 2000, 2000,2000,2000};
const LostCounter_t lost_counter_len[LOST_COUNTER_NUM] = {20,500, 500, 500, 500,200,100};

uint32_t lost_err = 0xFFFFFFFF;     //ÿһλ����һ������

/*
*********************************************************************************************************
*	�� �� ��: Get_Lost_Error
*	����˵��: ���ش�����
*	��    �Σ�err_code,������
*	�� �� ֵ: ������
*********************************************************************************************************
*/
uint32_t Get_Lost_Error(uint32_t err_code)
{
	return lost_err&err_code;
}

/*
*********************************************************************************************************
*	�� �� ��: LED_TOGGLE
*	����˵��: ������ش����Ƿ���notice: CYCLE_S>=TOGGLE_S n>=0
*	��    �Σ�led 0: led1 toggle led 1:led2 toggle
						CYCLE_S: TOGGLE_S + OFF_S 
						shine_times: times of the LED shine num
						TIME_SPAN: intrrrupt time period in ms
*	�� �� ֵ: ������
*********************************************************************************************************
*/
void LED_TOGGLE(uint8_t led, uint16_t CYCLE_S, uint16_t TOGGLE_S, uint16_t shine_times,uint16_t TIME_SPAN)
{
	static uint32_t timeSpanCount = 0;
	static uint32_t tempCount = 0;
	static uint32_t sec = 0;
	uint32_t sec_cycle = 0;   
	timeSpanCount++; 
	sec = timeSpanCount/(1000/TIME_SPAN);  
	sec_cycle = sec % CYCLE_S; 
	
	if(sec_cycle/TOGGLE_S == 0)   
	{   
		tempCount++;
		if(shine_times <=0 ) 
		{
			if(led == 0){LED1=0;LED2=1;}
			else{LED1=1;LED2=0;}
		}
		else if(tempCount%(((1000/TIME_SPAN)*TOGGLE_S)/(2*shine_times)) == 0) 
		{
			if(led == 0){LED1_TOGGLE();LED2=1;}
			else{LED2_TOGGLE();LED1=1;}
		}			
	}
	else{tempCount = 0;LED1=1;LED2=1;}	
}

/*
*********************************************************************************************************
*	�� �� ��: SuperviseTask
*	����˵��: �������״̬
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SuperviseTask(void)
{
	u8 i = 0;
	if(Is_AppParam_Calied())
	{
		LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_NOCALI));
	}
	//step 1;check the error 
	for(i = 0; i < LOST_COUNTER_NUM; i++)
	{
		if(LostCounterOverflowCheck(lost_counter[i], lost_counter_len[i]) == 0)    //4ms*200 = 2s error check time
		{
			LostCounterCount(&lost_counter[i], 1);   //add 1 everytime
			lost_err &= ~(uint32_t)(1 << i);    //clear the error bit
		}
		else lost_err |= (uint32_t)(1 << i);    //set the error bit
	}
	
	if(Get_Lost_Error(LOST_ERROR_NOCALI) == LOST_ERROR_NOCALI)
	{
		LED_TOGGLE(0,6,0,0,4);   //red always on
	}	
	else if(Get_Lost_Error(LOST_ERROR_MAG) == LOST_ERROR_MAG)
	{
		LED_TOGGLE(0,7,5,10,4);
	}
//	else if(Get_Lost_Error(LOST_ERROR_IMU) == LOST_ERROR_ZGYRO)
//	{
//		LED_TOGGLE(0,6,0,1,4);
//	}
//	else if(Get_Lost_Error(LOST_ERROR_MOTOR6) == LOST_ERROR_MOTOR6)
//	{
//		LED_TOGGLE(0,5,3,6,4);
//	}	
//	else if(Get_Lost_Error(LOST_ERROR_MOTOR5) == LOST_ERROR_MOTOR5)
//	{
//		LED_TOGGLE(0,5,3,5,4);
//	}	
	else if(Get_Lost_Error(LOST_ERROR_MOTOR1) == LOST_ERROR_MOTOR1)
	{
		LED_TOGGLE(0,3,2,1,4);
	}
	else if(Get_Lost_Error(LOST_ERROR_MOTOR2) == LOST_ERROR_MOTOR2)
	{
		LED_TOGGLE(0,4,2,2,4);
	}
	else if(Get_Lost_Error(LOST_ERROR_MOTOR3) == LOST_ERROR_MOTOR3)
	{
		LED_TOGGLE(0,5,3,3,4);
	}
	else if(Get_Lost_Error(LOST_ERROR_MOTOR4) == LOST_ERROR_MOTOR4)
	{
		LED_TOGGLE(0,5,3,4,4);
	}
	else if(Get_Lost_Error(LOST_ERROR_RC) == LOST_ERROR_RC)
	{
		LED_TOGGLE(1,5,5,5,4);
	}
//	else if(Get_Lost_Error(LOST_ERROR_DEADLOCK) == LOST_ERROR_DEADLOCK)
//	{
//		LED_TOGGLE(0,6,6,6,4);
//	}
	else
	{
		LED_TOGGLE(1,6,6,0,4); 
	}
}

/*
*********************************************************************************************************
*	�� �� ��: GetLostCounter
*	����˵��: ���ش�����
*	��    �Σ�index,���
*	�� �� ֵ: ������
*********************************************************************************************************
*/
uint32_t *GetLostCounter(uint8_t index)
{
	if(index < LOST_COUNTER_NUM)
		return &lost_counter[index];
	else return ((void *)0);
}

/*
*********************************************************************************************************
*	�� �� ��: Is_Lost_Error_Set
*	����˵��: ���������Ƿ�����
*	��    �Σ�err_code,������
*	�� �� ֵ: ������
*********************************************************************************************************
*/
uint8_t Is_Lost_Error_Set(uint32_t err_code)
{
	return Get_Lost_Error( err_code) == err_code;
}

/*
*********************************************************************************************************
*	�� �� ��: Is_Serious_Error
*	����˵��: ������ش����Ƿ���
*	��    �Σ�err_code,������
*	�� �� ֵ: ������
*********************************************************************************************************
*/
uint8_t Is_Serious_Error(void)
{
	return (Is_Lost_Error_Set(LOST_ERROR_IMU) || Is_Lost_Error_Set(LOST_ERROR_DEADLOCK)||Is_Lost_Error_Set(LOST_ERROR_MAG) || Is_Lost_Error_Set(LOST_ERROR_NOCALI));
}
