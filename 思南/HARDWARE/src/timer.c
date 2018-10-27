/*
*********************************************************************************************************
*	                                  
*	模块名称 : 定时器驱动模块
*	文件名称 : timer.c
*	版    本 : V1.3
*	说    明 : 定时器设置
*
*
*********************************************************************************************************
*/

#include "timer.h"
#include "uart_usr.h"	
 
/*
*********************************************************************************************************
*	函 数 名: TIM7_Int_Init
*	功能说明: 定时器7初始化设置
*	形    参：arr:自动重装载值  psc:时钟预分频数
*	返 回 值: 无
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
*	函 数 名: TIM7_IRQHandler
*	功能说明: 定时器7中断服务程序
*	形    参：无
*	返 回 值: 无
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
