/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��̬�����ںϴ���
*	�ļ����� : stabilizer.c
*	��    �� : V1.3
*	˵    �� : ��̬�����ںϴ���
*
*
*********************************************************************************************************
*/

#include "stabilizer.h"
#include "remote.h" 
#include "pwm_motors.h"
#include "common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "io.h"	
#include "supervise.h"
#include "LostCounter.h"
#include "remote.h"
#include "compass.h"
#include "config_param.h"
#include "gps.h"
#include "lcd.h"

PID_Regulator_t CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT; 
PID_Regulator_t CM1SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM2SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM3SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM4SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
WorkState_e lastWorkState = PREPARE_STATE;
WorkState_e workState = PREPARE_STATE;
static uint32_t time_tick_ms = 0;

/*
*********************************************************************************************************
*	�� �� ��: PIDCaliProcess
*	����˵��: ����PID
*	��    �Σ�p,����ֵ;
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PIDCaliProcess(u8* p)
{
	u16 temp;
	u16 temp_;
	temp=*(p);
	temp_=temp<<8;
	temp=*(p+1);
	temp_|=temp;
	CM1SpeedPID.kp=temp_/10.0;
	CM2SpeedPID.kp=CM1SpeedPID.kp;
	CM3SpeedPID.kp=CM1SpeedPID.kp;
	CM4SpeedPID.kp=CM1SpeedPID.kp;
	configParam.PID.veloc.P=CM1SpeedPID.kp;	
	temp=*(p+2);
	temp_=temp<<8;
	temp=*(p+3);
	temp_|=temp;
	CM1SpeedPID.ki=temp_/10.0;
	CM2SpeedPID.ki=CM1SpeedPID.ki;
	CM3SpeedPID.ki=CM1SpeedPID.ki;
	CM4SpeedPID.ki=CM1SpeedPID.ki;
	configParam.PID.veloc.I=CM1SpeedPID.ki;
	temp=*(p+4);
	temp_=temp<<8;
	temp=*(p+5);
	temp_|=temp;
	CM1SpeedPID.kd=temp_/10.0;
	CM2SpeedPID.kd=CM1SpeedPID.kd;
	CM3SpeedPID.kd=CM1SpeedPID.kd;
	CM4SpeedPID.kd=CM1SpeedPID.kd;
	configParam.PID.veloc.D=CM1SpeedPID.kd;
	temp=*(p+6);
	temp_=temp<<8;
	temp=*(p+7);
	temp_|=temp;
	CMRotatePID.kp=temp_/10.0;
	configParam.PID.rotate.P=CMRotatePID.kp;	
	temp=*(p+8);
	temp_=temp<<8;
	temp=*(p+9);
	temp_|=temp;
	CMRotatePID.ki=temp_/10.0;
	configParam.PID.rotate.I=CMRotatePID.ki;	
	temp=*(p+10);
	temp_=temp<<8;
	temp=*(p+11);
	temp_|=temp;
	CMRotatePID.kd=temp_/10.0;
	configParam.PID.rotate.D=CMRotatePID.kd;
}

/*
*********************************************************************************************************
*	�� �� ��: SetWorkState
*	����˵��: ���ù���״̬
*	��    �Σ�state,����״̬
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SetWorkState(WorkState_e state)
{
	workState = state;
}

/*
*********************************************************************************************************
*	�� �� ��: GetWorkState
*	����˵��: ��ѯ����״̬
*	��    �Σ���
*	�� �� ֵ: ���ع���״̬
*********************************************************************************************************
*/
WorkState_e GetWorkState(void)
{
	return workState;
}

