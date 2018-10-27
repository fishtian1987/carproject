/*
*********************************************************************************************************
*	                                  
*	模块名称 : 处理GPS数据
*	文件名称 : gps.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __GPS_H
#define __GPS_H
#include "sys.h"
#include "atkp.h"
#include "stdbool.h"

typedef struct  
{										    
	double latitude;		
	double longitude;	
	double latdest;
	double logdest;
	double latcompare;
	double logcompare;	
	float ecd_angle;
	u8 gpssta;
	u16 heading;
	u16	speed;
	u8 count;
}nmea_msg; 

typedef __packed struct
{
	double latitude;	
	double longitude;			 
	u8 gpssta;	
	u16 heading;
	u16	speed;
} mixData_t;

typedef __packed struct
{
	int16_t headOffset; 
	u16 gpsBias;
}GpsCaliStruct_t;

typedef __packed struct  
{										    
	uint16_t raw_value;
	uint16_t last_raw_value;
	uint16_t ecd_bias;
	int32_t diff;
	int32_t ecd_angle;
	int32_t ecd_angle_fifo;
	int8_t round_cnt;
}head_t; 

#define GPS_COORS_STICK_OFFSET  100000000.0

#define	GPS_Array_Interior ((u8)1<<1)
#define	GPS_Array_External ((u8)1<<2)
#define GPS_RouteData_Init ((u8)1<<3)

extern nmea_msg gpsx;
extern GpsCaliStruct_t GpsCali;

void gpsDataProcess(u8* p);
void gpsTask(void);
void GPSBaseLocation(u8* p);
void GPSRoutesData(atkp_t *packet);
void gpsHeadCoord(u8* p);
void GPSHeadOffsetProcess(u8* p);
void GPSBiasProcess(u8* p);
bool IsRouteDataFlagSet(u8 flag);
void Gps_Show(void);
void GPSHeadOffset_Show(void);
#endif 
