/*
*********************************************************************************************************
*	                                  
*	模块名称 : 姿态数据融合处理
*	文件名称 : stabilizer.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __STABALIZER_H
#define __STABALIZER_H
#include "pid_regulator.h" 

typedef enum
{
	PREPARE_STATE=1,    //上电后初始化准备阶段4s钟左右
	AUTONAVI_STATE,			//自动导航阶段
	NORMAL_STATE,			  //正常输入状态
	STOP_STATE,        	//停止运动状态
	CALI_STATE,    			//校准状态
}WorkState_e;

#define PREPARE_TIME_TICK_MS       500   

typedef __packed struct
{
	float P; 
	float I; 
	float D; 
}PID_t;

typedef __packed struct
{
	PID_t veloc; 
	PID_t rotate;
}PIDStruct_t;

#define CHASSIS_MOTOR_ROTATE_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	1.5f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	4900,\
	1000,\
	1500,\
	0,\
	5000,\
	0,\
	0,\
	0,\
	&PID_Calc,\
	&PID_Reset,\
}\

#define CHASSIS_MOTOR_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	60.0f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	4900,\
	3500,\
	1500,\
	0,\
	4950,\
	0,\
	0,\
	0,\
	&PID_Calc,\
	&PID_Reset,\
}\

extern PID_Regulator_t CMRotatePID;
extern PID_Regulator_t CM1SpeedPID;
extern PID_Regulator_t CM2SpeedPID;
extern PID_Regulator_t CM3SpeedPID;
extern PID_Regulator_t CM4SpeedPID;

void stabilizerInit(void);
void stabilizerTask(void* param);
void ControtLoopTaskInit(void);
void PIDCaliProcess(u8* p);
WorkState_e GetWorkState(void);
void Speed_Show(void);
#endif
