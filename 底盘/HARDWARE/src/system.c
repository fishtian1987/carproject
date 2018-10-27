/*
*********************************************************************************************************
*	                                  
*	ģ������ : Ӳ����ʼ��ģ��
*	�ļ����� : system.c
*	��    �� : V1.3
*	˵    �� : Ӳ����ʼ��ģ��
*
*
*********************************************************************************************************
*/

#include "system.h"	

/*
*********************************************************************************************************
*	�� �� ��: systemInit
*	����˵��: �ײ�Ӳ����ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
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
	
//	while(SD_Init())//��ⲻ��SD��
//	{
//		LCD_ShowString(30,150,200,24,24,"SD Card Error!");
//		delay_ms(300);	
//		BEEP=!BEEP;		
//		LCD_ShowString(30,150,200,24,24,"Please Check! ");
//		delay_ms(300);
//		BEEP=!BEEP;
//	}
	
	exfuns_init();					//Ϊfatfs��ر��������ڴ�		
	f_mount(fs[0],"0:",1); 		//����SD��  
	f_mount(fs[1],"1:",1); 		//���ع���FLASH. 
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
	
	Show_Str(0,0,80,24,"ת��:",24,0);
	Show_Str(170,0,80,24,"ת��:",24,0);
	Show_Str(0,25,100,24,"�ٶ�  P:",24,0);
	Show_Str(190,25,25,24,"I:",24,0);
	Show_Str(310,25,25,24,"D:",24,0);
	Show_Str(0,50,100,24,"ת��  P:",24,0);
	Show_Str(190,50,25,24,"I:",24,0);
	Show_Str(310,50,25,24,"D:",24,0);
	Show_Str(0,75,45,24,"FB:",24,0);
	Show_Str(95,75,45,24,"LR:",24,0);
	Show_Str(0,100,60,24,"����:",24,0);
	Show_Str(135,100,60,24,"����:",24,0);
	Show_Str(270,100,60,24,"����:",24,0);
	Show_Str(0,125,80,24,"γ��:",24,0);		
	Show_Str(200,125,100,24,"����:",24,0); 
	Show_Str(0,150,100,24,"����:",24,0);
	Show_Str(160,150,120,24,"��׼����:",24,0);
	Show_Str(0,175,100,24,"������:",24,0);
	Show_Str(185,175,100,24,"ƫ���:",24,0);
	Show_Str(0,200,60,24,"�ڵ�:",24,0);	
	Show_Str(365,200,120,24,"λ����ֵ",24,0);	
	Show_Str(0,225,60,24,"�ٶ�:",24,0);
	
	ControtLoopTaskInit(); //���������ʼ��
	RemoteTaskInit();//ң���������ʼ��	
	configParamInit();
}
