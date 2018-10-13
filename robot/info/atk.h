#ifndef ATK_H
#define ATK_H

#include <QString>

#define  PI 3.1415926
#define  BYTE0(dwTemp)  ( *( (uint8_t *)(&dwTemp)	)  )
#define  BYTE1(dwTemp)  ( *( (uint8_t *)(&dwTemp) + 1) )
#define  BYTE2(dwTemp)  ( *( (uint8_t *)(&dwTemp) + 2) )
#define  BYTE3(dwTemp)  ( *( (uint8_t *)(&dwTemp) + 3) )
#define  BYTE4(dwTemp)  ( *( (uint8_t *)(&dwTemp) + 4) )

class Atk
{
public:
    Atk();
    enum CD{
       D_CMAINCONTROL=0x01,
       D_CSUBCONTROL=0x02,
       D_C4GCONTROL=0x03,
       D_CGPSCONTROL=0x04,
       INVALID=0xFF
    };

    enum Head{
        UP_BYTE1=0xAA,
        UP_BYTE2=0xAA,
        DOWN_BYTE1=0xAA,
        DOWN_BYTE2=0xAF
    };

    enum Cmd_Type{
        PREPARE_STATE=0x00,
        C_ROUTE_BIAS=0x01,
        C_ROUTE=0x02,
        C_COORD=0x03,
        C_MAGBIAS=0x04,
        C_LOCAVPT=0x05,
        C_PID=0x06,
        C_REMOTESPEED=0x07,
        C_AUTOSPEED=0x08,
        C_DETECTSPEED=0x09
    };

    enum Cmd4G_Type{
        C_POINT_4G=0x01,
        C_PASS_4G=0x02,
        C_CARSTATE_4G=0x03
    };

    enum CmdGPS_Type{
        C_PASS_GPS=0x01
    };

    static unsigned int RADIUS;
    static float DEG2RAD;
    static unsigned short INF;
};

#endif // ATK_H
