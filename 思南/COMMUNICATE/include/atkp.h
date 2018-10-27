/*
*********************************************************************************************************
*	                                  
*	ģ������ : �����µװ�����
*	�ļ����� : atkp.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef __ATKP_H
#define __ATKP_H
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

/*����֡ͷ*/
#define UP_BYTE1 0xAA
#define UP_BYTE2 0xAF

/*����֡ͷ*/
#define DOWN_BYTE1 0xAA
#define DOWN_BYTE2 0xAA

#define ATKP_MAX_DATA_SIZE 30

#define SUB_BOARD 0x03

/*ͨѶ���ݽṹ*/
typedef struct
{
	uint8_t msgID;
	uint8_t dataLen;
	uint8_t data[ATKP_MAX_DATA_SIZE];
}atkp_t;

/*����ָ��ID*/
typedef enum 
{
	UP_HDATA	  = 0x01,
}upmsgID_e;

/*����ָ��ID*/
typedef enum 
{
	D_COMMAND_BLUETOOTH		       =0x01,
  D_COMMAND_ALARM    		       =0x02,
  D_COMMAND_TAIL_LIGHT		     =0x03,
  D_COMMAND_BRAKE_LIGHT        =0x04,
  D_COMMAND_TURN_LEFT_LIGHT    =0x05,
  D_COMMAND_TURN_RIGHT_LIGHT	 =0x06,
}downsubmsgID_e;


#define  D_ACK_READ_PID				0x01
#define  D_ACK_READ_VERSION			0xA0
#define  D_ACK_RESET_PARAM			0xA1
/*����ָ��ID*/
typedef enum 
{
	DOWN_ARM      = 0x01,
	DOWN_MAIN    	= 0x02,
}downmsgID_e;

void radiolinkDataProcessTask(void *param);
bool atkpReceivePacketBlocking(atkp_t *p);
void atkpInit(void);
void atkpRxAnlTask(void *param);
void Show_Msg(void);
void Gps_Show(void);
#endif
