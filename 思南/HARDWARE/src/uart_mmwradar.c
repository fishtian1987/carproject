/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���ײ��״�ͨ�ų�ʼ��
*	�ļ����� : uart_mmwradar.c
*	��    �� : V1.3
*	˵    �� : ���ײ��״�ͨ�ų�ʼ��
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
*	�� �� ��: uartmmwRadarInit
*	����˵��: USART4��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void uartmmwRadarInit(void)
{
	uartblueDataDelivery = xQueueCreate(10, sizeof(u8));	/*���� 10����Ϣ*/
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
*	�� �� ��: uartmmwRadarGetDataWithTimout
*	����˵��: �ӽ��ն��ж�ȡ����(����ʱ����)
*	��    �Σ�c,�����ַ�
*	�� �� ֵ: true,�ɹ�����ַ���false,û�л����ַ�
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
*	�� �� ��: UART4_IRQHandler
*	����˵��: UART4�жϷ�����
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: UART4_SendString
*	����˵��: UART4�����ַ���
*	��    �Σ�str:�������ַ���ָ��
            size:��С
*	�� �� ֵ: ��
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
