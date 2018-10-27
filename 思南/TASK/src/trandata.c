/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ͨ��
*	�ļ����� : trandata.c
*	��    �� : V1.3
*	˵    �� : ��������ͨ��
*
*
*********************************************************************************************************
*/

#include "trandata.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gps.h" 	
#include "atkp.h" 	
#include "string.h" 
#include "maslink.h"
#include "led.h"

/*
*********************************************************************************************************
*	�� �� ��: sendRmotorData
*	����˵��: ���Ϳ�������
*	��    �Σ�data,����
						len,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sendRmotorData(u8 *data, u8 len)
{
	atkp_t p;
	p.msgID = SUB_BOARD;
	p.data[0]=UP_HDATA;
	memcpy(&p.data[1], data, len);
	p.dataLen = len+1;
	radiolinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	�� �� ��: commanderTask
*	����˵��: ������������
*	��    �Σ�param,�β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void commanderTask(void* param)
{
	tranData_t sendData;
	u8 i=0;
	while(1)
	{
		vTaskDelay(5);
		sendData.latitude=gpsx.latitude;
		sendData.longitude=gpsx.longitude;
		sendData.gpssta=gpsx.gpssta;
		sendData.heading=gpsx.heading*100;
		sendData.speed=gpsx.speed*100;
		//��������
		sendRmotorData((u8*)&sendData, sizeof(sendData));
		i++;
		if(i==20)
		{
			Show_Msg();
			LED0=!LED0;
			i=0;
		}
	}
}

