/*
*********************************************************************************************************
*	                                  
*	模块名称 : 下主板通信驱动模块
*	文件名称 : uart_syslink.c
*	版    本 : V1.3
*	说    明 : 下主板通信驱动模块
*
*
*********************************************************************************************************
*/

#include "uart_syslink.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "debug_assert.h"
#include "common.h"

static xQueueHandle uartslkDataDelivery;

/*
*********************************************************************************************************
*	函 数 名: uartslkInit
*	功能说明: 与上主板通信初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void uartslkInit(void)
{	
	uartslkDataDelivery = xQueueCreate(10, sizeof(u8));
	ASSERT(uartslkDataDelivery);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
	USART_Cmd(USART3, ENABLE); 
}

/*
*********************************************************************************************************
*	函 数 名: uartslkGetDataWithTimout
*	功能说明: 从接收队列读取数据(带超时处理)
*	形    参：c,返回字符
*	返 回 值: true,成功获得字符，false,没有花的字符
*********************************************************************************************************
*/
bool uartslkGetDataWithTimout(u8 *c)
{
	if (xQueueReceive(uartslkDataDelivery, c, UARTSLK_DATA_TIMEOUT_TICKS) == pdTRUE)	
		return true;
	*c = 0;
	return false;
}

/*
*********************************************************************************************************
*	函 数 名: USART3_IRQHandler
*	功能说明: USART3中断服务函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void USART3_IRQHandler(void)               
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
	if ((USART3->SR & (1<<5)) != 0) 
	{
		u8 rxDataInterrupt = USART3->DR & 0xFF;
		xQueueSendFromISR(uartslkDataDelivery, &rxDataInterrupt, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
} 

/*
*********************************************************************************************************
*	函 数 名: UART3_SendString
*	功能说明: USART3发送字符串
*	形    参：str:待发送字符串指针
            size:大小
*	返 回 值: 无
*********************************************************************************************************
*/
void UART3_SendString(char* str,u8 size)
{
	u8 i=0;
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
		USART_SendData(USART3,*(str+i));
	}
}
