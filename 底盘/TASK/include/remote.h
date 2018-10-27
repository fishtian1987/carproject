/*
*********************************************************************************************************
*	                                  
*	模块名称 : 遥控器解码处理
*	文件名称 : remote.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __REMOTE_H
#define __REMOTE_H	 
#include "sys.h"	    
#include "ramp.h"

#define AUTO_FB_RAMP_TICK_COUNT		          100

#define REMOTE_CONTROLLER_STICK_VALUE_HIGH           1694u   
#define REMOTE_CONTROLLER_STICK_VALUE_MIDDLE         1000u  
#define REMOTE_CONTROLLER_STICK_VALUE_LOW            306u 
#define REMOTE_CONTROLLER_STICK_VALUE_RANGE_FB       695.0f

typedef __packed struct
{
	float FBfator;
	float LRfator;
}Remote_Speed_Fator;

typedef __packed struct
{
	u16 lowSpeed;
	u16 middleSpeed;
	u16 hightSpeed;
}Auto_Speed;


typedef __packed struct
{
	int16_t forward_back_ref;
	int16_t left_right_ref;
}ChassisSpeed_Ref_t;


//输入模式:遥控器/自主导航/急停
typedef enum
{
	REMOTE_INPUT = 1,
	STOP         = 2,
	AUTO_NAVI    = 3,
}InputMode_e;


extern int16_t rs[];
extern ChassisSpeed_Ref_t ChassisSpeedRef;
extern u16 left_right_speed;
extern RampGen_t LRSpeedRamp;
extern Remote_Speed_Fator remoteFator;
extern Auto_Speed autoSpeed;

void RemoteDataPrcess(u8 *pData);
void RemoteTaskInit(void);
//void RemoteControl(Remote *rc);
void RemoteSpeedFatorProcess(u8* p);
void AutoSpeedProcess(u8* p);
InputMode_e GetInputMode(void);
void OperatingMode(void);
#endif
