/*
*********************************************************************************************************
*	                                  
*	ģ������ : �����������ݽ���
*	�ļ����� : compass.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef _COMPASS_H
#define _COMPASS_H
#include "sys.h"
#include "atkp.h"

typedef struct  
{										    
	uint16_t raw_value;
	uint16_t last_raw_value;
	int32_t diff;
	uint16_t ecd_bias;
	int32_t ecd_angle;
	int8_t round_cnt;
}mag_t; 

#define MAG_PRH_ID 0x84

extern mag_t magData;

void MAG_PRH_Analysis(u16 heading);
void Mag_Show(void);
#endif
