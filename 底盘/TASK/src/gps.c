/*
*********************************************************************************************************
*	                                  
*	模块名称 : 处理GPS数据
*	文件名称 : gps.c
*	版    本 : V1.3
*	说    明 : 校验数据
*
*
*********************************************************************************************************
*/

#include "gps.h"
#include "stdio.h"
#include "lcd.h"
#include "text.h"	
#include "combug.h"
#include "math.h"
#include "compass.h"
#include "io.h"	
#include "config.h"	
#include "remote.h"
#include "config_param.h"
#include "string.h"

nmea_msg gpsx={0};
typedef __packed struct  
{
	int32_t fifo[5];
	u8 index;
}HEAD_FIFO_t;

HEAD_FIFO_t headFifo={
	{0},
	0,
};

typedef __packed struct  
{
	double latitude;		
	double longitude;	
	int16_t coors[2][20];
	u8 index;
}ROUTES_t;

static u8 bflagRoutesData=0;//1:组内坐标转换 2：组间坐标转换 3:数据准备

ROUTES_t routes={
	0,
	0,
	{0},
	0
};

GpsCaliStruct_t GpsCali={0,0};
head_t headData={0,0,0,0,0,0};

void show_test()
{
	char str[16];
	
	sprintf(str,"%.8f",routes.latitude);
	LCD_ShowString(60,300,125,24,24,(u8*)str);
	sprintf(str,"%.8f",routes.longitude);
	LCD_ShowString(200,300,135,24,24,(u8*)str);
}

/*
*********************************************************************************************************
*	函 数 名: SetRouteDataFlag
*	功能说明: 设置数据标志位
*	形    参：flag,标志位
*	返 回 值: 无
*********************************************************************************************************
*/
void SetRouteDataFlag(u8 flag)  
{
	bflagRoutesData |= flag;
}

/*
*********************************************************************************************************
*	函 数 名: ResetRouteDataFlag
*	功能说明: 取消数据标志位
*	形    参：flag,标志位
*	返 回 值: 无
*********************************************************************************************************
*/
void ResetRouteDataFlag(u8 flag)
{
	bflagRoutesData &= ~flag;
}

/*
*********************************************************************************************************
*	函 数 名: IsRouteDataFlagSet
*	功能说明: 查询某一位数据标志位
*	形    参：flag,待查询标志位
*	返 回 值: 0,未设置
						1,设置
*********************************************************************************************************
*/
bool IsRouteDataFlagSet(u8 flag)
{
	if(flag & bflagRoutesData)return true;
	else return false;	
}

/*
*********************************************************************************************************
*	函 数 名: Reset_RouteData_Flagt
*	功能说明: 复位所有数据标志位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Reset_RouteData_Flagt(void)
{
	bflagRoutesData=0;
	gpsx.count=0;
}
/*
*********************************************************************************************************
*	函 数 名: GPSBaseLocation
*	功能说明: 更新路径基准值
*	形    参：p,路径值;
*	返 回 值: 无
*********************************************************************************************************
*/
void GPSBaseLocation(u8* p)
{
	int64_t temp;
	int64_t temp_=0;
	u8 i=0;
	for(i=0;i<5;i++)
	{
		temp=*(p+i);
		temp_|=(temp<<(4-i)*8);	
	}
	routes.latitude=temp_/GPS_COORS_STICK_OFFSET;
	temp_=0;
	for(;i<10;i++)
	{
		temp=*(p+i);
		temp_|=(temp<<(9-i)*8);	
	}
	routes.longitude=temp_/GPS_COORS_STICK_OFFSET;
	requestRoutesData();
}

/*
*********************************************************************************************************
*	函 数 名: GPSRoutesData
*	功能说明: 更新路径差值
*	形    参：packet,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
void GPSRoutesData(atkp_t *packet)
{
	u16 temp;
	int16_t temp_;
	u8 *p=packet->data+1;
	u8 datalen=(packet->dataLen-1)/2;
	u8 i=0;
	
	for(i=0;i<datalen;i++)
	{
		temp=*(p+2*i);
		temp_=temp<<8;	
		temp=*(p+(2*i+1));
		temp_|=temp;
		
		if(IsRouteDataFlagSet(GPS_RouteData_Init))
		{
			if(IsRouteDataFlagSet(GPS_Array_External))
				routes.coors[0][i]=temp_;
			else
				routes.coors[1][i]=temp_;
		}
		else
		{
			if(IsRouteDataFlagSet(GPS_Array_External))
				routes.coors[1][i]=temp_;
			else
				routes.coors[0][i]=temp_;
		}
	}	
	
	if(!IsRouteDataFlagSet(GPS_RouteData_Init))
	{	
		if(IsRouteDataFlagSet(GPS_Array_External))
		{
			ResetRouteDataFlag(GPS_Array_External);
			SetRouteDataFlag(GPS_RouteData_Init);
		}
		else
		{
			SetRouteDataFlag(GPS_Array_External);
			requestRoutesData();
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: GPSHeadOffsetProcess
*	功能说明: 更新航向修正值
*	形    参：p,修正值;
*	返 回 值: 无
*********************************************************************************************************
*/
void GPSHeadOffsetProcess(u8* p)
{
	u16 temp;
	int16_t temp_;
	temp=*(p);
	temp_=temp<<8;
	temp=*(p+1);
	temp_|=temp;
	GpsCali.headOffset = temp_;
	configParam.GpsCali.headOffset=GpsCali.headOffset;
}

