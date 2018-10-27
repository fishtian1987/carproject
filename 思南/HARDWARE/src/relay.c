/*
*********************************************************************************************************
*	                                  
*	模块名称 : 继电器驱动模块
*	文件名称 : relay.c
*	版    本 : V1.3
*	说    明 : 继电器驱动模块
*
*
*********************************************************************************************************
*/

#include "relay.h" 
 
/*
*********************************************************************************************************
*	函 数 名: relayInit
*	功能说明: 继电器初始化IO口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void relayInit(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
  GPIO_ResetBits(GPIOD,GPIO_Pin_0);  
  GPIO_ResetBits(GPIOD,GPIO_Pin_1);  
  GPIO_ResetBits(GPIOD,GPIO_Pin_14);  
  GPIO_ResetBits(GPIOD,GPIO_Pin_15); 
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_7);  
  GPIO_ResetBits(GPIOE,GPIO_Pin_8);  
  GPIO_ResetBits(GPIOE,GPIO_Pin_9);  
  GPIO_ResetBits(GPIOE,GPIO_Pin_10); 
}

/*
*********************************************************************************************************
*	函 数 名: switch_bluetooth
*	功能说明: 蓝牙
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_bluetooth(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOD,GPIO_Pin_1); 
	else
		GPIO_ResetBits(GPIOD,GPIO_Pin_1); 
}

/*
*********************************************************************************************************
*	函 数 名: switch_alarm
*	功能说明: 警灯
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_alarm(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOD,GPIO_Pin_0); 
	else
		GPIO_ResetBits(GPIOD,GPIO_Pin_0); 
}

/*
*********************************************************************************************************
*	函 数 名: switch_tail_light
*	功能说明: 尾灯
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_tail_light(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOE,GPIO_Pin_8); 
	else
		GPIO_ResetBits(GPIOE,GPIO_Pin_8); 
}

/*
*********************************************************************************************************
*	函 数 名: switch_brake_light
*	功能说明: 刹车
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_brake_light(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOE,GPIO_Pin_7); 
	else
		GPIO_ResetBits(GPIOE,GPIO_Pin_7); 
}

/*
*********************************************************************************************************
*	函 数 名: turn_left_light
*	功能说明: 左转灯
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_left_light(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOE,GPIO_Pin_10); 
	else
		GPIO_ResetBits(GPIOE,GPIO_Pin_10); 
}

/*
*********************************************************************************************************
*	函 数 名: turn_right_light
*	功能说明: 右转灯
*	形    参：sw,1打开;0关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void switch_right_light(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOE,GPIO_Pin_9); 
	else
		GPIO_ResetBits(GPIOE,GPIO_Pin_9); 
}
