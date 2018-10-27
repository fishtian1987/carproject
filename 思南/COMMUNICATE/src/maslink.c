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

#define RADIOLINK_TX_QUEUE_SIZE  10 

//static enum
//{
//	waitForStartByte1,
//	waitForStartByte2,
//	waitForMsgID,
//	waitForDataLength,
//	waitForData,
//	waitForChksum1,
//}rxState;

static atkp_t txPacket;
static xQueueHandle  txQueue;

/*
*********************************************************************************************************
*	函 数 名: uart3SendPacket
*	功能说明: 打包数据通过串口发送
*	形    参：p,待发送数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void uart3SendPacket(atkp_t *p)
{
	u8 i=0;
	int dataSize;
	u8 cksum = 0;
	char sendBuffer[25];
	
	sendBuffer[0] = UP_BYTE1;
	sendBuffer[1] = UP_BYTE2;
	sendBuffer[2] = p->msgID;
	sendBuffer[3] = p->dataLen;
	memcpy(&sendBuffer[4], p->data, p->dataLen);
	dataSize = p->dataLen + 5;
	for (i=0; i<dataSize-1; i++)
		cksum += sendBuffer[i];
	sendBuffer[dataSize-1] = cksum;
	UART3_SendString(sendBuffer,dataSize);
}

/*
*********************************************************************************************************
*	函 数 名: radiolinkSendPacket
*	功能说明: 发送数据包，无时间阻塞
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
*	函 数 名: radiolinkSendPacketBlocking
*	功能说明: 发送数据包,有100ms时间阻塞
*	形    参：p,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
bool radiolinkSendPacketBlocking(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 100);
}

/*
*********************************************************************************************************
*	函 数 名: maslinkTask
*	功能说明: 发送数据包任务
*	形    参：param,参数
*	返 回 值: 无
*********************************************************************************************************
*/
void maslinkTask(void *param)
{
	while(1)
	{
		if(xQueueReceive(txQueue, &txPacket, portMAX_DELAY) == pdTRUE)
		{
			ASSERT(txPacket.dataLen <= ATKP_MAX_DATA_SIZE);
			uart3SendPacket(&txPacket);
		}
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
