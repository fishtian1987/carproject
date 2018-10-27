/*
*********************************************************************************************************
*	                                  
*	模块名称 : 蜂鸣器驱动模块
*	文件名称 : beep.c
*	版    本 : V1.3
*	说    明 : 蜂鸣器驱动程序
*
*
*********************************************************************************************************
*/

#include "beep.h" 
 
/*
*********************************************************************************************************
*	函 数 名: BEEP_Init
*	功能说明: 蜂鸣器初始化IO口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
  GPIO_ResetBits(GPIOG,GPIO_Pin_7);  
}
