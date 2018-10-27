/*
*********************************************************************************************************
*	                                  
*	模块名称 : 配置处理模块
*	文件名称 : config_param.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __CONFIG_PARAM_H
#define __CONFIG_PARAM_H
#include "sys.h"
#include "stdbool.h"
#include "stabilizer.h"
#include "remote.h"
#include "gps.h"

typedef struct	
{
	u8 version;
	GpsCaliStruct_t GpsCali;
	PIDStruct_t PID;
	Remote_Speed_Fator remoteFator;
	Auto_Speed autoSpeed;

} configParam_t;

extern configParam_t configParam;

void configParamInit(void);
void configParamTask(void* param);
void timerStart(void);
void PID_Show(void);
void GPSBiasRange_Show(void);
void RemoteFactor_Show(void);
void AutoSpeed_Show(void);
#endif 

