/*
*********************************************************************************************************
*	                                  
*	模块名称 : 电子罗盘通信驱动
*	文件名称 : uart_mag.c
*	版    本 : V1.3
*	说    明 : 电子罗盘通信驱动
*
*
*********************************************************************************************************
*/


#include "uart_mag.h"
#include "atkp.h"
#include "compass.h"

static enum
{
	waitForStartByte1,
	waitForDataLength,
	waitForAddr,
	waitForMsgID,
	waitForData,
	waitForChksum1,
}rxState;

/*
*********************************************************************************************************
*	函 数 名: UsartCompassInit
*	功能说明: 电子罗盘通信初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void UsartCompassInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	rxState = waitForStartByte1;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure); 
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
	USART_Cmd(USART6, ENABLE); 
}

atkp_t rxMagPacket;
/*
*********************************************************************************************************
*	函 数 名: USART6_IRQHandler
*	功能说明: UART6中断服务函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void USART6_IRQHandler(void) 
{
	static u8 dataIndex = 0;
	static u8 cksum = 0;
	u8 Res=0;
	if((USART6->SR & (1<<5)) != 0) 
	{
		Res =USART6->DR & 0xFF;
		switch(rxState)
		{
			case waitForStartByte1:
			{
				rxState = (Res == DOWN6_BYTE) ? waitForDataLength : waitForStartByte1;	
			}break;
			case waitForDataLength:
			{
				if(Res<USART6_REC_LEN)
				{
					rxMagPacket.dataLen = Res;
					dataIndex = 0;
					rxState = waitForAddr;
					cksum = Res;
				} else rxState = waitForStartByte1;
			}break;
			case waitForAddr:
			{
				rxState = (Res == DOWN6_ADDR) ? waitForMsgID : waitForStartByte1;	
				cksum += Res;
			}break;
			case waitForMsgID:
			{
				rxMagPacket.msgID = Res;
				rxState = waitForData;	
				cksum += Res;
			}break;
			case waitForData:
			{
				rxMagPacket.data[dataIndex++] = Res;
				cksum += Res;
				if(dataIndex == rxMagPacket.dataLen-4)
				{
					rxMagPacket.dataLen-=4;
					rxState = waitForChksum1;
				}
			}break;
			case waitForChksum1:
			{
				if(cksum == Res)	
					MAG_Analysis(&rxMagPacket);
				rxState = waitForStartByte1;
			}break;
			default:
			{
				rxState = waitForStartByte1;
			}break;
		}	
	}
}

/*
*********************************************************************************************************
*	函 数 名: UART6_SendString
*	功能说明: USART6发送字符串
*	形    参：str:待发送字符串指针
            size:大小
*	返 回 值: 无
*********************************************************************************************************
*/
void UART6_SendString(u8* str,u8 size)
{
	u8 i=0;
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(USART6, USART_FLAG_TC)==RESET); 
		USART_SendData(USART6,*(str+i));
	}
}
