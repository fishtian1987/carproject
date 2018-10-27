/*
*********************************************************************************************************
*	                                  
*	ģ������ : GPS����ģ��
*	�ļ����� : gps.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*********************************************************************************************************
*/

#ifndef __GPS_H
#define __GPS_H	 
#include "sys.h"  
#include "stdbool.h"  

__packed typedef struct  
{			
	double  latitude;	//γ��
	double  longitude;//���� 
	u8 gpssta;				//GPS״̬:0,��Ч��;1,���㶨λ��;2,α����;4,�̶���;5,������.	
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
