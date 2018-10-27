/*
*********************************************************************************************************
*	                                  
*	ģ������ : ���ô���ģ��
*	�ļ����� : config_param.c
*	��    �� : V1.3
*	˵    �� : ���ô���ģ��
*
*
*********************************************************************************************************
*/

#include "config_param.h"
#include "config.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "math.h"
#include "delay.h"
#include "io.h"
#include "compass.h"
#include "lcd.h"
#include "beep.h"

#define VERSION 13
#define CONFIG_PARAM_ADDR  FLASH_SIZE - CONFIG_PARAM_SIZE

static TimerHandle_t timer;
configParam_t configParam;

static configParam_t configParamDefault=
{
	.version = VERSION,
	.GpsCali.headOffset=0,
	.GpsCali.gpsBias=1000,
	.PID.veloc.P=60.0f,
	.PID.veloc.I=0.0f,
	.PID.veloc.D=0.0f,
	.PID.rotate.P=1.2,
	.PID.rotate.I=0.0f,
	.PID.rotate.D=0.0f,
	.remoteFator.FBfator=1.5f,
	.remoteFator.LRfator=0.3f,
	.autoSpeed.lowSpeed=200,
	.autoSpeed.middleSpeed=350,
	.autoSpeed.hightSpeed=450,
};

static SemaphoreHandle_t  xSemaphore = NULL;


//static u8 configParamCksum(configParam_t* data)
//{
//	int i;
//	u8 cksum=0;	
//	u8* c = (u8*)data;  	
//	size_t len=sizeof(configParam_t);

//	for (i=0; i<len; i++)
//		cksum += *(c++);
//	cksum-=data->cksum;
//	
//	return cksum;
//}

/*
*********************************************************************************************************
*	�� �� ��: configParamGiveSemaphore
*	����˵��: �������������ź���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void configParamGiveSemaphore(void)
{
	xSemaphoreGive(xSemaphore);		
}

/*
*********************************************************************************************************
*	�� �� ��: timerCallback
*	����˵��: ��ʱ���ص�����
*	��    �Σ�xTimer����ʱ��id
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void timerCallback(TimerHandle_t xTimer)
{
	configParamGiveSemaphore();
}

/*
*********************************************************************************************************
*	�� �� ��: timerStart
*	����˵��: ������ʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void timerStart(void)
{
	xTimerStart(timer,0);
}

/*
*********************************************************************************************************
*	�� �� ��: readParam
*	����˵��: ��ȡ���ò���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void readParam(void)
{
	W25QXX_Read((u8*)&configParam,CONFIG_PARAM_ADDR,sizeof(configParam));		
	GpsCali.headOffset=configParam.GpsCali.headOffset;
	GpsCali.gpsBias=configParam.GpsCali.gpsBias;
	CM1SpeedPID.kp=configParam.PID.veloc.P;
	CM2SpeedPID.kp=configParam.PID.veloc.P;
	CM3SpeedPID.kp=configParam.PID.veloc.P;
	CM4SpeedPID.kp=configParam.PID.veloc.P;
	CM1SpeedPID.ki=configParam.PID.veloc.I;
	CM2SpeedPID.ki=configParam.PID.veloc.I;
	CM3SpeedPID.ki=configParam.PID.veloc.I;
	CM4SpeedPID.ki=configParam.PID.veloc.I;
	CM1SpeedPID.kd=configParam.PID.veloc.D;
	CM2SpeedPID.kd=configParam.PID.veloc.D;
	CM3SpeedPID.kd=configParam.PID.veloc.D;
	CM4SpeedPID.kd=configParam.PID.veloc.D;
	CMRotatePID.kp=configParam.PID.rotate.P;
	CMRotatePID.ki=configParam.PID.rotate.I;
  CMRotatePID.kd=configParam.PID.rotate.D;
	
	remoteFator.FBfator=configParam.remoteFator.FBfator;
	remoteFator.LRfator=configParam.remoteFator.LRfator;
	autoSpeed.lowSpeed=configParam.autoSpeed.lowSpeed;
	autoSpeed.middleSpeed=configParam.autoSpeed.middleSpeed;
	autoSpeed.hightSpeed=configParam.autoSpeed.hightSpeed;
	
}

/*
*********************************************************************************************************
*	�� �� ��: configParamInit
*	����˵��: �������ó�ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void configParamInit(void)	
{
	timer=xTimerCreate("CPTimer",8000,pdFALSE,(void*)1,timerCallback);			  
	configParam=configParamDefault;
	readParam();
	//timerStart();
	
	SetCaliCmdFlag(GPS_FLAG_HEADOffset);
	SetCaliCmdFlag(RANGE_FLAG_GPS);
	SetCaliCmdFlag(PID_FLAG_MUX);
	SetCaliCmdFlag(FACTOR_FLAG_REMOTE);
	SetCaliCmdFlag(SPEED_FLAG_AUTO);

//	if(configParam.version == VERSION)	/*�汾��ȷ*/
//	{
//		if(configParamCksum(&configParam) == configParam.cksum)	/*У����ȷ*/
//		{
//			printf("Version V%1.1f check [OK]\r\n", configParam.version / 10.0f);
//			isConfigParamOK = true;
//		} else
//		{
//			printf("Version check [FAIL]\r\n");
//			isConfigParamOK = false;
//		}
//	}	
//	else	/*�汾����*/
//	{
//		isConfigParamOK = false;
//	}
//	
//	if(isConfigParamOK == false)	/*���ò�������д��Ĭ�ϲ���*/
//	{
//		memcpy((u8 *)&configParam, (u8 *)&configParamDefault, sizeof(configParam));
//		configParam.cksum = configParamCksum(&configParam);				/*����У��ֵ*/
//		STMFLASH_Write(CONFIG_PARAM_ADDR,(u32 *)&configParam, lenth);	/*д��stm32 flash*/
//		isConfigParamOK=true;
//	}	
//	
	xSemaphore = xSemaphoreCreateBinary();
