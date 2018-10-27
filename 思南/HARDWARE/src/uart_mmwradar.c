/*
*********************************************************************************************************
*	                                  
*	模块名称 : 毫米波雷达通信初始化
*	文件名称 : uart_mmwradar.c
*	版    本 : V1.3
*	说    明 : 毫米波雷达通信初始化
*
*
*********************************************************************************************************
*/

#include "uart_mmwradar.h"
#include "atkp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "common.h"

static xQueueHandle uartblueDataDelivery;

/*
*********************************************************************************************************
*	函 数 名: uartmmwRadarInit
*	功能说明: USART4初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void uartmmwRadarInit(void)
{
	uartblueDataDelivery = xQueueCreate(10, sizeof(u8));	/*队列 10个消息*/
	ASSERT(uartblueDataDelivery);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	rxState = waitForStartByte1;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	USART_DeInit(UART4); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure); 
	
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
	USART_Cmd(UART4, ENABLE); 
}

/*
*********************************************************************************************************
*	函 数 名: uartmmwRadarGetDataWithTimout
*	功能说明: 从接收队列读取数据(带超时处理)
*	形    参：c,返回字符
*	返 回 值: true,成功获得字符，false,没有花的字符
*********************************************************************************************************
*/
bool uartmmwRadarGetDataWithTimout(u8 *c)
{
	if (xQueueReceive(uartblueDataDelivery, c, UARTSLK_DATA_TIMEOUT_TICKS) == pdTRUE)	
		return true;
	*c = 0;
	return false;
}

/*
*********************************************************************************************************
*	函 数 名: UART4_IRQHandler
*	功能说明: UART4中断服务函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void UART4_IRQHandler(void)               
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) 
	{
		u8 rxDataInterrupt = USART_ReceiveData(UART4);
		xQueueSendFromISR(uartblueDataDelivery, &rxDataInterrupt, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
} 

/*
*********************************************************************************************************
*	函 数 名: UART4_SendString
*	功能说明: UART4发送字符串
*	形    参：str:待发送字符串指针
            size:大小
*	返 回 值: 无
*********************************************************************************************************
*/
void UART4_SendString(char* str,u8 size)
{
	u8 i=0;
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); 
		USART_SendData(UART4,*(str+i));
	}
}
