/*
*********************************************************************************************************
*	                                  
*	模块名称 : 遥控数据处理
*	文件名称 : remote.c
*	版    本 : V1.3
*	说    明 : 遥控数据处理。摇杆最大值1684，最小值364。
*
*
*********************************************************************************************************
*/

#include "remote.h"
#include "stabilizer.h"
#include "stdio.h"
#include "math.h"
#include "lcd.h"
#include "text.h"
#include "atkp.h"
#include "maslink.h"
#include "config_param.h"

typedef struct
{
	bool bluetooth;
	bool alarm;
	bool tail_light;
	bool brake_light;
	bool left_light;
	bool right_light;
}device_sta_t;

device_sta_t device_sta={false,false,false,false,false,false};
int16_t rs[10]  = {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};
ChassisSpeed_Ref_t ChassisSpeedRef={0};
//RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;
Auto_Speed autoSpeed;
Remote_Speed_Fator remoteFator;
static InputMode_e inputmode = STOP; 

/*
*********************************************************************************************************
*	函 数 名: RemoteSpeedFatorProcess
*	功能说明: 更新遥控速犊刂埔蜃尤
*	形    参：p,数值;
*	返 回 值: 无
*********************************************************************************************************
*/
void RemoteSpeedFatorProcess(u8* p)
{
	remoteFator.FBfator=*(p)/10.0;
	configParam.remoteFator.FBfator=remoteFator.FBfator;
	remoteFator.LRfator=*(p+1)/10.0;
	configParam.remoteFator.LRfator=remoteFator.LRfator;
}

/*
*********************************************************************************************************
*	函 数 名: AutoSpeedProcess
*	功能说明: 更新自动导航速度
*	形    参：p,数值;
*	返 回 值: 无
*********************************************************************************************************
*/
void AutoSpeedProcess(u8* p)
{
	u16 temp;
	u16 temp_;
	temp=*(p);
	temp_=temp<<8;
	temp=*(p+1);
	temp_|=temp;
	autoSpeed.lowSpeed=temp_;
	configParam.autoSpeed.lowSpeed=autoSpeed.lowSpeed;
	temp=*(p+2);
	temp_=temp<<8;
	temp=*(p+3);
	temp_|=temp;
	autoSpeed.middleSpeed=temp_;
	configParam.autoSpeed.middleSpeed=autoSpeed.middleSpeed;
	temp=*(p+4);
	temp_=temp<<8;
	temp=*(p+5);
	temp_|=temp;
	autoSpeed.hightSpeed=temp_;
	configParam.autoSpeed.hightSpeed=autoSpeed.hightSpeed;
}

