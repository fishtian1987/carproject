/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���в���ģ��
*	�ļ����� : combug.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef __COMBUG_H
#define __COMBUG_H
#include "sys.h"
#include "stdbool.h"

static enum
{
	waitForStartByte1,
	waitForStartByte2,
	waitForMsgID,
	waitForDataLength,
	waitForData,
	waitForChksum1,
}rxState;

#define UARTSLK_DATA_TIMEOUT_MS 	1000
#define UARTSLK_DATA_TIMEOUT_TICKS (UARTSLK_DATA_TIMEOUT_MS / portTICK_RATE_MS)


//int32_t dataFusion(u8* addr,u8 len);
//void dataDiv(int32_t data,u8 len,int32_t *dataf,int32_t *datab);
int32_t BCD_DataProcess(u8 *addr,u8 len,bool cmd);
#endif
