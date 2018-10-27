/*
*********************************************************************************************************
*	                                  
*	模块名称 : 与上主板通信
*	文件名称 : trandata.c
*	版    本 : V1.3
*	说    明 : 与上主板通信
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
*	函 数 名: sendRmotorData
*	功能说明: 发送控制数据
*	形    参：data,数据
						len,长度
*	返 回 值: 无
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
*	函 数 名: commanderTask
*	功能说明: 发送数据任务
*	形    参：param,形参
*	返 回 值: 无
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
		//发送数据
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

