/*
*********************************************************************************************************
*	                                  
*	模块名称 : 硬件初始化模块
*	文件名称 : system.c
*	版    本 : V1.3
*	说    明 : 硬件初始化模块
*
*
*********************************************************************************************************
*/

#include "system.h"	

/*
*********************************************************************************************************
*	函 数 名: systemInit
*	功能说明: 底层硬件初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void systemInit(void)
{
	//char *str;
	nvicInit();		
	//extiInit();		
	delay_init(168);	
	ledInit();
	BEEP_Init();
	MotorsInit();
	W25QXX_Init();
	My_RTC_Init();
	LCD_Init();
	POINT_COLOR=WHITE;
	BACK_COLOR=BLUE;
	UsartRemoteInit(); 
	uartDebugInit();
	maslinkInit();	
	armlinkInit();
	atkpInit();	
	
//	while(SD_Init())//检测不到SD卡
//	{
//		LCD_ShowString(30,150,200,24,24,"SD Card Error!");
//		delay_ms(300);	
//		BEEP=!BEEP;		
//		LCD_ShowString(30,150,200,24,24,"Please Check! ");
//		delay_ms(300);
//		BEEP=!BEEP;
//	}
	
	exfuns_init();					//为fatfs相关变量申请内存		
	f_mount(fs[0],"0:",1); 		//挂载SD卡  
	f_mount(fs[1],"1:",1); 		//挂载挂载FLASH. 
	while(font_init()) 
	{    	   	  				 						    
		LCD_ShowString(30,150,200,24,24,"Font Updating...");
		while(update_font(30,150,24,"0:"))
		{			 		  
			LCD_ShowString(30,150,200,24,24,"Font Update Failed!");
			delay_ms(200);
			BEEP=!BEEP;
			LCD_Fill(20,150,200+20,110+24,BLUE);
			delay_ms(200);
			BEEP=!BEEP;			
		} 		  
		LCD_ShowString(30,150,200,24,24,"Font Update Success! ");
		delay_ms(1500);	
		LCD_Clear(BLUE);	       
	}  
	
//	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);	
//	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
//	filename=pvPortMalloc(30);
//	str=pvPortMalloc(25);
//	sprintf(str,"20%02d-%02d-%02d %02d-%02d-%02d.txt",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,
//																										RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
//	path_name(filename,"0:",str);  
//	while(f_open (&fil,filename, FA_CREATE_ALWAYS|FA_WRITE)!=0)
//	{
//		LCD_ShowString(30,150,1500,24,24,"Failed to create file!");
//		delay_ms(300);	
//		BEEP=!BEEP;		
//		LCD_ShowString(30,150,1500,24,24,"Please Check!         ");
//		delay_ms(300);
//		BEEP=!BEEP;
//	}
//	f_close(&fil);
//	vPortFree(str);
	LCD_Clear(BLUE);
	
	Show_Str(0,0,80,24,"转速:",24,0);
	Show_Str(170,0,80,24,"转向:",24,0);
	Show_Str(0,25,100,24,"速度  P:",24,0);
	Show_Str(190,25,25,24,"I:",24,0);
	Show_Str(310,25,25,24,"D:",24,0);
	Show_Str(0,50,100,24,"转向  P:",24,0);
	Show_Str(190,50,25,24,"I:",24,0);
	Show_Str(310,50,25,24,"D:",24,0);
	Show_Str(0,75,45,24,"FB:",24,0);
	Show_Str(95,75,45,24,"LR:",24,0);
	Show_Str(0,100,60,24,"低速:",24,0);
	Show_Str(135,100,60,24,"中速:",24,0);
	Show_Str(270,100,60,24,"高速:",24,0);
	Show_Str(0,125,80,24,"纬度:",24,0);		
	Show_Str(200,125,100,24,"经度:",24,0); 
	Show_Str(0,150,100,24,"航向:",24,0);
	Show_Str(160,150,120,24,"基准航向:",24,0);
	Show_Str(0,175,100,24,"修正角:",24,0);
	Show_Str(185,175,100,24,"偏差角:",24,0);
	Show_Str(0,200,60,24,"节点:",24,0);	
	Show_Str(365,200,120,24,"位置阈值",24,0);	
	Show_Str(0,225,60,24,"速度:",24,0);
	
	ControtLoopTaskInit(); //控制任务初始化
	RemoteTaskInit();//遥控器任务初始化	
	configParamInit();
}
