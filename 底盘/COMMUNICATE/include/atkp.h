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
#include <stdint.h>
#include <stdbool.h>

/*����֡ͷ*/
#define UP_BYTE1 0xAA
#define UP_BYTE2 0xAA

/*����֡ͷ*/
#define DOWN_BYTE1 0xAA
#define DOWN_BYTE2 0xAF

#define ATKP_MAX_DATA_SIZE 55

/*ͨѶ���ݽṹ*/
typedef struct
{
	uint8_t msgID;
	uint8_t dataLen;
	uint8_t data[ATKP_MAX_DATA_SIZE];
}atkp_t;

#define  DATA_BLUETOOTH  0x01

#define MAIN_BOARD       0x02

typedef enum 
{
	D_COMMAND_ARM_ROUTE_BIAS   		=0x01,
  D_COMMAND_ARM_ROUTE	        	=0x02,
  D_COMMAND_ARM_COORD        		=0x03,
  D_COMMAND_ARM_MAGBIAS      		=0x04,
  D_COMMAND_ARM_LOCAVPT      		=0x05,
  D_COMMAND_ARM_PID      		    =0x06,
  D_COMMAND_ARM_REMOTESPEED     =0x07,
  D_COMMAND_ARM_AUTOSPEED      	=0x08,
	D_COMMAND_ARM_DETECTSPEED     =0x09
}downarmmsgID_e;

typedef enum 
{
	D_COMMAND_SUB_HDATA   	    	=0x01,
}downsubmsgID_e;

typedef enum 
{
	D_COMMAND_SUB_BLUETOOTH         =0x01,
	D_COMMAND_SUB_ALARM             =0x02,
	D_COMMAND_SUB_TAIL_LIGHT	      =0x03,
	D_COMMAND_SUB_BRAKE_LIGHT       =0x04,
	D_COMMAND_SUB_TURN_LEFT_LIGHT   =0x05,
	D_COMMAND_SUB_TURN_RIGHT_LIGHT	=0x06,
}upsubmsgID_e;;

#define  D_ACK_READ_PID				0x01
#define  D_ACK_READ_VERSION			0xA0
#define  D_ACK_RESET_PARAM			0xA1

typedef enum 
{
	DOWN_ARM      = 0x01,
	DOWN_SUB    	= 0x03,
	DOWN_POWER    =0xA0
}downmsgID_e;

void atkpInit(void);
bool atkpReceivePacketBlocking(atkp_t *p);
void atkpRxAnlTask(void *param);
void atkpTxTask(void *param);
void requestRoutesData(void);
void requestBaseLocation(void);	
#endif /*ATKP_H*/

