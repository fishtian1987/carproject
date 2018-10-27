/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��ʱ������ģ��
*	�ļ����� : timer.c
*	��    �� : V1.3
*	˵    �� : ��ʱ������
*
*
*********************************************************************************************************
*/

#include "timer.h"
#include "uart_usr.h"	
 
/*
*********************************************************************************************************
*	�� �� ��: TIM7_Int_Init
*	����˵��: ��ʱ��7��ʼ������
*	��    �Σ�arr:�Զ���װ��ֵ  psc:ʱ��Ԥ��Ƶ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE );
  
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
}

/*
*********************************************************************************************************
*	�� �� ��: TIM7_IRQHandler
*	����˵��: ��ʱ��7�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{	 		
	
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); 	
		TIM_Cmd(TIM7, DISABLE); 
	}
}
