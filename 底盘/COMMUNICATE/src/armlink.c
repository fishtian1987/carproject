/*
*********************************************************************************************************
*	                                  
*	ģ������ : arm���ݽ����뷢��
*	�ļ����� : armlink.c
*	��    �� : V1.3
*	˵    �� : arm���ݽ����뷢��
*
*
*********************************************************************************************************
*/

#include "armlink.h"
#include "uart_arm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "combug.h"
#include "uart_debug.h"

#define ARMLINK_TX_QUEUE_SIZE  10 

static atkp_t txPacket;
static atkp_t rxPacket;
static xQueueHandle  txQueue;

/*
*********************************************************************************************************
*	�� �� ��: uart4SendPacket
*	����˵��: �������ͨ�����ڷ���
*	��    �Σ�p,���������ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void uart4SendPacket(atkp_t *p)
{
	u8 i=0;
	int dataSize;
	u8 cksum = 0;
	char sendBuffer[40];
	
	sendBuffer[0] = DOWN_BYTE1;
	sendBuffer[1] = DOWN_BYTE2;
	sendBuffer[2] = p->msgID;
	sendBuffer[3] = p->dataLen;
	memcpy(&sendBuffer[4], p->data, p->dataLen);
	dataSize = p->dataLen + 5;
	for (i=0; i<dataSize-1; i++)
		cksum += sendBuffer[i];
	sendBuffer[dataSize-1] = cksum;
	UART4_SendString(sendBuffer,dataSize);
	//UART1_SendString(sendBuffer,dataSize);
}

/*
*********************************************************************************************************
*	�� �� ��: armlinkSendPacket
*	����˵��: �������ݰ�
*	��    �Σ�p,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
bool armlinkSendPacket(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: armlinkSendPacketBlocking
*	����˵��: �������ݰ�,��100msʱ������
*	��    �Σ�p,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
bool armlinkSendPacketBlocking(const atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(txQueue, p, 100);
}

/*
*********************************************************************************************************
*	�� �� ��: armlinkReciveTask
*	����˵��: �������ݰ�����
*	��    �Σ�param,���� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void armlinkReciveTask(void *param)
{
	rxState = waitForStartByte1;
	
	u8 c;
	u8 dataIndex = 0;
	u8 cksum = 0;

	while(1)
	{
		if (uartArmGetDataWithTimout(&c))
		{
			switch(rxState)
			{
				case waitForStartByte1:
				{
					rxState = (c == UP_BYTE1) ? waitForStartByte2 : waitForStartByte1;
					cksum = c;
				}break;
				case waitForStartByte2:
				{
					rxState = (c == UP_BYTE2) ? waitForMsgID : waitForStartByte1;
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
						atkpReceivePacketBlocking(&rxPacket);
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
*	�� �� ��: maslinkTask
*	����˵��: �������ݰ�����
*	��    �Σ�param,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void armlinkTask(void *param)
{
	while(1)
	{
		if(xQueueReceive(txQueue, &txPacket, portMAX_DELAY) == pdTRUE)
		{
			ASSERT(txPacket.dataLen <= ATKP_MAX_DATA_SIZE);
			uart4SendPacket(&txPacket);
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: armlinkInit
*	����˵��: �����������ݶ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void armlinkInit(void)
{
	uartArmInit();
	txQueue = xQueueCreate(ARMLINK_TX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(txQueue);
}
