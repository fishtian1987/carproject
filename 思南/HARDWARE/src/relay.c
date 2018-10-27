/*
*********************************************************************************************************
*	                                  
*	ģ������ : �̵�������ģ��
*	�ļ����� : relay.c
*	��    �� : V1.3
*	˵    �� : �̵�������ģ��
*
*
*********************************************************************************************************
*/

#include "relay.h" 
 
/*
*********************************************************************************************************
*	�� �� ��: relayInit
*	����˵��: �̵�����ʼ��IO��
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: switch_bluetooth
*	����˵��: ����
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: switch_alarm
*	����˵��: ����
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: switch_tail_light
*	����˵��: β��
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: switch_brake_light
*	����˵��: ɲ��
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: turn_left_light
*	����˵��: ��ת��
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: turn_right_light
*	����˵��: ��ת��
*	��    �Σ�sw,1��;0�ر�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void switch_right_light(u8 *sw)
{
	if(*sw)
		GPIO_SetBits(GPIOE,GPIO_Pin_9); 
	else
		GPIO_ResetBits(GPIOE,GPIO_Pin_9); 
}
