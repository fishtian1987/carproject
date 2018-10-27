/*
*********************************************************************************************************
*	                                  
*	ģ������ : ����GPS����
*	�ļ����� : gps.c
*	��    �� : V1.3
*	˵    �� : У������
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

static u8 bflagRoutesData=0;//1:��������ת�� 2���������ת�� 3:����׼��

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
*	�� �� ��: SetRouteDataFlag
*	����˵��: �������ݱ�־λ
*	��    �Σ�flag,��־λ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SetRouteDataFlag(u8 flag)  
{
	bflagRoutesData |= flag;
}

/*
*********************************************************************************************************
*	�� �� ��: ResetRouteDataFlag
*	����˵��: ȡ�����ݱ�־λ
*	��    �Σ�flag,��־λ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ResetRouteDataFlag(u8 flag)
{
	bflagRoutesData &= ~flag;
}

/*
*********************************************************************************************************
*	�� �� ��: IsRouteDataFlagSet
*	����˵��: ��ѯĳһλ���ݱ�־λ
*	��    �Σ�flag,����ѯ��־λ
*	�� �� ֵ: 0,δ����
						1,����
*********************************************************************************************************
*/
bool IsRouteDataFlagSet(u8 flag)
{
	if(flag & bflagRoutesData)return true;
	else return false;	
}

/*
*********************************************************************************************************
*	�� �� ��: Reset_RouteData_Flagt
*	����˵��: ��λ�������ݱ�־λ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Reset_RouteData_Flagt(void)
{
	bflagRoutesData=0;
	gpsx.count=0;
}
/*
*********************************************************************************************************
*	�� �� ��: GPSBaseLocation
*	����˵��: ����·����׼ֵ
*	��    �Σ�p,·��ֵ;
*	�� �� ֵ: ��
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
*	�� �� ��: GPSRoutesData
*	����˵��: ����·����ֵ
*	��    �Σ�packet,���ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: GPSHeadOffsetProcess
*	����˵��: ���º�������ֵ
*	��    �Σ�p,����ֵ;
*	�� �� ֵ: ��
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
*	�� �� ��: GPSBiasProcess
*	����˵��: ����GPSƫ�Χ
*	��    �Σ�p,У׼ֵ;
*	�� �� ֵ: ��
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
*	�� �� ��: gpsRange
*	����˵��: gps�Ƿ񵽴�ָ��������
*	��    �Σ�movdot,���Ƚϵ�
				 targetdot,Ŀ���
*	�� �� ֵ: true,����λ��
						false,δ��λ��
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
*	�� �� ��: gpsDataProcess
*	����˵��: ����GPS����
*	��    �Σ�pk,���ݰ�
*	�� �� ֵ: ��
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
*	�� �� ��: headDataFiltrate
*	����˵��: ����������˲�
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: inclinedAngle
*	����˵��: ���������汱����ļн�
*	��    �Σ���
*	�� �� ֵ: ��
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
		if (LogB < LogA)//B���ڵڶ�����
			Azimuth = 360 + Azimuth;
	}
	else
		Azimuth = 180 - Azimuth;//B���ڵ������޻��������
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
*	�� �� ��: GetHeadBias
*	����˵��: ���������
*	��    �Σ�heading,�����
*	�� �� ֵ: ��
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
*	�� �� ��: Gpsdest_Show
*	����˵��: ��ʾ�ڵ�����
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: updateHeadbias
*	����˵��: ���º����׼��
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: gpsTask
*	����˵��: GPS·�����紦������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void gpsTask(void)
{
	if(IsRouteDataFlagSet(GPS_RouteData_Init))
		updateHeadbias();
}

/*
*********************************************************************************************************
*	�� �� ��: Gps_Show
*	����˵��: ��ʾGPS����������
*	��    �Σ���
*	�� �� ֵ: ��
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
				Show_Str(410,125,100,24,"��Ч",24,0);
			break;
		case 1:
				Show_Str(410,125,100,24,"����",24,0);
			break;
		case 2:
				Show_Str(410,125,100,24,"α��",24,0);
			break;
		case 4:
				Show_Str(410,125,100,24,"�̶�",24,0);
			break;
		case 5:
				Show_Str(410,125,100,24,"����",24,0);
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
*	�� �� ��: GPSHeadOffset_Show
*	����˵��: ��ʾ���õ�������ƫ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GPSHeadOffset_Show(void)
{
	char str[10];
	sprintf(str,"%.2f   ",GpsCali.headOffset/100.0);
	LCD_ShowString(85,175,75,24,24,(u8*)str);
}

