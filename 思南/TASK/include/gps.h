/*
*********************************************************************************************************
*	                                  
*	模块名称 : GPS驱动模块
*	文件名称 : gps.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __GPS_H
#define __GPS_H	 
#include "sys.h"  
#include "stdbool.h"  

__packed typedef struct  
{			
	double  latitude;	//纬度
	double  longitude;//经度 
	u8 gpssta;				//GPS状态:0,无效解;1,单点定位解;2,伪距差分;4,固定解;5,浮动解.	
	u8 posslnum;
	float heading;
	//float pitch;
	//float roll;
	float speed;
	bool bflagProce;
}nmea_msg; 

extern nmea_msg gpsx;
void NMEA_Analysis(nmea_msg *gpsx,u8 *buf);
#endif  
