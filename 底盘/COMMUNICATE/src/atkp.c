/*
*********************************************************************************************************
*	                                  
*	模块名称 : 解析下底板数据
*	文件名称 : atkp.c
*	版    本 : V1.3
*	说    明 : 解析下底板数据
*
*
*********************************************************************************************************
*/

#include "atkp.h"
#include "maslink.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "gps.h"
#include "io.h"	
#include "compass.h"
#include "config_param.h"
#include "armlink.h"

#define  BYTE0(dwTemp)       ( *( (u8 *)(&dwTemp)	   ) )
#define  BYTE1(dwTemp)       ( *( (u8 *)(&dwTemp) + 1) )
#define  BYTE2(dwTemp)       ( *( (u8 *)(&dwTemp) + 2) )
#define  BYTE3(dwTemp)       ( *( (u8 *)(&dwTemp) + 3) )
#define  BYTE4(dwTemp)       ( *( (u8 *)(&dwTemp) + 4) )

#define ATKP_RX_QUEUE_SIZE 	10 

static xQueueHandle rxQueue;

/*
*********************************************************************************************************
*	函数名称: atkpSendPacket
*	功能说明: 发送数据包
*	形    参: p,待发送数据
*	返 回 值: 无
*********************************************************************************************************
*/
//static void atkpSendPacket(atkp_t *p)
//{
//	radiolinkSendPacket(p);
//}

/*
*********************************************************************************************************
*	函数名称: sendMagBias
*	功能说明: 发送磁偏角
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendMagBias()
{
	u8 _cnt=0;
	atkp_t p;
	vs16 _temp;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_MAGBIAS;
	p.data[_cnt++]=BYTE1(configParam.GpsCali.headOffset);
	p.data[_cnt++]=BYTE0(configParam.GpsCali.headOffset);
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendLocAvpt
*	功能说明: 发送位置阈值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendLocAvpt()
{
	u8 _cnt=0;
	atkp_t p;
	vs16 _temp;
	_temp=configParam.GpsCali.gpsBias/100;
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_LOCAVPT;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendPid
*	功能说明: 发送PID数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendPid(void)
{
	u8 _cnt=0;
	atkp_t p;
	vu16 _temp;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_PID;
	
	_temp = configParam.PID.veloc.P * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = configParam.PID.veloc.I * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = configParam.PID.veloc.D * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = configParam.PID.rotate.P * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = configParam.PID.rotate.I * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = configParam.PID.rotate.D * 10 ;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendRemoteSpeedFactor
*	功能说明: 发送遥控速度因子
*	形    参: 无	
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendRemoteSpeedFactor(void)
{
	u8 _cnt=0;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_REMOTESPEED;
	p.data[_cnt++]=configParam.remoteFator.FBfator*10;;
	p.data[_cnt++]=configParam.remoteFator.LRfator*10;
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendAutoSpeed
*	功能说明: 发送自动导航速度
*	形    参: 无			
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendAutoSpeed(void)
{
	u8 _cnt=0;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_AUTOSPEED;
	p.data[_cnt++]=BYTE1(configParam.autoSpeed.lowSpeed);
	p.data[_cnt++]=BYTE0(configParam.autoSpeed.lowSpeed);
	p.data[_cnt++]=BYTE1(configParam.autoSpeed.middleSpeed);
	p.data[_cnt++]=BYTE0(configParam.autoSpeed.middleSpeed);
	p.data[_cnt++]=BYTE1(configParam.autoSpeed.hightSpeed);
	p.data[_cnt++]=BYTE0(configParam.autoSpeed.hightSpeed);
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函 数 名: requestBaseLocation
*	功能说明: 请求基地址
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void requestBaseLocation(void)
{
	u8 _cnt=0;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_ROUTE_BIAS;
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函 数 名: requestRoutesData
*	功能说明: 请求路径差值
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void requestRoutesData(void)
{
	u8 _cnt=0;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_ROUTE;
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendCollectCoord
*	功能说明: 发送采集坐标点
*	形    参: 无			
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendCollectCoord(void)
{
	u8 _cnt=0;
	int64_t temp;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_COORD;	
	temp=gpsx.latitude*GPS_COORS_STICK_OFFSET;	
	p.data[_cnt++]=BYTE4(temp);
	p.data[_cnt++]=BYTE3(temp);
	p.data[_cnt++]=BYTE2(temp);
	p.data[_cnt++]=BYTE1(temp);
	p.data[_cnt++]=BYTE0(temp);	
	temp=gpsx.longitude*GPS_COORS_STICK_OFFSET;	
	p.data[_cnt++]=BYTE4(temp);
	p.data[_cnt++]=BYTE3(temp);
	p.data[_cnt++]=BYTE2(temp);
	p.data[_cnt++]=BYTE1(temp);
	p.data[_cnt++]=BYTE0(temp);
	
	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}

/*
*********************************************************************************************************
*	函数名称: sendDetectSpeed
*	功能说明: 发送轮子及整车速度
*	形    参: 无			
*	返 回 值: 无
*********************************************************************************************************
*/
static void sendDetectSpeed(void)
{
	u8 _cnt=0;
	int16_t _temp;
	atkp_t p;
	
	p.msgID = DOWN_ARM;
	p.data[_cnt++]=D_COMMAND_ARM_DETECTSPEED;	
	_temp=CM1SpeedPID.ref;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	
	_temp=CM2SpeedPID.ref;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	
	_temp=CM3SpeedPID.ref;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	
	_temp=CM4SpeedPID.ref;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	
	_temp=gpsx.speed;
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	

	p.dataLen = _cnt;
	armlinkSendPacket(&p);
}
/*
*********************************************************************************************************
*	函数名称: atkpCheckSum
*	功能说明: 校验数据
*	形    参: packet,待发送数据包
*	返 回 值: 无
*********************************************************************************************************
*/
//static u8 atkpCheckSum(atkp_t *packet)
//{
//	u8 sum;
//	sum = DOWN_BYTE1;
//	sum += DOWN_BYTE2;
//	sum += packet->msgID;
//	sum += packet->dataLen;
//	for(int i=0; i<packet->dataLen; i++)
//	{
//		sum += packet->data[i];
//	}
//	return sum;
//}

