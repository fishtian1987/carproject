/*
*********************************************************************************************************
*	                                  
*	ģ������ : �������ģ��
*	�ļ����� : pwm_motors.c
*	��    �� : V1.3
*	˵    �� : ����������ݴ���
*
*
*********************************************************************************************************
*/

#include "pwm_motors.h"
#include "common.h"
#include "stdlib.h"

/*
*********************************************************************************************************
*	�� �� ��: MotorsInit
*	����˵��: ���������ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MotorsInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4,ENABLE);   
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	TIM_DeInit(TIM4);	
	TIM_DeInit(TIM3);	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); 	//MOTOR0,ת����
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4); 	//MOTOR1,�˶���
  
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;        			
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;		
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;      			
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;   
	GPIO_Init(GPIOA,&GPIO_InitStructure);  	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period=8000-1;			//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler=210-1;	//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 				
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period=(int16_t)PWM_FB_STICK_VALUE_RANGE;
	TIM_TimeBaseStructure.TIM_Prescaler=84-1;	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);			
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;				
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse=(int16_t)PWM_LR_STICK_VALUE_MIDDLE;						
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;		
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); 
	
	TIM_OCInitStructure.TIM_Pulse=0;				
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); 
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 
  
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: MotorsStart
*	����˵��: �����ʼ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MotorsStart(void)
{
	MOTOR_EN = 0;
	TIM_Cmd(TIM4, ENABLE); 
	TIM_Cmd(TIM3, ENABLE); 
}

/*
*********************************************************************************************************
*	�� �� ��: MotorsStop
*	����˵��: ���ֹͣ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MotorsStop(void)
{
	MOTOR_EN = 1;
	TIM_Cmd(TIM4, DISABLE); 
	TIM_Cmd(TIM3, DISABLE); 
}

/*
*********************************************************************************************************
*	�� �� ��: MotorsSetRatio
*	����˵��: ����PWMռ�ձ�
*	��    �Σ�rotation,����;ithrust,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MotorsSetRatio(int16_t rotation,int16_t ithrust)
{
	int16_t temp=0;
	temp = PWM_LR_STICK_VALUE_MIDDLE+rotation;
	VAL_LIMIT(temp, PWM_LR_STICK_VALUE_LOW, PWM_LR_STICK_VALUE_HIGH);
	TIM_SetCompare1(TIM3,temp);
	
	temp = abs(ithrust);
	VAL_LIMIT(temp,0,PWM_FB_STICK_VALUE_RANGE);
	if(temp>10)	
	{
		if(ithrust<0)
			MOTOR_FR = 1;
		else
			MOTOR_FR = 0;	
		TIM_SetCompare1(TIM4,temp);
	}
	else
		TIM_SetCompare1(TIM4,0);
}
