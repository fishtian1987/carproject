/*
*********************************************************************************************************
*	                                  
*	ģ������ : �����ϵװ�����
*	�ļ����� : atkp.c
*	��    �� : V1.3
*	˵    �� : Ӳ����ʼ��ģ��
*
*
*********************************************************************************************************
*/

#include "atkp.h"
#include "uart_syslink.h"
#include "stdio.h"
#include "string.h"
#include "gps.h"
#include "hmi.h"
#include "relay.h" 
#include "string.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "maslink.h"
#include "relay.h"
#include "led.h"

#define  BYTE0(dwTemp)       ( *( (u8 *)(&dwTemp)))
#define  BYTE1(dwTemp)       ( *( (u8 *)(&dwTemp) + 1) )
#define  BYTE2(dwTemp)       ( *( (u8 *)(&dwTemp) + 2) )
#define  BYTE3(dwTemp)       ( *( (u8 *)(&dwTemp) + 3) )

#define ATKP_RX_QUEUE_SIZE 	10 /*ATKP�����ն�����Ϣ����*/

static xQueueHandle rxQueue;
static atkp_t rxPacket;

/*
*********************************************************************************************************
*	��������: armnReceiveAnl
*	����˵��: ����arm���ݰ�
*	��    ��: packet,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void armnReceiveAnl(atkp_t *packet)
{
	
}

/*
*********************************************************************************************************
*	��������: mainReceiveAnl
*	����˵��: ����arm���ݰ�
*	��    ��: packet,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void mainReceiveAnl(atkp_t *packet)
{
	switch(packet->data[0])
	{
		case D_COMMAND_ALARM:
		{
			switch_alarm(packet->data+1);
		}break;
		case D_COMMAND_TAIL_LIGHT:
		{
			switch_tail_light(packet->data+1);
		}break;
		case D_COMMAND_BRAKE_LIGHT:
		{
			switch_brake_light(packet->data+1);
		}break;
		case D_COMMAND_TURN_LEFT_LIGHT:
		{
			switch_left_light(packet->data+1);
		}break;
		case D_COMMAND_TURN_RIGHT_LIGHT:
		{
			switch_right_light(packet->data+1);
		}break;
		default:break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: atkpReceiveAnl
*	����˵��: �������ݰ�
*	��    �Σ�anlPacket,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void atkpReceiveAnl(atkp_t *anlPacket)
{
	if(anlPacket->msgID	== DOWN_ARM)
		armnReceiveAnl(anlPacket);
	else if(anlPacket->msgID == DOWN_MAIN)
		mainReceiveAnl(anlPacket);	
} 

/*
*********************************************************************************************************
*	��������: atkpRxAnlTask
*	����˵��: �ȴ��������ݰ�
*	��    ��: param,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void atkpRxAnlTask(void *param)
{
	atkp_t p;
	while(1)
	{
		xQueueReceive(rxQueue, &p, portMAX_DELAY);
		atkpReceiveAnl(&p);
	}
}

/*
*********************************************************************************************************
*	��������: atkpInit
*	����˵��: �����������ݶ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void atkpInit(void)
{
	rxQueue = xQueueCreate(ATKP_RX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(rxQueue);
}

/*
*********************************************************************************************************
*	��������: atkpReceivePacketBlocking
*	����˵��: ���ͽ������ݰ�����
*	��    ��: p,���ݰ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
bool atkpReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(rxQueue, p, portMAX_DELAY);	
}

/*
*********************************************************************************************************
*	�� �� ��: radiolinkDataProcessTask
*	����˵��: ���ݴ�������
*	��    �Σ�param,�β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void radiolinkDataProcessTask(void *param) 
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
*	�� �� ��: Show_Msg
*	����˵��: ��ʾ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Show_Msg(void)
{
	sprintf(USART6_TX_BUF,"t1.txt=\"%.8f\"",gpsx.latitude);//γ��
	HMISends(USART6_TX_BUF);
	sprintf(USART6_TX_BUF,"t3.txt=\"%.8f\"",gpsx.longitude);//����
	HMISends(USART6_TX_BUF);
	switch (gpsx.gpssta)//GPS״̬
	{
		case 0:
		{
			sprintf(USART6_TX_BUF,"t5.txt=\"%s\"","��Ч��   ");
			HMISends(USART6_TX_BUF);
		}break;
		case 1:
		{
			sprintf(USART6_TX_BUF,"t5.txt=\"%s\"","���㶨λ��");
			HMISends(USART6_TX_BUF);
		}break;
		case 2:
		{
			sprintf(USART6_TX_BUF,"t5.txt=\"%s\"","α����  ");
			HMISends(USART6_TX_BUF);
		}break;
		case 4:
		{
			sprintf(USART6_TX_BUF,"t5.txt=\"%s\"","�̶���    ");
			HMISends(USART6_TX_BUF);
		}break;
		case 5:
		{
			sprintf(USART6_TX_BUF,"t5.txt=\"%s\"","������    ");
			HMISends(USART6_TX_BUF);
		}break;
		default:
			break;
	}
	
	sprintf(USART6_TX_BUF,"t7.txt=\"%0.2f\"",gpsx.heading);//�����
	HMISends(USART6_TX_BUF);
	sprintf(USART6_TX_BUF,"t9.txt=\"%0.2f\"",gpsx.speed);//�ٶ�
	HMISends(USART6_TX_BUF);
	sprintf(USART6_TX_BUF,"t11.txt=\"%d\"",gpsx.posslnum);
	HMISends(USART6_TX_BUF);
}	 
