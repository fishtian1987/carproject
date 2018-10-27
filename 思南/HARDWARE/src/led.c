/*
*********************************************************************************************************
*	                                  
*	模块名称 : LED灯驱动模块
*	文件名称 : led.c
*	版    本 : V1.3
*	说    明 : LED灯驱动程序
*
*
*********************************************************************************************************
*/

#include "led.h"

/*
*********************************************************************************************************
*	函 数 名: LED_Init
*	功能说明: LED IO初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG,ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; 
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_9);
	GPIO_SetBits(GPIOE,GPIO_Pin_3|GPIO_Pin_4); 
}