/*
*********************************************************************************************************
*	函 数 名: GPSBiasProcess
*	功能说明: 更新GPS偏差范围
*	形    参：p,校准值;
*	返 回 值: 无
*********************************************************************************************************
*/
void GPSBiasProcess(u8* p)
{
	u16 temp;
	u16 temp_;
	temp=*(p);
	temp_=temp<<8;
	temp=*(p+1);
	temp_|=temp;
	GpsCali.gpsBias=temp_*100;
	configParam.GpsCali.gpsBias=GpsCali.gpsBias;
}

/*
*********************************************************************************************************
*	函 数 名: gpsRange
*	功能说明: gps是否到达指定点坐标
*	形    参：movdot,待比较点
				 targetdot,目标点
*	返 回 值: true,到达位置
						false,未达位置
*********************************************************************************************************
*/
bool gpsRange(double *movdot,double *targetdot)
{
	int32_t temp=(*targetdot-*movdot)*GPS_COORS_STICK_OFFSET;
	temp=abs(temp);
	if(temp<GpsCali.gpsBias)
		return true;
	return false;
}

/*
*********************************************************************************************************
*	函 数 名: gpsDataProcess
*	功能说明: 更新GPS数据
*	形    参：pk,数据包
*	返 回 值: 无
*********************************************************************************************************
*/
void gpsDataProcess(u8* p)
{
	mixData_t mixData=*(mixData_t*)p;
	gpsx.latitude=mixData.latitude;
	gpsx.longitude=mixData.longitude;
	gpsx.gpssta=mixData.gpssta;
	gpsx.heading=mixData.heading;
	gpsx.speed=mixData.speed;
	MAG_PRH_Analysis(gpsx.heading);
}

/*
*********************************************************************************************************
*	函 数 名: headDataFiltrate
*	功能说明: 航向角数剧滤波
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void headDataFiltrate(void)
{
	u8 i=0;
	int32_t sum=0;
	
	headFifo.fifo[headFifo.index++]=headData.ecd_angle;
	if(headFifo.index==5)
		headFifo.index=0;
	for(i=0;i<5;i++)
		sum+=headFifo.fifo[i];
	headData.ecd_angle_fifo=sum/5;
}

/*
*********************************************************************************************************
*	函 数 名: inclinedAngle
*	功能说明: 两坐标与真北方向的夹角
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
float inclinedAngle(void)
{
	double LogA, LatA, LogB, LatB, a, b, Azimuth;
	LatA = gpsx.latitude*DEG2RAD;
	LogA = gpsx.longitude*DEG2RAD;
	LatB = gpsx.latdest*DEG2RAD;
	LogB = gpsx.logdest*DEG2RAD;
	a = sin(LatA)*sin(LatB) + cos(LatA)*cos(LatB)*cos(LogB - LogA);
	a = sqrt(1 - a*a);
	b = cos(LatB)*sin(LogB - LogA) / a;
	Azimuth = asin(b) * RAD2DEG;
	if (LatB > LatA)
	{
		if (LogB < LogA)//B点在第二象限
			Azimuth = 360 + Azimuth;
	}
	else
		Azimuth = 180 - Azimuth;//B点在第三象限或第四象限
	if(LatA==LatB)
	{
		if(LogB>LogA)Azimuth=90;
		else Azimuth=270;
	}
	if(LogB==LogA)
	{
		if(LatB>LatA)Azimuth=0;
		else Azimuth=180;
	}
	return Azimuth;
}

/*
*********************************************************************************************************
*	函 数 名: GetHeadBias
*	功能说明: 解析航向角
*	形    参：heading,航向角
*	返 回 值: 无
*********************************************************************************************************
*/
void GetHeadBias(u16 heading)
{	
	headData.last_raw_value=headData.raw_value;
	headData.raw_value=heading;
	headData.diff=headData.raw_value-headData.last_raw_value;
	if(headData.diff>30000)
		headData.round_cnt--;
	else if(headData.diff < -30000)
		headData.round_cnt++;
	headData.ecd_angle=headData.raw_value-headData.ecd_bias+ headData.round_cnt * 36000;
}

