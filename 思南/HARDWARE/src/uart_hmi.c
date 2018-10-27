/*
*********************************************************************************************************
*	                                  
*	ģ������ : HMIͨ�ų�ʼ��
*	�ļ����� : uart_hmi.c
*	��    �� : V1.3
*	˵    �� : HMIͨ�ų�ʼ��
*
*
*********************************************************************************************************
*/

#include "uart_hmi.h"

char USART6_TX_BUF[USART6_REC_LEN]; 
/*
*********************************************************************************************************
*	�� �� ��: usart6_init
*	����˵��: USART6��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void uartHmiInit(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

 	USART_DeInit(USART6);  	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx; 
	USART_Init(USART6, &USART_InitStructure); 
	USART_Cmd(USART6, ENABLE);              
}
