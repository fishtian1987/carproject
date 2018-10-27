/*
*********************************************************************************************************
*	                                  
*	ģ������ : ң�����ݴ���
*	�ļ����� : remote.c
*	��    �� : V1.3
*	˵    �� : ң�����ݴ���ҡ�����ֵ1684����Сֵ364��
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
*	�� �� ��: RemoteSpeedFatorProcess
*	����˵��: ����ң���ٶ���������
*	��    �Σ�p,��ֵ;
*	�� �� ֵ: ��
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
*	�� �� ��: AutoSpeedProcess
*	����˵��: �����Զ������ٶ�
*	��    �Σ�p,��ֵ;
*	�� �� ֵ: ��
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
*	�� �� ��: sendRmotorCmd
*	����˵��: ����ң������
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchBluetooth
*	����˵��: ������������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchAlarm
*	����˵��: ���ƿ�������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchTailLight
*	����˵��: β�ƿ�������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchBrakeLight
*	����˵��: ɲ���ƿ�������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchTurnLeftLight
*	����˵��: ��ת��ƿ�������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: SwitchTurnRightLight
*	����˵��: ��ת��ƿ�������
*	��    �Σ�sw,true:��;false:�ر�
*	�� �� ֵ: ��
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
*	�� �� ��: RemoteSetDevice
*	����˵��: ���������Դ
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
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
*	�� �� ��: RemoteControl
*	����˵��: ң��������ģʽ����
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
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
*	�� �� ��: SetSpeedDev
*	����˵��: ���������ٶȿ��ƺͲ���У׼
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
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
*	�� �� ��: StopControl
*	����˵��: ��������ֹͣ
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void StopControl(Remote *rc)
//{
//	
//}

//return the state of the remote 0:no action 1:action 
/*
*********************************************************************************************************
*	�� �� ��: IsRemoteBeingAction
*	����˵��: ��ѯң������״̬��Ϣ
*	��    �Σ���
*	�� �� ֵ: 0,�޻
						1,�
*********************************************************************************************************
*/
uint8_t IsRemoteBeingAction(void)
{
	return (abs(ChassisSpeedRef.forward_back_ref)>=10 || abs(ChassisSpeedRef.left_right_ref)>=10 );//|| fabs(GimbalRef.yaw_speed_ref)>=10 || fabs(GimbalRef.pitch_speed_ref)>=10);
}

/*
*********************************************************************************************************
*	�� �� ��: SetInputMode
*	����˵��: ����ģʽ���� 
*	��    �Σ�rc,ң������
*	�� �� ֵ: ��
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
*	�� �� ��: GetInputMode
*	����˵��: ��ѯ����ģʽ���� 
*	��    �Σ���
*	�� �� ֵ: ����ģʽ
*********************************************************************************************************
*/
InputMode_e GetInputMode(void)
{
	return inputmode;
}

/*
*********************************************************************************************************
*	�� �� ��: RemoteDataPrcess
*	����˵��: ����ң������������
*	��    �Σ�pData,ң������
*	�� �� ֵ: ��
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
		case REMOTE_INPUT://ң��������ģʽ
		{
			RemoteControl(rs);
		}break;
		case AUTO_NAVI:	//��������ģʽ
		{
			SetSpeedDev(rs);
		}break;
		case STOP://����ͣ��
		{
//			ChassisSpeedRef.forward_back_ref = 0;
		}break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: OperatingMode
*	����˵��: ��ʾ����ģʽ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void OperatingMode(void)
{
	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			Show_Str(190,770,100,24,"�ֶ�ģʽ",24,0);
		}break;
		case AUTO_NAVI:
		{
			Show_Str(190,770,100,24,"����ģʽ",24,0);
		}break;
		case STOP:
		{
			Show_Str(190,770,100,24,"����ͣ��",24,0);
		}break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: RemoteTaskInit
*	����˵��: ң�������ݳ�ʼ����б�º����ȵĳ�ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RemoteTaskInit(void)
{
//	//б�³�ʼ��
//	FBSpeedRamp.SetScale(&FBSpeedRamp, AUTO_FB_RAMP_TICK_COUNT);
//	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
}
