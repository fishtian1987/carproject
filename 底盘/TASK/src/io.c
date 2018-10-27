/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ʾ�ʹ洢
*	�ļ����� : io.c
*	��    �� : V1.3
*	˵    �� : ������ʾ�ʹ洢
*
*
*********************************************************************************************************
*/

#include "FreeRTOS.h"
#include "task.h"
#include "io.h"	
#include "remote.h"
#include "stdio.h"
#include "stabilizer.h"
#include "lcd.h"
#include "text.h"
#include "string.h"
#include "gps.h"
#include "compass.h"
#include "combug.h"
#include "config_param.h"
#include "led.h"

static uint16_t CaliCmdFlagGrp = 0; 

uint8_t app_param_calied_flag = 0;
char *filename=0;
RTC_TimeTypeDef RTC_TimeStruct={0};
RTC_DateTypeDef RTC_DateStruct={0};
FIL fil;
FRESULT res;
UINT bww;

/*
*********************************************************************************************************
*	�� �� ��: path_name
*	����˵��: ��������·����
*	��    �Σ�pname,������·��
			    	path,·��
						name���ļ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void path_name(char *pname,char* path,char *name)
{
	const char chgchar[2]={0X2F,0X00};
	strcpy(pname,path);	
	strcat(pname,chgchar);	
	strcat(pname,name);	
}

/*
*********************************************************************************************************
*	�� �� ��: AppParamSave
*	����˵��: ����У׼ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AppParamSave(void)
{
	
}

/*
*********************************************************************************************************
*	�� �� ��: Is_AppParam_Calied
*	����˵��: ������SuperviseTask�����ô����־λ
*	��    �Σ���
*	�� �� ֵ: �����־λ
*********************************************************************************************************
*/
uint8_t Is_AppParam_Calied(void)
{
	return app_param_calied_flag; 
}

/*
*********************************************************************************************************
*	�� �� ��: SetCaliCmdFlag
*	����˵��: ����У׼��־λ
*	��    �Σ�flag,��־λ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SetCaliCmdFlag(uint16_t flag)  
{
	CaliCmdFlagGrp |= flag;
}

/*
*********************************************************************************************************
*	�� �� ��: ResetCaliCmdFlag
*	����˵��: ȡ��У׼��־λ
*	��    �Σ�flag,��־λ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResetCaliCmdFlag(uint16_t flag)
{
	CaliCmdFlagGrp &= ~flag;
}

/*
*********************************************************************************************************
*	�� �� ��: GetCaliCmdFlagGrp
*	����˵��: ��ѯУ׼��־λ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t GetCaliCmdFlagGrp(void)
{
	return CaliCmdFlagGrp;
}

/*
*********************************************************************************************************
*	�� �� ��: IsCaliCmdFlagSet
*	����˵��: ��ѯĳһλУ׼��־λ
*	��    �Σ�flag,����ѯ��־λ
*	�� �� ֵ: 0,δ����
						1,����
*********************************************************************************************************
*/
bool IsCaliCmdFlagSet(uint16_t flag)
{
	if(flag & CaliCmdFlagGrp)return true;
	else return false;	
}

/*
*********************************************************************************************************
*	�� �� ��: CalibrateLoop
*	����˵��: ����У׼��־λ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CalibrateLoop(void)
{
	if(IsCaliCmdFlagSet(GPS_FLAG_HEADOffset))
	{
		GPSHeadOffset_Show();
		ResetCaliCmdFlag(GPS_FLAG_HEADOffset);	
	}
	else if(IsCaliCmdFlagSet(RANGE_FLAG_GPS))
	{
		GPSBiasRange_Show();
		ResetCaliCmdFlag(RANGE_FLAG_GPS);
	}
	else if(IsCaliCmdFlagSet(PID_FLAG_MUX))
	{
		PID_Show();
		ResetCaliCmdFlag(PID_FLAG_MUX);
	}
	else if(IsCaliCmdFlagSet(FACTOR_FLAG_REMOTE))
	{
		RemoteFactor_Show();
		ResetCaliCmdFlag(FACTOR_FLAG_REMOTE);
	}
	else if(IsCaliCmdFlagSet(SPEED_FLAG_AUTO))
	{
		AutoSpeed_Show();
		ResetCaliCmdFlag(SPEED_FLAG_AUTO);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ShowInformation
*	����˵��: ��ʾ��Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ShowInformation(void)
{
	
	Speed_Show();
	Gps_Show();
	Mag_Show();
	OperatingMode();
}

/*
*********************************************************************************************************
*	�� �� ��: SaveInformation
*	����˵��: �洢��Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SaveInformation(void)
{
	if(gpsx.gpssta)
	{
		char *tbuf=0;
		tbuf=pvPortMalloc(INFOSIZE);
		if(tbuf==NULL)	
			return;
		
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		sprintf(tbuf,"%02d:%02d:%02d %f %f %d\r\n",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds,gpsx.latitude/1000000.0,gpsx.longitude/1000000.0,gpsx.gpssta); 
		if(f_open (&fil,filename, FA_OPEN_ALWAYS|FA_WRITE)!=0)return; 
		f_lseek(&fil,fil.fsize);
		f_write (&fil, tbuf, 33, &bww);
		f_close(&fil);
		vPortFree(tbuf);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ioTask
*	����˵��: ��ʾ�ʹ洢��Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ioTask(void* param)
{
	//u8 i=0;
	while(1)
	{
		ShowInformation();
		CalibrateLoop();
//		if(i>=15)
//		{
//			SaveInformation();
//			i=0;
//		}
//		i++;
		LED3=!LED3;
		vTaskDelay(100);
	}
}