/*
*********************************************************************************************************
*	�� �� ��: WorkStateFSM
*	����˵��: ����״̬�л�״̬��,��1ms��ʱ�ж�ͬƵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WorkStateFSM(void)
{
	lastWorkState = workState;
	switch(workState)
	{
		case PREPARE_STATE: //׼���׶� 
		{
			if(GetInputMode() == STOP )//|| Is_Serious_Error())
				workState = STOP_STATE;
			else if(time_tick_ms > PREPARE_TIME_TICK_MS)
			{
				workState = NORMAL_STATE;	
				MotorsStart();
			}				
		}break;
		case AUTONAVI_STATE:
		{
		
		}break;
		case NORMAL_STATE:
		{
			if((GetInputMode() == STOP))
				workState = STOP_STATE;
		
//			else if(GetCaliCmdFlagGrp())   
//			{
//				workState = CALI_STATE;
//			}
//			else if((!IsRemoteBeingAction() ||(Get_Lost_Error(LOST_ERROR_RC) == LOST_ERROR_RC)) && GetShootState() != SHOOTING)
//				workState = STANDBY_STATE;      	
		}break;
		case STOP_STATE:   
		{
			if((GetInputMode() != STOP))
				workState = PREPARE_STATE;   
		}break;
		case CALI_STATE:    //У׼״̬  
		{
//			if(GetInputMode() == STOP )//|| Is_Serious_Error())
//				workState = STOP_STATE;
		}break;	    
		default:
			break;
	}	
}

void stabilizerInit(void)
{
	
}

/*
*********************************************************************************************************
*	�� �� ��: AutoControl
*	����˵��: �Զ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AutoControl(void)
{
	if(GetInputMode()==AUTO_NAVI)
	{
		if(IsRouteDataFlagSet(GPS_RouteData_Init))
		{
			gpsTask();
			CMRotatePID.ref = 0;
			CMRotatePID.fdb = gpsx.ecd_angle;
			CMRotatePID.Calc(&CMRotatePID);   
			ChassisSpeedRef.left_right_ref = CMRotatePID.output;//*CHASSIS_ROTATE_ATTEN;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: CMControlLoop
*	����˵��: ���̿�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CMControlLoop(void)
{

//	if(Is_Lost_Error_Set(LOST_ERROR_RC))      //���ң������ʧ��ǿ�ƽ��ٶ��趨ֵreset
//	{
//		ChassisSpeedRef.forward_back_ref = 0;
//		ChassisSpeedRef.left_right_ref = 0;
//	}

	if((GetWorkState() == STOP_STATE))//||Is_Serious_Error() || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE) //����ͣ����У׼���޿�������ʱֹͣ
		MotorsSetRatio(0,0);
	else
		MotorsSetRatio(ChassisSpeedRef.left_right_ref,ChassisSpeedRef.forward_back_ref);
}

/*
*********************************************************************************************************
*	�� �� ��: Speed_Show
*	����˵��: ��ʾ�ٶ���Ϣ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Speed_Show(void)
{
	char str[15];
	
	sprintf(str,"%d  ",ChassisSpeedRef.forward_back_ref);
	LCD_ShowString(75,0,70,24,24,(u8*)str);	
	sprintf(str,"%d  ",ChassisSpeedRef.left_right_ref);
	LCD_ShowString(240,0,70,24,24,(u8*)str);	
}

/*
*********************************************************************************************************
*	�� �� ��: WorkStateSwitchProcess
*	����˵��: ���������ģʽ�л���prapareģʽ��Ҫ��һϵ�в�����ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//static void WorkStateSwitchProcess(void)
//{
//	if((lastWorkState != workState) && (workState == PREPARE_STATE))  
//	{
//		ControtLoopTaskInit();
//		RemoteTaskInit();
//	}
//}

/*
*********************************************************************************************************
*	�� �� ��: stabilizerTask
*	����˵��: λ����̬��������
*	��    �Σ�param,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void stabilizerTask(void* param)
{
	u32 lastWakeTime = getSysTickCnt();
	while(1)
	{
		vTaskDelayUntil(&lastWakeTime,2);
		time_tick_ms++;
		WorkStateFSM();
		AutoControl();
		//WorkStateSwitchProcess();
		//SuperviseTask();    
		CMControlLoop();			 
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ControtLoopTaskInit
*	����˵��: ���������ʼ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ControtLoopTaskInit(void)
{
	//AppParamInit();	//���������ʼ��
	SetWorkState(PREPARE_STATE);//���ù���ģʽ

	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_RC));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR1));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR2));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR3));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR4));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MAG));
	LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_NOCALI));
 
	//PID��ʼ��
	CMRotatePID.Reset(&CMRotatePID);
}

