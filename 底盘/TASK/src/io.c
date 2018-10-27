/*
*********************************************************************************************************
*	                                  
*	模块名称 : 数据显示和存储
*	文件名称 : io.c
*	版    本 : V1.3
*	说    明 : 数据显示和存储
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
*	函 数 名: path_name
*	功能说明: 返回完整路径名
*	形    参：pname,待返回路径
			    	path,路径
						name，文件名
*	返 回 值: 无
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
*	函 数 名: AppParamSave
*	功能说明: 保存校准值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AppParamSave(void)
{
	
}

/*
*********************************************************************************************************
*	函 数 名: Is_AppParam_Calied
*	功能说明: 用于在SuperviseTask中设置错误标志位
*	形    参：无
*	返 回 值: 错误标志位
*********************************************************************************************************
*/
uint8_t Is_AppParam_Calied(void)
{
	return app_param_calied_flag; 
}

/*
*********************************************************************************************************
*	函 数 名: SetCaliCmdFlag
*	功能说明: 设置校准标志位
*	形    参：flag,标志位
*	返 回 值: 无
*********************************************************************************************************
*/
void SetCaliCmdFlag(uint16_t flag)  
{
	CaliCmdFlagGrp |= flag;
}

/*
*********************************************************************************************************
*	函 数 名: ResetCaliCmdFlag
*	功能说明: 取消校准标志位
*	形    参：flag,标志位
*	返 回 值: 无
*********************************************************************************************************
*/
void ResetCaliCmdFlag(uint16_t flag)
{
	CaliCmdFlagGrp &= ~flag;
}

/*
*********************************************************************************************************
*	函 数 名: GetCaliCmdFlagGrp
*	功能说明: 查询校准标志位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t GetCaliCmdFlagGrp(void)
{
	return CaliCmdFlagGrp;
}

/*
*********************************************************************************************************
*	函 数 名: IsCaliCmdFlagSet
*	功能说明: 查询某一位校准标志位
*	形    参：flag,待查询标志位
*	返 回 值: 0,未设置
						1,设置
*********************************************************************************************************
*/
bool IsCaliCmdFlagSet(uint16_t flag)
{
	if(flag & CaliCmdFlagGrp)return true;
	else return false;	
}

/*
*********************************************************************************************************
*	函 数 名: CalibrateLoop
*	功能说明: 设置校准标志位
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: ShowInformation
*	功能说明: 显示信息
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: SaveInformation
*	功能说明: 存储信息
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: ioTask
*	功能说明: 显示和存储信息
*	形    参：无
*	返 回 值: 无
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