/*
*********************************************************************************************************
*	函数名称: armReceiveAnl
*	功能说明: 解析arm数据包
*	形    参: packet,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void armReceiveAnl(atkp_t *packet)
{
	switch(packet->data[0])
	{
		case D_COMMAND_ARM_ROUTE_BIAS:
		{
			GPSBaseLocation(packet->data+1);
		}break;
		case D_COMMAND_ARM_ROUTE:
		{
			if(packet->dataLen!=1)
				GPSRoutesData(packet);
		}break;
		case D_COMMAND_ARM_COORD:
		{
			sendCollectCoord();
		}break;
		case D_COMMAND_ARM_MAGBIAS:
		{
			if(packet->dataLen==1)
				sendMagBias();
			else
			{
				GPSHeadOffsetProcess(packet->data+1);
				SetCaliCmdFlag(GPS_FLAG_HEADOffset);
				timerStart();
			}		
		}break;
		case D_COMMAND_ARM_LOCAVPT:
		{
			if(packet->dataLen==1)
				sendLocAvpt();
			else
			{
				GPSBiasProcess(packet->data+1);
				SetCaliCmdFlag(RANGE_FLAG_GPS);
				timerStart();
			}		
		}break;
		case D_COMMAND_ARM_PID:
		{
			if(packet->dataLen==1)
				sendPid();
			else
			{
				PIDCaliProcess(packet->data+1);
				SetCaliCmdFlag(PID_FLAG_MUX);
				timerStart();
			}
		}break;
		case D_COMMAND_ARM_REMOTESPEED:
		{
			if(packet->dataLen==1)
				sendRemoteSpeedFactor();
			else
			{
				RemoteSpeedFatorProcess(packet->data+1);
				SetCaliCmdFlag(FACTOR_FLAG_REMOTE);	
				timerStart();				
			}		
		}break;
		case D_COMMAND_ARM_AUTOSPEED:
		{
			if(packet->dataLen==1)
				sendAutoSpeed();
			else
			{
				AutoSpeedProcess(packet->data+1);
				SetCaliCmdFlag(SPEED_FLAG_AUTO);
				timerStart();
			}
		}break;	
		case D_COMMAND_ARM_DETECTSPEED:
		{
			sendDetectSpeed();
		}break;
		default:break;
	}
}

/*
*********************************************************************************************************
*	函数名称: subReceiveAnl
*	功能说明: 解析arm数据包
*	形    参: packet,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void subReceiveAnl(atkp_t *packet)
{
	switch(packet->data[0])
	{
		case D_COMMAND_SUB_HDATA:
		{
			gpsDataProcess(packet->data+1);
		}break;
		default:break;
	}
}

/*
*********************************************************************************************************
*	函数名称: atkpReceiveAnl
*	功能说明: 解析数据包
*	形    参: packet,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
static void atkpReceiveAnl(atkp_t *anlPacket)
{
	if(anlPacket->msgID	==DOWN_ARM)
		armReceiveAnl(anlPacket);
	else if(anlPacket->msgID == DOWN_SUB)
		subReceiveAnl(anlPacket);
} 

/*
*********************************************************************************************************
*	函数名称: atkpTxTask
*	功能说明: atkp·¢ËÍÈÎÎñÈÎÎñ
*	形    参: param,参数
*	返 回 值: 无
*********************************************************************************************************
*/
//void atkpTxTask(void *param)
//{
//	//sendMsgACK();
//	while(1)
//	{
//		atkpSendPeriod();
//		vTaskDelay(1);
//	}
//}

/*
*********************************************************************************************************
*	函数名称: atkpRxAnlTask
*	功能说明: 等待接收数据包
*	形    参: param,参数
*	返 回 值: 无
*********************************************************************************************************
*/
void atkpRxAnlTask(void *param)
{
	atkp_t p;
	while(1)
	{
		xQueueReceive(rxQueue, &p, portMAX_DELAY);
		atkpReceiveAnl(&p);
	}
}

/*
*********************************************************************************************************
*	函数名称: atkpInit
*	功能说明: 创建接收数据队列
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void atkpInit(void)
{
	rxQueue = xQueueCreate(ATKP_RX_QUEUE_SIZE, sizeof(atkp_t));
	ASSERT(rxQueue);
}

/*
*********************************************************************************************************
*	函数名称: atkpReceivePacketBlocking
*	功能说明: 发送接收数据包队列
*	形    参: p,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
bool atkpReceivePacketBlocking(atkp_t *p)
{
	ASSERT(p);
	ASSERT(p->dataLen <= ATKP_MAX_DATA_SIZE);
	return xQueueSend(rxQueue, p, portMAX_DELAY);	
}
