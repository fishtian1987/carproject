/*
*********************************************************************************************************
*	                                  
*	模块名称 : 遥控器通信驱动
*	文件名称 : usart_remote.c
*	版    本 : V1.3
*	说    明 : 遥控器通信驱动
*
*
*********************************************************************************************************
*/

#include "usart_remote.h"
#include "remote.h"

__align(8) u8 USART3_RX_BUF[2][USART3_REC_LEN]; 
/*
*********************************************************************************************************
*	函 数 名: UsartRemoteInit
*	功能说明: 遥控器通信初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void UsartRemoteInit(void)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
 	USART_DeInit(USART3);  
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = 100000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx ;	
	USART_Init(USART3, &USART_InitStructure); 
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
	
	DMA_DeInit(DMA1_Stream1);
	while (DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART3_RX_BUF[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
	DMA_InitStructure.DMA_BufferSize = sizeof(USART3_RX_BUF)/2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream1, &DMA_InitStructure);
	
	DMA_DoubleBufferModeConfig(DMA1_Stream1,(uint32_t)&USART3_RX_BUF[1][0],DMA_Memory_0);
	DMA_DoubleBufferModeCmd(DMA1_Stream1,ENABLE);
	DMA_Cmd(DMA1_Stream1, ENABLE);  
		
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART3, ENABLE);              
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
	u8 len=0;
	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
	{
		USART3->SR;
		USART3->DR;
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
		{
			DMA_Cmd(DMA1_Stream1, DISABLE); 
			len = USART3_REC_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)USART3_REC_LEN;     
			DMA1_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                 
			DMA1_Stream1->M0AR=(uint32_t)&USART3_RX_BUF[1][0];
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(len == RC_FRAME_LEN)
			{
				RemoteDataPrcess(USART3_RX_BUF[1]);
			  //LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_RC));
			}
		}
		else
		{
			DMA_Cmd(DMA1_Stream1, DISABLE); 
			len = USART3_REC_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)USART3_REC_LEN;      
			DMA1_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  
			DMA1_Stream1->M1AR=(uint32_t)&USART3_RX_BUF[0][0];
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(len == RC_FRAME_LEN)
			{
				RemoteDataPrcess(USART3_RX_BUF[1]);
			 // LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_RC));
			}
		}  
	}
}  