/*
*********************************************************************************************************
*	函 数 名: Gpsdest_Show
*	功能说明: 显示节点坐标
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Gpsdest_Show(void)
{
	char str[16];
	
	sprintf(str,"%.8f",gpsx.latdest);
	LCD_ShowString(60,200,125,24,24,(u8*)str);
	sprintf(str,"%.8f",gpsx.logdest);
	LCD_ShowString(200,200,135,24,24,(u8*)str);
}

/*
*********************************************************************************************************
*	函 数 名: updateHeadbias
*	功能说明: 更新航向基准角
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void updateHeadbias(void)
{
	if(gpsx.latitude!=gpsx.latcompare||gpsx.longitude!=gpsx.logcompare)
	{
		if(gpsx.count<20)
		{
			routes.index=0;
			gpsx.latdest=routes.latitude;
			gpsx.logdest=routes.longitude;
			GetHeadBias(inclinedAngle()*100);
			headDataFiltrate();
			gpsx.ecd_angle=(magData.ecd_angle-headData.ecd_angle-GpsCali.headOffset)/100.0;
			gpsx.count++;
			Gpsdest_Show();
		}
		else
		{
			gpsx.latcompare=gpsx.latitude;
			gpsx.logcompare=gpsx.longitude;
			if(gpsRange(&gpsx.latitude,&gpsx.latdest)&&gpsRange(&gpsx.longitude,&gpsx.logdest))
			{
				if(IsRouteDataFlagSet(GPS_Array_Interior))
				{
					if(IsRouteDataFlagSet(GPS_Array_External))
					{
						if((routes.coors[1][2*routes.index]==0)&&(routes.coors[1][2*routes.index+1]==0))
						{
							Reset_RouteData_Flagt();
							return;
						}
						gpsx.latdest+=routes.coors[1][2*routes.index]/GPS_COORS_STICK_OFFSET;
						gpsx.logdest+=routes.coors[1][2*routes.index+1]/GPS_COORS_STICK_OFFSET;
					}
					else
					{
						if((routes.coors[0][2*routes.index]==0)&&(routes.coors[0][2*routes.index+1]==0))
						{
							Reset_RouteData_Flagt();
							return;
						}
						gpsx.latdest+=routes.coors[0][2*routes.index]/GPS_COORS_STICK_OFFSET;
						gpsx.logdest+=routes.coors[0][2*routes.index+1]/GPS_COORS_STICK_OFFSET;
					}		
					routes.index++;
					if(routes.index==10)
					{
						routes.index=0;
						if(IsRouteDataFlagSet(GPS_Array_External))
						{
							ResetRouteDataFlag(GPS_Array_External);
							memset((int16_t*)routes.coors[1],0,20*sizeof(int16_t));
						}
						else
						{
							SetRouteDataFlag(GPS_Array_External);
							memset((int16_t*)routes.coors[0],0,20*sizeof(int16_t));
						}
						requestRoutesData();
					}
					ResetRouteDataFlag(GPS_Array_Interior);
				}
				Gpsdest_Show();
			}
			else
				SetRouteDataFlag(GPS_Array_Interior);
				
			GetHeadBias(inclinedAngle()*100);
			headDataFiltrate();
			gpsx.ecd_angle=(magData.ecd_angle-headData.ecd_angle_fifo-GpsCali.headOffset)/100.0;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: gpsTask
*	功能说明: GPS路径数剧处理任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void gpsTask(void)
{
	if(IsRouteDataFlagSet(GPS_RouteData_Init))
		updateHeadbias();
}

/*
*********************************************************************************************************
*	函 数 名: Gps_Show
*	功能说明: 显示GPS解算后的数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Gps_Show(void)
{
	char str[16];
	
	sprintf(str,"%.8f",gpsx.latitude);
	LCD_ShowString(60,125,125,24,24,(u8*)str);
	sprintf(str,"%.8f",gpsx.longitude);
	LCD_ShowString(260,125,135,24,24,(u8*)str);
	switch (gpsx.gpssta)
	{
		case 0:
				Show_Str(410,125,100,24,"无效",24,0);
			break;
		case 1:
				Show_Str(410,125,100,24,"单点",24,0);
			break;
		case 2:
				Show_Str(410,125,100,24,"伪距",24,0);
			break;
		case 4:
				Show_Str(410,125,100,24,"固定",24,0);
			break;
		case 5:
				Show_Str(410,125,100,24,"浮动",24,0);
			break;
		default:
			break;
	} 
	sprintf(str,"%.2f",gpsx.speed/100.0);
	LCD_ShowString(60,225,80,24,24,(u8*)str);
	
	sprintf(str,"%.2f  ",headData.ecd_angle/100.0);
	LCD_ShowString(270,150,100,24,24,(u8*)str);
	sprintf(str,"%.2f  ",gpsx.ecd_angle);
	LCD_ShowString(270,175,100,24,24,(u8*)str);
}	 

/*
*********************************************************************************************************
*	函 数 名: GPSHeadOffset_Show
*	功能说明: 显示设置的修正磁偏角
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void GPSHeadOffset_Show(void)
{
	char str[10];
	sprintf(str,"%.2f   ",GpsCali.headOffset/100.0);
	LCD_ShowString(85,175,75,24,24,(u8*)str);
}

