/*
*********************************************************************************************************
*	                                  
*	ģ������ : �����������ݽ���
*	�ļ����� : compass.c
*	��    �� : V1.3
*	˵    �� : �����������ݽ���
*
*
*********************************************************************************************************
*/

#include "compass.h"
#include "lcd.h"
#include "text.h"	
#include "stdio.h"
#include "io.h"	
#include "config_param.h"
#include "gps.h"	

mag_t magData={0,0,0,0,0,0};

/*
*********************************************************************************************************
*	�� �� ��: MAG_PRH_Analysis
*	����˵��: ���������
*	��    �Σ�heading:�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MAG_PRH_Analysis(u16 heading)
{
	magData.last_raw_value=magData.raw_value;
	magData.raw_value=heading;
	magData.diff=magData.raw_value-magData.last_raw_value;
	if(magData.diff>33000)
		magData.round_cnt--;
	else if(magData.diff < -33000)
		magData.round_cnt++;
	magData.ecd_angle=magData.raw_value-magData.ecd_bias+ magData.round_cnt * 36000;
}

/*
*********************************************************************************************************
*	�� �� ��: Mag_Show
*	����˵��: ��ʾ�������̽���������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Mag_Show(void)
{
	char str[10];
	
	sprintf(str,"%.2f  ",magData.raw_value/100.0);
	LCD_ShowString(60,150,65,24,24,(u8*)str);
}	
