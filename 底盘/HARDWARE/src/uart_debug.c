/*
*********************************************************************************************************
*	                                  
*	模块名称 : 调试通信初始化
*	文件名称 : uart_debug.c
*	版    本 : V1.3
*	说    明 : 调试通信初始化
*
*
*********************************************************************************************************
*/

#include "uart_debug.h"
#include "stdio.h"	

/*
*********************************************************************************************************
*	函 数 名: uartDebugInit
*	功能说明: USART1初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void uartDebugInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_Cmd(USART1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: UART1_SendString
*	功能说明: UART1发送字符串
*	形    参：str:待发送字符串指针
            size:大小
*	返 回 值: 无
*********************************************************************************************************
*/
void UART1_SendString(char* str,u8 size)
{
	u8 i=0;
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1,*(str+i));
	}
}
