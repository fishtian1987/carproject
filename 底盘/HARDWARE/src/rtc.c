/*
*********************************************************************************************************
*	                                  
*	ģ������ : RTCʱ�ӹ���
*	�ļ����� : rtc.c
*	��    �� : V1.3
*	˵    �� : RTCʱ�ӹ���
*
*
*********************************************************************************************************
*/

#include "rtc.h"
#include "delay.h"

/*
*********************************************************************************************************
*	�� �� ��: RTC_Set_Time
*	����˵��: RTCʱ������
*	��    �Σ�hour,min,sec:Сʱ,����,����
						ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
*	�� �� ֵ: SUCEE(1),�ɹ�
						ERROR(0),�����ʼ��ģʽʧ�� 
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
*	�� �� ��: RTC_Set_Date
*	����˵��: RTC��������
*	��    �Σ�year,month,date:��(0~99),��(1~12),��(0~31)
						week:����(1~7,0,�Ƿ�!)
*	�� �� ֵ: SUCEE(1),�ɹ�
						ERROR(0),�����ʼ��ģʽʧ�� 
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
*	�� �� ��: My_RTC_Init
*	����˵��: RTC��ʼ��
*	��    �Σ�year,month,date:��(0~99),��(1~12),��(0~31)
						week:����(1~7,0,�Ƿ�!)
*	�� �� ֵ: 0,��ʼ���ɹ�
				    1,LSE����ʧ��
				    2,�����ʼ��ģʽʧ��
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
