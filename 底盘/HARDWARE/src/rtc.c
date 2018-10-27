/*
*********************************************************************************************************
*	                                  
*	模块名称 : RTC时钟管理
*	文件名称 : rtc.c
*	版    本 : V1.3
*	说    明 : RTC时钟管理
*
*
*********************************************************************************************************
*/

#include "rtc.h"
#include "delay.h"

/*
*********************************************************************************************************
*	函 数 名: RTC_Set_Time
*	功能说明: RTC时间设置
*	形    参：hour,min,sec:小时,分钟,秒钟
						ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
*	返 回 值: SUCEE(1),成功
						ERROR(0),进入初始化模式失败 
*********************************************************************************************************
*/
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);	
}

/*
*********************************************************************************************************
*	函 数 名: RTC_Set_Date
*	功能说明: RTC日期设置
*	形    参：year,month,date:年(0~99),月(1~12),日(0~31)
						week:星期(1~7,0,非法!)
*	返 回 值: SUCEE(1),成功
						ERROR(0),进入初始化模式失败 
*********************************************************************************************************
*/
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: My_RTC_Init
*	功能说明: RTC初始化
*	形    参：year,month,date:年(0~99),月(1~12),日(0~31)
						week:星期(1~7,0,非法!)
*	返 回 值: 0,初始化成功
				    1,LSE开启失败
				    2,进入初始化模式失败
*********************************************************************************************************
*/
void My_RTC_Init(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);	
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5000)
	{
		RCC_LSEConfig(RCC_LSE_ON);   
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			retry++;
			delay_ms(10);
		}
		if(retry==0)return;					
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		
		RCC_RTCCLKCmd(ENABLE);
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure); 
		RTC_Set_Time(22,38,0,RTC_H12_AM);	
		RTC_Set_Date(17,9,21,4);	 
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5000);
	} 
}
