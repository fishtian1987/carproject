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
#include "combug.h"

#define RADIOLINK_TX_QUEUE_SIZE  10 

static atkp_t txPacket;
static atkp_t rxPacket;
static xQueueHandle  txQueue;

/*
*********************************************************************************************************
*	�� �� ��: uartSendPacket
*	����˵��: �������ͨ�����ڷ���
*	��    �Σ�p,���������ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: atkpPacketDispatch
*	����˵��: Ԥ�������ݰ�
*	��    �Σ�rxPacket,���ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: radiolinkSendPacket
*	����˵��: �������ݰ�
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
*	�� �� ��: maslinkTask
*	����˵��: �������ݰ�����
*	��    �Σ�param,����
*	�� �� ֵ: ��
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
						rxState = (c > 0) ? waitForData : waitForChksum1;	/*c=0,���ݳ���Ϊ0��У��1*/
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