//	
//	isInit=true;
}

/*
*********************************************************************************************************
*	�� �� ��: configParamTask
*	����˵��: ������������
*	��    �Σ�param,�β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void configParamTask(void* param)
{
	u8 cksum = 0;
	
	while(1) 
	{	
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		//cksum = configParamCksum(&configParam);		/*����У��*/
		
		//if(configParam.cksum != cksum)	
		//{
			//configParam.cksum = cksum;	/*����У��*/
		//	watchdogInit(1000);			/*����ʱ��Ƚϳ������Ź�ʱ�����ô�һЩ*/					
		W25QXX_Write((u8*)&configParam,CONFIG_PARAM_ADDR,sizeof(configParam));	
		for(cksum=0;cksum<4;cksum++	)	
		{
			BEEP=!BEEP;
			vTaskDelay(250);
		}
		xTimerStop(timer,0);
			//watchdogInit(WATCHDOG_RESET_MS);		/*�������ÿ��Ź�*/
		//}						
	}
}

//void resetConfigParamPID(void)
//{
////	configParam.pidAngle = configParamDefault.pidAngle;
////	configParam.pidRate = configParamDefault.pidRate;
////	configParam.pidPos = configParamDefault.pidPos;
//}

/*
*********************************************************************************************************
*	�� �� ��: PID_Show
*	����˵��: ��ʾPID����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PID_Show(void)
{
	char str[10];

	sprintf(str,"%.1f   ",configParam.PID.veloc.P);
	LCD_ShowString(100,25,80,24,24,(u8*)str);
	sprintf(str,"%.1f   ",configParam.PID.veloc.I);
	LCD_ShowString(220,25,80,24,24,(u8*)str);
	sprintf(str,"%.1f   ",configParam.PID.veloc.D);
	LCD_ShowString(340,25,80,24,24,(u8*)str);
	
	sprintf(str,"%.1f   ",configParam.PID.rotate.P);
	LCD_ShowString(100,50,80,24,24,(u8*)str);
	sprintf(str,"%.1f   ",configParam.PID.rotate.I);
	LCD_ShowString(220,50,80,24,24,(u8*)str);
	sprintf(str,"%.1f   ",configParam.PID.rotate.D);
	LCD_ShowString(340,50,80,24,24,(u8*)str);
}

/*
*********************************************************************************************************
*	�� �� ��: RemoteFactor_Show
*	����˵��: ��ʾң���ٶȿ�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RemoteFactor_Show(void)
{
	char str[10];
	
	sprintf(str,"%.1f ",configParam.remoteFator.FBfator);
	LCD_ShowString(40,75,35,24,24,(u8*)str);
	sprintf(str,"%.1f ",configParam.remoteFator.LRfator);
	LCD_ShowString(135,75,35,24,24,(u8*)str);
}

/*
*********************************************************************************************************
*	�� �� ��: AutoSpeed_Show
*	����˵��: ��ʾ�����ٶ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AutoSpeed_Show(void)
{
	char str[10];
	
	sprintf(str,"%d  ",configParam.autoSpeed.lowSpeed);
	LCD_ShowString(60,100,65,24,24,(u8*)str);
	sprintf(str,"%d  ",configParam.autoSpeed.middleSpeed);
	LCD_ShowString(195,100,65,24,24,(u8*)str);
	sprintf(str,"%d  ",configParam.autoSpeed.hightSpeed);
	LCD_ShowString(330,100,65,24,24,(u8*)str);
}

/*
*********************************************************************************************************
*	�� �� ��: GPSBiasRange_Show
*	����˵��: ��ʾGPSλ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GPSBiasRange_Show(void)
{
	char str[10];
	
	sprintf(str,"%1f  ",configParam.GpsCali.gpsBias/1000.0);
	LCD_ShowString(385,225,40,24,24,(u8*)str);
}