/*
*********************************************************************************************************
*	函 数 名: sendRmotorCmd
*	功能说明: 发送遥控命令
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void sendRmotorCmd(u8 cmd, u8 data)
{
	atkp_t p;
	p.msgID = MAIN_BOARD;
	p.dataLen = 2;
	p.data[0] = cmd;
	p.data[1] = data;
	radiolinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函 数 名: SwitchBluetooth
*	功能说明: 蓝牙开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchBluetooth(bool sw)
{
	if(sw)
	{
		if(!device_sta.bluetooth)
		{
			sendRmotorCmd(D_COMMAND_SUB_BLUETOOTH,1);
			device_sta.bluetooth=true;
		}
	}
	else
	{
		if(device_sta.bluetooth)
		{
			sendRmotorCmd(D_COMMAND_SUB_BLUETOOTH,0);
			device_sta.bluetooth=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SwitchAlarm
*	功能说明: 警灯开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchAlarm(bool sw)
{
	if(sw)
	{
		if(!device_sta.alarm)
		{
			sendRmotorCmd(D_COMMAND_SUB_ALARM,1);
			device_sta.alarm=true;
		}
	}
	else
	{
		if(device_sta.alarm)
		{
			sendRmotorCmd(D_COMMAND_SUB_ALARM,0);
			device_sta.alarm=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SwitchTailLight
*	功能说明: 尾灯开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchTailLight(bool sw)
{
	if(sw)
	{
		if(!device_sta.tail_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TAIL_LIGHT,1);
			device_sta.tail_light=true;
		}
	}
	else
	{
		if(device_sta.tail_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TAIL_LIGHT,0);
			device_sta.tail_light=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SwitchBrakeLight
*	功能说明: 刹车灯开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchBrakeLight(bool sw)
{
	if(sw)
	{
		if(!device_sta.brake_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_BRAKE_LIGHT,1);
			device_sta.brake_light=true;
		}
	}
	else
	{
		if(device_sta.brake_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_BRAKE_LIGHT,0);
			device_sta.brake_light=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SwitchTurnLeftLight
*	功能说明: 左转向灯开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchTurnLeftLight(bool sw)
{
	if(sw)
	{
		if(!device_sta.left_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TURN_LEFT_LIGHT,1);
			device_sta.left_light=true;
		}
	}
	else
	{
		if(device_sta.left_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TURN_LEFT_LIGHT,0);
			device_sta.left_light=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SwitchTurnRightLight
*	功能说明: 右转向灯开关命令
*	形    参：sw,true:打开;false:关闭
*	返 回 值: 无
*********************************************************************************************************
*/
void SwitchTurnRightLight(bool sw)
{
	if(sw)
	{
		if(!device_sta.right_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TURN_RIGHT_LIGHT,1);
			device_sta.right_light=true;
		}
	}
	else
	{
		if(device_sta.right_light)
		{
			sendRmotorCmd(D_COMMAND_SUB_TURN_RIGHT_LIGHT,0);
			device_sta.right_light=false;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: RemoteSetDevice
*	功能说明: 开关外设电源
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void RemoteSetDevice(int16_t *rc)
{
	if(rc[5]<=400)
	{
		SwitchTailLight(false);
		SwitchAlarm(false);
	}
	else if(rc[5]>400 && rc[5]<=500)
	{
		SwitchTailLight(true);
		SwitchAlarm(false);
	}
	else if(rc[5]>500&&rc[5]<=600)
		SwitchAlarm(true);
}

/*
*********************************************************************************************************
*	函 数 名: RemoteControl
*	功能说明: 遥控器控制模式处理
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void RemoteControl(int16_t *rc)
{
	if(GetWorkState()!=PREPARE_STATE)
	{
		ChassisSpeedRef.forward_back_ref = (rc[1] - (int16_t)REMOTE_CONTROLLER_STICK_VALUE_MIDDLE) * remoteFator.FBfator;
		ChassisSpeedRef.left_right_ref   = (rc[0] - (int16_t)REMOTE_CONTROLLER_STICK_VALUE_MIDDLE) * remoteFator.LRfator; 
	}
	RemoteSetDevice(rc);
}

/*
*********************************************************************************************************
*	函 数 名: SetSpeedDev
*	功能说明: 自主导航速度控制和参数校准
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void SetSpeedDev(int16_t *rc)
{	
	if(IsRouteDataFlagSet(GPS_RouteData_Init))
		ChassisSpeedRef.forward_back_ref = (rc[7]- (int16_t)REMOTE_CONTROLLER_STICK_VALUE_LOW) * 0.75;
	RemoteSetDevice(rc);
}

/*
*********************************************************************************************************
*	函 数 名: StopControl
*	功能说明: 紧急控制停止
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
//void StopControl(Remote *rc)
//{
//	
//}

//return the state of the remote 0:no action 1:action 
/*
*********************************************************************************************************
*	函 数 名: IsRemoteBeingAction
*	功能说明: 查询遥控器的状态信息
*	形    参：无
*	返 回 值: 0,无活动
						1,活动
*********************************************************************************************************
*/
uint8_t IsRemoteBeingAction(void)
{
	return (abs(ChassisSpeedRef.forward_back_ref)>=10 || abs(ChassisSpeedRef.left_right_ref)>=10 );//|| fabs(GimbalRef.yaw_speed_ref)>=10 || fabs(GimbalRef.pitch_speed_ref)>=10);
}

/*
*********************************************************************************************************
*	函 数 名: SetInputMode
*	功能说明: 输入模式设置 
*	形    参：rc,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void SetInputMode(int16_t *rc)
{
	if(rc[6] == REMOTE_CONTROLLER_STICK_VALUE_LOW)
		inputmode = REMOTE_INPUT;
	else if(rc[6] == REMOTE_CONTROLLER_STICK_VALUE_MIDDLE)
		inputmode = STOP;
	else if(rc[6] == REMOTE_CONTROLLER_STICK_VALUE_HIGH)
		inputmode = AUTO_NAVI;
}

/*
*********************************************************************************************************
*	函 数 名: GetInputMode
*	功能说明: 查询输入模式设置 
*	形    参：无
*	返 回 值: 输入模式
*********************************************************************************************************
*/
InputMode_e GetInputMode(void)
{
	return inputmode;
}

/*
*********************************************************************************************************
*	函 数 名: RemoteDataPrcess
*	功能说明: 解析遥控器按键数据
*	形    参：pData,遥控数据
*	返 回 值: 无
*********************************************************************************************************
*/
void RemoteDataPrcess(u8 *pData)
{
	if(pData == NULL)
		return;
	
	uint8_t i;
	uint8_t byte_in_sbus = 1;
	uint8_t bit_in_sbus = 0;
	uint8_t ch = 0;
	uint8_t bit_in_channel = 0;
	
	for (i=0; i<16; i++) 
		rs[i] = 0;

	for (i=0; i<110; i++) 
	{
			if (pData[byte_in_sbus] & (1<<bit_in_sbus)) {
					rs[ch] |= (1<<bit_in_channel);
			}
			bit_in_sbus++;
			bit_in_channel++;

			if (bit_in_sbus == 8) 
			{
					bit_in_sbus =0;
					byte_in_sbus++;
			}
			if (bit_in_channel == 11) 
			{
					bit_in_channel =0;
					ch++;
			}
	}
	
	SetInputMode(rs);
	switch(GetInputMode())
	{
		case REMOTE_INPUT://遥控器控制模式
		{
			RemoteControl(rs);
		}break;
		case AUTO_NAVI:	//自主控制模式
		{
			SetSpeedDev(rs);
		}break;
		case STOP://紧急停车
		{
//			ChassisSpeedRef.forward_back_ref = 0;
		}break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: OperatingMode
*	功能说明: 显示操作模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void OperatingMode(void)
{
	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			Show_Str(190,770,100,24,"手动模式",24,0);
		}break;
		case AUTO_NAVI:
		{
			Show_Str(190,770,100,24,"导航模式",24,0);
		}break;
		case STOP:
		{
			Show_Str(190,770,100,24,"紧急停车",24,0);
		}break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: RemoteTaskInit
*	功能说明: 遥控器数据初始化，斜坡函数等的初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RemoteTaskInit(void)
{
//	//斜坡初始化
//	FBSpeedRamp.SetScale(&FBSpeedRamp, AUTO_FB_RAMP_TICK_COUNT);
//	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
}
