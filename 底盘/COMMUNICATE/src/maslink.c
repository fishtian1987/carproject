/*
*********************************************************************************************************
*	                                  
*	模块名称 : 校验数据
*	文件名称 : maslink.c
*	版    本 : V1.3
*	说    明 : 校验数据
*
*
*********************************************************************************************************
*/

#include "maslink.h"
#include "uart_syslink.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "combug.h"

#define RADIOLINK_TX_QUEUE_SIZE  10 

static atkp_t txPacket;
static atkp_t rxPacket;
static xQueueHandle  txQueue;

/*
*********************************************************************************************************
*	函 数 名: uartSendPacket
*	功能说明: 打包数据通过串口发送
*	形    参：p,待发送数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void uartSendPacket(atkp_t *p)
{
	int dataSize;
	u8 cksum = 0;
	char sendBuffer[36];
	
	sendBuffer[0] = UP_BYTE1;
	sendBuffer[1] = UP_BYTE2;
	sendBuffer[2] = p->msgID;
	sendBuffer[3] = p->dataLen;
	
	memcpy(&sendBuffer[4], p->data, p->dataLen);
	dataSize = p->dataLen + 5;
	for (int i=0; i<dataSize-1; i++)
		cksum += sendBuffer[i];
	sendBuffer[dataSize-1] = cksum;
	UART2_SendString(sendBuffer,dataSize);
}

/*
*********************************************************************************************************
*	函 数 名: atkpPacketDispatch
*	功能说明: 预处理数据包
*	形    参：rxPacket,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void atkpPacketDispatch(atkp_t *rxPacket)
{
	atkpReceivePacketBlocking(rxPacket);
	
	if( rxPacket->msgID == DOWN_POWER){;}
	else
	{
		if(xQueueReceive(txQueue, &txPacket, 0) == pdTRUE)
		{
			ASSERT(txPacket.dataLen <= ATKP_MAX_DATA_SIZE);
			uartSendPacket(&txPacket);
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: radiolinkSendPacket
*	功能说明: 发送数据包
*	形    参：p,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
bool radiolinkSendPacket(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 0);
}

/*
*********************************************************************************************************
*	函 数 名: maslinkTask
*	功能说明: 接收数据包任务
*	形    参：param,参数
*	返 回 值: 无
*********************************************************************************************************
*/
void maslinkTask(void *param)
{
	rxState = waitForStartByte1;
	
	u8 c;
	u8 dataIndex = 0;
	u8 cksum = 0;

	while(1)
	{
		if (uartslkGetDataWithTimout(&c))
		{
			switch(rxState)
			{
				case waitForStartByte1:
				{
					rxState = (c == DOWN_BYTE1) ? waitForStartByte2 : waitForStartByte1;
					cksum = c;
				}break;
				case waitForStartByte2:
				{
					rxState = (c == DOWN_BYTE2) ? waitForMsgID : waitForStartByte1;
					cksum += c;
				}break;
				case waitForMsgID:
				{
					rxPacket.msgID = c;
					rxState = waitForDataLength;
					cksum += c;
				}break;
				case waitForDataLength:
				{
					if (c <= ATKP_MAX_DATA_SIZE)
					{
						rxPacket.dataLen = c;
						dataIndex = 0;
						rxState = (c > 0) ? waitForData : waitForChksum1;	/*c=0,数据长度为0，校验1*/
						cksum += c;
					} else rxState = waitForStartByte1;
				}break;
				case waitForData:
				{
					rxPacket.data[dataIndex++] = c;
					cksum += c;
					if (dataIndex == rxPacket.dataLen)
						rxState = waitForChksum1;
				}break;
				case waitForChksum1:
				{
					if (cksum == c)	
						atkpPacketDispatch(&rxPacket);
					else
					{
						rxState = waitForStartByte1;	
						IF_DEBUG_ASSERT(1);
					}
					rxState = waitForStartByte1;
				}break;
				default:
					ASSERT(0);
					break;
			}
		}
		else
			rxState = waitForStartByte1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: maslinkInit
*	功能说明: 创建接收数据队列
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void maslinkInit(void)
{
	uartslkInit();
	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(txQueue);
}
