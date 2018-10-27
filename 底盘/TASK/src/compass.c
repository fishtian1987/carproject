/*
*********************************************************************************************************
*	                                  
*	模块名称 : 电子罗盘数据解析
*	文件名称 : compass.c
*	版    本 : V1.3
*	说    明 : 电子罗盘数据解析
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
*	函 数 名: MAG_PRH_Analysis
*	功能说明: 解析航向角
*	形    参：heading:航向角
*	返 回 值: 无
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
*	函 数 名: Mag_Show
*	功能说明: 显示电子罗盘解算后的数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Mag_Show(void)
{
	char str[10];
	
	sprintf(str,"%.2f  ",magData.raw_value/100.0);
	LCD_ShowString(60,150,65,24,24,(u8*)str);
}	
