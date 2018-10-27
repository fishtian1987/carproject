/*
*********************************************************************************************************
*	                                  
*	模块名称 : GPS通信驱动模块
*	文件名称 : uart_gps.c
*	版    本 : V1.3
*	说    明 : GPS通信驱动模块
*
*
*********************************************************************************************************
*/

#include "uart_gps.h"
#include "gps.h"

/*
*********************************************************************************************************
*	函 数 名: uartGpsInit
*	功能说明: USART1初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void uartGpsInit(void)
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_Cmd(USART1, ENABLE);
}

//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];  
u16 USART1_RX_STA=0;      
/*
*********************************************************************************************************
*	函 数 名: USART1_IRQHandler
*	功能说明: USART1中断服务函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void USART1_IRQHandler(void)                	
{
	u8 Res;
	if((USART1->SR & (1<<5)) != 0)  
	{
		Res =USART1->DR & 0xFF;
		
		if(Res=='$')
			USART1_RX_STA=0;
		if((USART1_RX_STA&0x8000)==0)
		{
			USART1_RX_BUF[USART1_RX_STA++]=Res ;

			if(USART1_RX_STA>(USART1_REC_LEN-1))
				USART1_RX_STA|=0x8000;
			if(Res=='\n')
			{
				gpsx.bflagProce=false;
				NMEA_Analysis(&gpsx,USART1_RX_BUF);
				USART1_RX_STA|=0x8000;
			}
		}
  } 
} 
