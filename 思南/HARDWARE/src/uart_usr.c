/*
*********************************************************************************************************
*	                                  
*	ģ������ : GPRSͨ�ų�ʼ��
*	�ļ����� : uart_usr.c
*	��    �� : V1.3
*	˵    �� : GPRSͨ�ų�ʼ��
*
*
*********************************************************************************************************
*/

#include "uart_usr.h"
#include "timer.h"

/*
*********************************************************************************************************
*	�� �� ��: uart2_init
*	����˵��: GPRSͨ�ų�ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void uartUsrInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);  
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
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
	USART_Init(USART2, &USART_InitStructure); 
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_Cmd(USART2, ENABLE);
//	TIM7_Int_Init(1000-1,8400-1);		//100ms�ж�
//	TIM_Cmd(TIM7, DISABLE); 
}

u8 USART2_RX_BUF[USART2_REC_LEN]; 			
u16 USART2_RX_STA=0;   
/*
*********************************************************************************************************
*	�� �� ��: USART2_IRQHandler
*	����˵��: USART2�жϷ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void USART2_IRQHandler(void) 
{
	u8 Res=0;
	if((USART2->SR & (1<<5)) != 0) 
	{
		Res =USART2->DR & 0xFF;
		if((USART2_RX_STA&0x8000)==0)
		{ 
			if(USART2_RX_STA<USART2_REC_LEN)	
			{
				TIM_SetCounter(TIM7,0);  				 
				if(USART2_RX_STA==0)		
					TIM_Cmd(TIM7, ENABLE);  
				USART2_RX_BUF[USART2_RX_STA++]=Res;	
			}
			else 
					USART2_RX_STA=0;
		}  	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: UART2_SendString
*	����˵��: USART2�����ַ���
*	��    �Σ�str:�������ַ���ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UART2_SendString(char* str)
{
	while(*str != '\0')
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
		USART_SendData(USART2 ,*str);
		str++;
	}
}
