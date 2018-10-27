/*
*********************************************************************************************************
*	                                  
*	ģ������ : У������
*	�ļ����� : maslink.c
*	��    �� : V1.3
*	˵    �� : У������
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
*	�� �� ��: uart3SendPacket
*	����˵��: �������ͨ�����ڷ���
*	��    �Σ�p,���������ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: radiolinkSendPacket
*	����˵��: �������ݰ�����ʱ������
*	��    �Σ�p,���ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: radiolinkSendPacketBlocking
*	����˵��: �������ݰ�,��100msʱ������
*	��    �Σ�p,���ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: maslinkTask
*	����˵��: �������ݰ�����
*	��    �Σ�param,����
*	�� �� ֵ: ��
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
*	�� �� ��: maslinkInit
*	����˵��: �����������ݶ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void maslinkInit(void)
{
	uartslkInit();
	txQueue = xQueueCreate(RADIOLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(txQueue);
}
