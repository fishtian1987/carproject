/*
*********************************************************************************************************
*	                                  
*	模块名称 : 数据显示和存储
*	文件名称 : io.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef _IO_H
#define _IO_H
#include "sys.h"	
#include "stdbool.h"  
#include "ff.h"  

#define INFOSIZE 35

#define PARAM_CALI_DONE 0x5A 	
#define PARAM_CALI_NONE 0x00

#define GPS_FLAG_HEADOffset ((uint16_t)1<<1)
#define RANGE_FLAG_GPS      ((uint16_t)1<<2)
#define PID_FLAG_MUX        ((uint16_t)1<<3)
#define FACTOR_FLAG_REMOTE  ((uint16_t)1<<4)
#define SPEED_FLAG_AUTO     ((uint16_t)1<<5)

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern char *filename;
extern FIL fil;
extern FRESULT res;
extern UINT bww;

void ioTask(void* param);
uint8_t Is_AppParam_Calied(void);
void AppParamInit(void);
void SaveInformation(void);
void ShowInformation(void);
void path_name(char *pname,char* path,char *name);
void CaliedDataProc(u8 *data);
void CalibrateLoop(void);
void SetCaliCmdFlag(uint16_t flag);
void MagCaliProcess(u8 *p);
#endif
