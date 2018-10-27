/*
*********************************************************************************************************
*	                                  
*	Ä£¿éÃû³Æ : LCD´¦Àí
*	ÎÄ¼þÃû³Æ : lcd.c
*	°æ    ±¾ : V1.3
*	Ëµ    Ã÷ : LCD´¦Àí
*
*
*********************************************************************************************************
*/

#include "lcd.h"
#include "font.h" 
#include "delay.h"	 
  
u16 POINT_COLOR=0x0000;	
u16 BACK_COLOR=0xFFFF;  

_lcd_dev lcddev;
	 
/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_WR_REG
*	¹¦ÄÜËµÃ÷: Ð´¼Ä´æÆ÷º¯Êý
*	ÐÎ    ²Î£ºregval:¼Ä´æÆ÷Öµ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_WR_REG(vu16 regval)
{   
	LCD->LCD_REG=regval;	 
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_WR_DATA
*	¹¦ÄÜËµÃ÷: Ð´LCDÊý¾Ý
*	ÐÎ    ²Î£ºdata:ÒªÐ´ÈëµÄÖµ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_WR_DATA(vu16 data)
{	  
	LCD->LCD_RAM=data;	 
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_RD_DATA
*	¹¦ÄÜËµÃ÷: ¶ÁLCDÊý¾Ý
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ¶Áµ½µÄÖµ
*********************************************************************************************************
*/
u16 LCD_RD_DATA(void)
{
	vu16 ram;		
	ram=LCD->LCD_RAM;	
	return ram;	 
}					   

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_WriteReg
*	¹¦ÄÜËµÃ÷: Ð´¼Ä´æÆ÷
*	ÐÎ    ²Î£ºLCD_Reg:¼Ä´æÆ÷µØÖ·
						LCD_RegValue:ÒªÐ´ÈëµÄÊý¾Ý
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		
	LCD->LCD_RAM = LCD_RegValue;		 
}	   

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ReadReg
*	¹¦ÄÜËµÃ÷: ¶Á¼Ä´æÆ÷
*	ÐÎ    ²Î£ºLCD_Reg:¼Ä´æÆ÷µØÖ·
*	·µ »Ø Öµ: ¶Áµ½µÄÊý¾Ý
*********************************************************************************************************
*/
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		
	delay_us(5);		  
	return LCD_RD_DATA();
}   

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_WriteRAM_Prepare
*	¹¦ÄÜËµÃ÷: ¿ªÊ¼Ð´GRAM
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_WriteRAM
*	¹¦ÄÜËµÃ÷: LCDÐ´GRAM£
*	ÐÎ    ²Î£ºRGB_Code:ÑÕÉ«Öµ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//Ð´Ê®ÁùÎ»GRAM
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_BGR2RGB
*	¹¦ÄÜËµÃ÷: ´ÓILI93xx¶Á³öµÄÊý¾ÝÎªGBR¸ñÊ½£¬¶øÎÒÃÇÐ´ÈëµÄÊ±ºòÎªRGB¸ñÊ½¡£
						Í¨¹ý¸Ãº¯Êý×ª»»
*	ÐÎ    ²Î£ºc:GBR¸ñÊ½µÄÑÕÉ«Öµ
*	·µ »Ø Öµ: RGB¸ñÊ½µÄÑÕÉ«Öµ
*********************************************************************************************************
*/
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: opt_delay
*	¹¦ÄÜËµÃ÷: µ±mdk -O1Ê±¼äÓÅ»¯Ê±ÐèÒªÉèÖÃ
*	ÐÎ    ²Î£ºÑÓÊ±i
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void opt_delay(u8 i)
{
	while(i--);
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_SetCursor
*	¹¦ÄÜËµÃ÷: ÉèÖÃ¹â±êÎ»ÖÃ
*	ÐÎ    ²Î£ºXpos:ºá×ø±ê
						Ypos:×Ý×ø±ê
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
	LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(Xpos>>8); 		
	LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(Xpos&0XFF);			 
	LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(Ypos>>8);  		
	LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(Ypos&0XFF);			
} 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ReadPoint
*	¹¦ÄÜËµÃ÷: ¶ÁÈ¡¸öÄ³µãµÄÑÕÉ«Öµ	 
*	ÐÎ    ²Î£ºx,y:×ø±ê
*	·µ »Ø Öµ: ´ËµãµÄÑÕÉ«
*********************************************************************************************************
*/
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	
	LCD_SetCursor(x,y);	    
	LCD_WR_REG(0X2E00);	    
 	r=LCD_RD_DATA();							
	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						
	opt_delay(2);	  
	b=LCD_RD_DATA(); 
	g=r&0XFF;		
	g<<=8;	
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
}		 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_DisplayOn
*	¹¦ÄÜËµÃ÷: LCD¿ªÆôÏÔÊ¾
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X2900);					 	
}	 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_DisplayOff
*	¹¦ÄÜËµÃ÷: LCD¹Ø±ÕÏÔÊ¾
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X2800);	
}   		 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Scan_Dir
*	¹¦ÄÜËµÃ÷: ÉèÖÃLCDµÄ×Ô¶¯É¨Ãè·½Ïò
						×¢Òâ:ÆäËûº¯Êý¿ÉÄÜ»áÊÜµ½´Ëº¯ÊýÉèÖÃµÄÓ°Ïì(ÓÈÆäÊÇ9341/6804ÕâÁ½¸öÆæÝâ),
						ËùÒÔ,Ò»°ãÉèÖÃÎªL2R_U2D¼´¿É,Èç¹ûÉèÖÃÎªÆäËûÉ¨Ãè·½Ê½,¿ÉÄÜµ¼ÖÂÏÔÊ¾²»Õý³£.
*	ÐÎ    ²Î£ºdir:0~7,´ú±í8¸ö·½Ïò(¾ßÌå¶¨Òå¼ûlcd.h)
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  

	if(lcddev.dir==1)
	{			   
		switch(dir)
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	switch(dir)
	{
		case L2R_U2D:
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U:
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D:
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U:
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R:
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L:
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R:
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L:
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X3600;
	LCD_WriteReg(dirreg,regval);
		
	if(regval&0X20)
	{
		if(lcddev.width<lcddev.height)
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}
	else  
	{
		if(lcddev.width>lcddev.height)
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}  

	LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
	LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
	LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
	LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
	LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
	LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
	LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
	LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);	
}     

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_DrawPoint
*	¹¦ÄÜËµÃ÷: »­µã
*	ÐÎ    ²Î£ºx,y:×ø±ê
						POINT_COLOR:ÑÕÉ«
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		
	LCD_WriteRAM_Prepare();
	LCD->LCD_RAM=POINT_COLOR; 
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Fast_DrawPoint
*	¹¦ÄÜËµÃ÷: ¿ìËÙ»­µã
*	ÐÎ    ²Î£ºx,y:×ø±ê
						color:ÑÕÉ«
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);  
	LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);	  
	LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);  
	LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF);  
	LCD->LCD_REG=lcddev.wramcmd; 
	LCD->LCD_RAM=color; 
}	 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_SSD_BackLightSet
*	¹¦ÄÜËµÃ÷: ±³¹âÉèÖÃ
*	ÐÎ    ²Î£ºpwm:±³¹âµÈ¼¶,0~100.Ô½´óÔ½ÁÁ.
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_SSD_BackLightSet(u8 pwm)
{	
	LCD_WR_REG(0xBE);	
	LCD_WR_DATA(0x05);	
	LCD_WR_DATA(pwm*2.55);
	LCD_WR_DATA(0x01);	
	LCD_WR_DATA(0xFF);	
	LCD_WR_DATA(0x00);	
	LCD_WR_DATA(0x00);	
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Display_Dir
*	¹¦ÄÜËµÃ÷: ÉèÖÃLCDÏÔÊ¾·½Ïò
*	ÐÎ    ²Î£ºdir:0,ÊúÆÁ£»1,ºáÆÁ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			
	{
		lcddev.dir=0;	
		lcddev.wramcmd=0X2C00;
		lcddev.setxcmd=0X2A00;
		lcddev.setycmd=0X2B00; 
		lcddev.width=480;
		lcddev.height=800;		
	}
	else 				
	{	  				
		lcddev.dir=1;	
		lcddev.wramcmd=0X2C00;
		lcddev.setxcmd=0X2A00;
		lcddev.setycmd=0X2B00; 
		lcddev.width=800;
		lcddev.height=480;
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	
}	 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Set_Window
*	¹¦ÄÜËµÃ÷: ÉèÖÃ´°¿Ú,²¢×Ô¶¯ÉèÖÃ»­µã×ø±êµ½´°¿Ú×óÉÏ½Ç(sx,sy)
*	ÐÎ    ²Î£ºsx,sy:´°¿ÚÆðÊ¼×ø±ê(×óÉÏ½Ç)
						width,height:´°¿Ú¿í¶ÈºÍ¸ß¶È,±ØÐë´óÓÚ0!!
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	
	LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);  
	LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);	  
	LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(twidth>>8);   
	LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(twidth&0XFF);   
	LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);   
	LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);  
	LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(theight>>8);   
	LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(theight&0XFF);  
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Init
*	¹¦ÄÜËµÃ÷: ³õÊ¼»¯lcd
*	ÐÎ    ²Î£ºÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Init(void)
{ 	
	vu32 i=0;
	
  GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);
 
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);
 
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

  readWriteTiming.FSMC_AddressSetupTime = 0XF;	 
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 
  readWriteTiming.FSMC_DataSetupTime = 60;		
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 
    
	writeTiming.FSMC_AddressSetupTime =9;	      
  writeTiming.FSMC_AddressHoldTime = 0x00;	
  writeTiming.FSMC_DataSetupTime = 8;		 
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;  
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;
  
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
		
 	delay_ms(50); 
 	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50);
  lcddev.id = LCD_ReadReg(0x0000);   
	
	LCD_WR_REG(0XDA00);	
	lcddev.id=LCD_RD_DATA();		 
	LCD_WR_REG(0XDB00);	
	lcddev.id=LCD_RD_DATA();		
	lcddev.id<<=8;	
	LCD_WR_REG(0XDC00);	
	lcddev.id|=LCD_RD_DATA();		
									
	FSMC_Bank1E->BWTR[6]&=~(0XF<<0); 
	FSMC_Bank1E->BWTR[6]&=~(0XF<<8);
	FSMC_Bank1E->BWTR[6]|=3<<0;
	FSMC_Bank1E->BWTR[6]|=2<<8;

	LCD_WriteReg(0xF000,0x55);
	LCD_WriteReg(0xF001,0xAA);
	LCD_WriteReg(0xF002,0x52);
	LCD_WriteReg(0xF003,0x08);
	LCD_WriteReg(0xF004,0x01);
	LCD_WriteReg(0xB000,0x0D);
	LCD_WriteReg(0xB001,0x0D);
	LCD_WriteReg(0xB002,0x0D);
	LCD_WriteReg(0xB600,0x34);
	LCD_WriteReg(0xB601,0x34);
	LCD_WriteReg(0xB602,0x34);
	LCD_WriteReg(0xB100,0x0D);
	LCD_WriteReg(0xB101,0x0D);
	LCD_WriteReg(0xB102,0x0D);
	LCD_WriteReg(0xB700,0x34);
	LCD_WriteReg(0xB701,0x34);
	LCD_WriteReg(0xB702,0x34);
	LCD_WriteReg(0xB200,0x00);
	LCD_WriteReg(0xB201,0x00);
	LCD_WriteReg(0xB202,0x00);
	LCD_WriteReg(0xB800,0x24);
	LCD_WriteReg(0xB801,0x24);
	LCD_WriteReg(0xB802,0x24);
	LCD_WriteReg(0xBF00,0x01);
	LCD_WriteReg(0xB300,0x0F);
	LCD_WriteReg(0xB301,0x0F);
	LCD_WriteReg(0xB302,0x0F);
	LCD_WriteReg(0xB900,0x34);
	LCD_WriteReg(0xB901,0x34);
	LCD_WriteReg(0xB902,0x34);
	LCD_WriteReg(0xB500,0x08);
	LCD_WriteReg(0xB501,0x08);
	LCD_WriteReg(0xB502,0x08);
	LCD_WriteReg(0xC200,0x03);
	LCD_WriteReg(0xBA00,0x24);
	LCD_WriteReg(0xBA01,0x24);
	LCD_WriteReg(0xBA02,0x24);
	LCD_WriteReg(0xBC00,0x00);
	LCD_WriteReg(0xBC01,0x78);
	LCD_WriteReg(0xBC02,0x00);
	LCD_WriteReg(0xBD00,0x00);
	LCD_WriteReg(0xBD01,0x78);
	LCD_WriteReg(0xBD02,0x00);
	LCD_WriteReg(0xBE00,0x00);
	LCD_WriteReg(0xBE01,0x64);
	LCD_WriteReg(0xD100,0x00);
	LCD_WriteReg(0xD101,0x33);
	LCD_WriteReg(0xD102,0x00);
	LCD_WriteReg(0xD103,0x34);
	LCD_WriteReg(0xD104,0x00);
	LCD_WriteReg(0xD105,0x3A);
	LCD_WriteReg(0xD106,0x00);
	LCD_WriteReg(0xD107,0x4A);
	LCD_WriteReg(0xD108,0x00);
	LCD_WriteReg(0xD109,0x5C);
	LCD_WriteReg(0xD10A,0x00);
	LCD_WriteReg(0xD10B,0x81);
	LCD_WriteReg(0xD10C,0x00);
	LCD_WriteReg(0xD10D,0xA6);
	LCD_WriteReg(0xD10E,0x00);
	LCD_WriteReg(0xD10F,0xE5);
	LCD_WriteReg(0xD110,0x01);
	LCD_WriteReg(0xD111,0x13);
	LCD_WriteReg(0xD112,0x01);
	LCD_WriteReg(0xD113,0x54);
	LCD_WriteReg(0xD114,0x01);
	LCD_WriteReg(0xD115,0x82);
	LCD_WriteReg(0xD116,0x01);
	LCD_WriteReg(0xD117,0xCA);
	LCD_WriteReg(0xD118,0x02);
	LCD_WriteReg(0xD119,0x00);
	LCD_WriteReg(0xD11A,0x02);
	LCD_WriteReg(0xD11B,0x01);
	LCD_WriteReg(0xD11C,0x02);
	LCD_WriteReg(0xD11D,0x34);
	LCD_WriteReg(0xD11E,0x02);
	LCD_WriteReg(0xD11F,0x67);
	LCD_WriteReg(0xD120,0x02);
	LCD_WriteReg(0xD121,0x84);
	LCD_WriteReg(0xD122,0x02);
	LCD_WriteReg(0xD123,0xA4);
	LCD_WriteReg(0xD124,0x02);
	LCD_WriteReg(0xD125,0xB7);
	LCD_WriteReg(0xD126,0x02);
	LCD_WriteReg(0xD127,0xCF);
	LCD_WriteReg(0xD128,0x02);
	LCD_WriteReg(0xD129,0xDE);
	LCD_WriteReg(0xD12A,0x02);
	LCD_WriteReg(0xD12B,0xF2);
	LCD_WriteReg(0xD12C,0x02);
	LCD_WriteReg(0xD12D,0xFE);
	LCD_WriteReg(0xD12E,0x03);
	LCD_WriteReg(0xD12F,0x10);
	LCD_WriteReg(0xD130,0x03);
	LCD_WriteReg(0xD131,0x33);
	LCD_WriteReg(0xD132,0x03);
	LCD_WriteReg(0xD133,0x6D);
	LCD_WriteReg(0xD200,0x00);
	LCD_WriteReg(0xD201,0x33);
	LCD_WriteReg(0xD202,0x00);
	LCD_WriteReg(0xD203,0x34);
	LCD_WriteReg(0xD204,0x00);
	LCD_WriteReg(0xD205,0x3A);
	LCD_WriteReg(0xD206,0x00);
	LCD_WriteReg(0xD207,0x4A);
	LCD_WriteReg(0xD208,0x00);
	LCD_WriteReg(0xD209,0x5C);
	LCD_WriteReg(0xD20A,0x00);
	LCD_WriteReg(0xD20B,0x81);
	LCD_WriteReg(0xD20C,0x00);
	LCD_WriteReg(0xD20D,0xA6);
	LCD_WriteReg(0xD20E,0x00);
	LCD_WriteReg(0xD20F,0xE5);
	LCD_WriteReg(0xD210,0x01);
	LCD_WriteReg(0xD211,0x13);
	LCD_WriteReg(0xD212,0x01);
	LCD_WriteReg(0xD213,0x54);
	LCD_WriteReg(0xD214,0x01);
	LCD_WriteReg(0xD215,0x82);
	LCD_WriteReg(0xD216,0x01);
	LCD_WriteReg(0xD217,0xCA);
	LCD_WriteReg(0xD218,0x02);
	LCD_WriteReg(0xD219,0x00);
	LCD_WriteReg(0xD21A,0x02);
	LCD_WriteReg(0xD21B,0x01);
	LCD_WriteReg(0xD21C,0x02);
	LCD_WriteReg(0xD21D,0x34);
	LCD_WriteReg(0xD21E,0x02);
	LCD_WriteReg(0xD21F,0x67);
	LCD_WriteReg(0xD220,0x02);
	LCD_WriteReg(0xD221,0x84);
	LCD_WriteReg(0xD222,0x02);
	LCD_WriteReg(0xD223,0xA4);
	LCD_WriteReg(0xD224,0x02);
	LCD_WriteReg(0xD225,0xB7);
	LCD_WriteReg(0xD226,0x02);
	LCD_WriteReg(0xD227,0xCF);
	LCD_WriteReg(0xD228,0x02);
	LCD_WriteReg(0xD229,0xDE);
	LCD_WriteReg(0xD22A,0x02);
	LCD_WriteReg(0xD22B,0xF2);
	LCD_WriteReg(0xD22C,0x02);
	LCD_WriteReg(0xD22D,0xFE);
	LCD_WriteReg(0xD22E,0x03);
	LCD_WriteReg(0xD22F,0x10);
	LCD_WriteReg(0xD230,0x03);
	LCD_WriteReg(0xD231,0x33);
	LCD_WriteReg(0xD232,0x03);
	LCD_WriteReg(0xD233,0x6D);
	LCD_WriteReg(0xD300,0x00);
	LCD_WriteReg(0xD301,0x33);
	LCD_WriteReg(0xD302,0x00);
	LCD_WriteReg(0xD303,0x34);
	LCD_WriteReg(0xD304,0x00);
	LCD_WriteReg(0xD305,0x3A);
	LCD_WriteReg(0xD306,0x00);
	LCD_WriteReg(0xD307,0x4A);
	LCD_WriteReg(0xD308,0x00);
	LCD_WriteReg(0xD309,0x5C);
	LCD_WriteReg(0xD30A,0x00);
	LCD_WriteReg(0xD30B,0x81);
	LCD_WriteReg(0xD30C,0x00);
	LCD_WriteReg(0xD30D,0xA6);
	LCD_WriteReg(0xD30E,0x00);
	LCD_WriteReg(0xD30F,0xE5);
	LCD_WriteReg(0xD310,0x01);
	LCD_WriteReg(0xD311,0x13);
	LCD_WriteReg(0xD312,0x01);
	LCD_WriteReg(0xD313,0x54);
	LCD_WriteReg(0xD314,0x01);
	LCD_WriteReg(0xD315,0x82);
	LCD_WriteReg(0xD316,0x01);
	LCD_WriteReg(0xD317,0xCA);
	LCD_WriteReg(0xD318,0x02);
	LCD_WriteReg(0xD319,0x00);
	LCD_WriteReg(0xD31A,0x02);
	LCD_WriteReg(0xD31B,0x01);
	LCD_WriteReg(0xD31C,0x02);
	LCD_WriteReg(0xD31D,0x34);
	LCD_WriteReg(0xD31E,0x02);
	LCD_WriteReg(0xD31F,0x67);
	LCD_WriteReg(0xD320,0x02);
	LCD_WriteReg(0xD321,0x84);
	LCD_WriteReg(0xD322,0x02);
	LCD_WriteReg(0xD323,0xA4);
	LCD_WriteReg(0xD324,0x02);
	LCD_WriteReg(0xD325,0xB7);
	LCD_WriteReg(0xD326,0x02);
	LCD_WriteReg(0xD327,0xCF);
	LCD_WriteReg(0xD328,0x02);
	LCD_WriteReg(0xD329,0xDE);
	LCD_WriteReg(0xD32A,0x02);
	LCD_WriteReg(0xD32B,0xF2);
	LCD_WriteReg(0xD32C,0x02);
	LCD_WriteReg(0xD32D,0xFE);
	LCD_WriteReg(0xD32E,0x03);
	LCD_WriteReg(0xD32F,0x10);
	LCD_WriteReg(0xD330,0x03);
	LCD_WriteReg(0xD331,0x33);
	LCD_WriteReg(0xD332,0x03);
	LCD_WriteReg(0xD333,0x6D);
	LCD_WriteReg(0xD400,0x00);
	LCD_WriteReg(0xD401,0x33);
	LCD_WriteReg(0xD402,0x00);
	LCD_WriteReg(0xD403,0x34);
	LCD_WriteReg(0xD404,0x00);
	LCD_WriteReg(0xD405,0x3A);
	LCD_WriteReg(0xD406,0x00);
	LCD_WriteReg(0xD407,0x4A);
	LCD_WriteReg(0xD408,0x00);
	LCD_WriteReg(0xD409,0x5C);
	LCD_WriteReg(0xD40A,0x00);
	LCD_WriteReg(0xD40B,0x81);
	LCD_WriteReg(0xD40C,0x00);
	LCD_WriteReg(0xD40D,0xA6);
	LCD_WriteReg(0xD40E,0x00);
	LCD_WriteReg(0xD40F,0xE5);
	LCD_WriteReg(0xD410,0x01);
	LCD_WriteReg(0xD411,0x13);
	LCD_WriteReg(0xD412,0x01);
	LCD_WriteReg(0xD413,0x54);
	LCD_WriteReg(0xD414,0x01);
	LCD_WriteReg(0xD415,0x82);
	LCD_WriteReg(0xD416,0x01);
	LCD_WriteReg(0xD417,0xCA);
	LCD_WriteReg(0xD418,0x02);
	LCD_WriteReg(0xD419,0x00);
	LCD_WriteReg(0xD41A,0x02);
	LCD_WriteReg(0xD41B,0x01);
	LCD_WriteReg(0xD41C,0x02);
	LCD_WriteReg(0xD41D,0x34);
	LCD_WriteReg(0xD41E,0x02);
	LCD_WriteReg(0xD41F,0x67);
	LCD_WriteReg(0xD420,0x02);
	LCD_WriteReg(0xD421,0x84);
	LCD_WriteReg(0xD422,0x02);
	LCD_WriteReg(0xD423,0xA4);
	LCD_WriteReg(0xD424,0x02);
	LCD_WriteReg(0xD425,0xB7);
	LCD_WriteReg(0xD426,0x02);
	LCD_WriteReg(0xD427,0xCF);
	LCD_WriteReg(0xD428,0x02);
	LCD_WriteReg(0xD429,0xDE);
	LCD_WriteReg(0xD42A,0x02);
	LCD_WriteReg(0xD42B,0xF2);
	LCD_WriteReg(0xD42C,0x02);
	LCD_WriteReg(0xD42D,0xFE);
	LCD_WriteReg(0xD42E,0x03);
	LCD_WriteReg(0xD42F,0x10);
	LCD_WriteReg(0xD430,0x03);
	LCD_WriteReg(0xD431,0x33);
	LCD_WriteReg(0xD432,0x03);
	LCD_WriteReg(0xD433,0x6D);
	LCD_WriteReg(0xD500,0x00);
	LCD_WriteReg(0xD501,0x33);
	LCD_WriteReg(0xD502,0x00);
	LCD_WriteReg(0xD503,0x34);
	LCD_WriteReg(0xD504,0x00);
	LCD_WriteReg(0xD505,0x3A);
	LCD_WriteReg(0xD506,0x00);
	LCD_WriteReg(0xD507,0x4A);
	LCD_WriteReg(0xD508,0x00);
	LCD_WriteReg(0xD509,0x5C);
	LCD_WriteReg(0xD50A,0x00);
	LCD_WriteReg(0xD50B,0x81);
	LCD_WriteReg(0xD50C,0x00);
	LCD_WriteReg(0xD50D,0xA6);
	LCD_WriteReg(0xD50E,0x00);
	LCD_WriteReg(0xD50F,0xE5);
	LCD_WriteReg(0xD510,0x01);
	LCD_WriteReg(0xD511,0x13);
	LCD_WriteReg(0xD512,0x01);
	LCD_WriteReg(0xD513,0x54);
	LCD_WriteReg(0xD514,0x01);
	LCD_WriteReg(0xD515,0x82);
	LCD_WriteReg(0xD516,0x01);
	LCD_WriteReg(0xD517,0xCA);
	LCD_WriteReg(0xD518,0x02);
	LCD_WriteReg(0xD519,0x00);
	LCD_WriteReg(0xD51A,0x02);
	LCD_WriteReg(0xD51B,0x01);
	LCD_WriteReg(0xD51C,0x02);
	LCD_WriteReg(0xD51D,0x34);
	LCD_WriteReg(0xD51E,0x02);
	LCD_WriteReg(0xD51F,0x67);
	LCD_WriteReg(0xD520,0x02);
	LCD_WriteReg(0xD521,0x84);
	LCD_WriteReg(0xD522,0x02);
	LCD_WriteReg(0xD523,0xA4);
	LCD_WriteReg(0xD524,0x02);
	LCD_WriteReg(0xD525,0xB7);
	LCD_WriteReg(0xD526,0x02);
	LCD_WriteReg(0xD527,0xCF);
	LCD_WriteReg(0xD528,0x02);
	LCD_WriteReg(0xD529,0xDE);
	LCD_WriteReg(0xD52A,0x02);
	LCD_WriteReg(0xD52B,0xF2);
	LCD_WriteReg(0xD52C,0x02);
	LCD_WriteReg(0xD52D,0xFE);
	LCD_WriteReg(0xD52E,0x03);
	LCD_WriteReg(0xD52F,0x10);
	LCD_WriteReg(0xD530,0x03);
	LCD_WriteReg(0xD531,0x33);
	LCD_WriteReg(0xD532,0x03);
	LCD_WriteReg(0xD533,0x6D);
	LCD_WriteReg(0xD600,0x00);
	LCD_WriteReg(0xD601,0x33);
	LCD_WriteReg(0xD602,0x00);
	LCD_WriteReg(0xD603,0x34);
	LCD_WriteReg(0xD604,0x00);
	LCD_WriteReg(0xD605,0x3A);
	LCD_WriteReg(0xD606,0x00);
	LCD_WriteReg(0xD607,0x4A);
	LCD_WriteReg(0xD608,0x00);
	LCD_WriteReg(0xD609,0x5C);
	LCD_WriteReg(0xD60A,0x00);
	LCD_WriteReg(0xD60B,0x81);
	LCD_WriteReg(0xD60C,0x00);
	LCD_WriteReg(0xD60D,0xA6);
	LCD_WriteReg(0xD60E,0x00);
	LCD_WriteReg(0xD60F,0xE5);
	LCD_WriteReg(0xD610,0x01);
	LCD_WriteReg(0xD611,0x13);
	LCD_WriteReg(0xD612,0x01);
	LCD_WriteReg(0xD613,0x54);
	LCD_WriteReg(0xD614,0x01);
	LCD_WriteReg(0xD615,0x82);
	LCD_WriteReg(0xD616,0x01);
	LCD_WriteReg(0xD617,0xCA);
	LCD_WriteReg(0xD618,0x02);
	LCD_WriteReg(0xD619,0x00);
	LCD_WriteReg(0xD61A,0x02);
	LCD_WriteReg(0xD61B,0x01);
	LCD_WriteReg(0xD61C,0x02);
	LCD_WriteReg(0xD61D,0x34);
	LCD_WriteReg(0xD61E,0x02);
	LCD_WriteReg(0xD61F,0x67);
	LCD_WriteReg(0xD620,0x02);
	LCD_WriteReg(0xD621,0x84);
	LCD_WriteReg(0xD622,0x02);
	LCD_WriteReg(0xD623,0xA4);
	LCD_WriteReg(0xD624,0x02);
	LCD_WriteReg(0xD625,0xB7);
	LCD_WriteReg(0xD626,0x02);
	LCD_WriteReg(0xD627,0xCF);
	LCD_WriteReg(0xD628,0x02);
	LCD_WriteReg(0xD629,0xDE);
	LCD_WriteReg(0xD62A,0x02);
	LCD_WriteReg(0xD62B,0xF2);
	LCD_WriteReg(0xD62C,0x02);
	LCD_WriteReg(0xD62D,0xFE);
	LCD_WriteReg(0xD62E,0x03);
	LCD_WriteReg(0xD62F,0x10);
	LCD_WriteReg(0xD630,0x03);
	LCD_WriteReg(0xD631,0x33);
	LCD_WriteReg(0xD632,0x03);
	LCD_WriteReg(0xD633,0x6D);
	LCD_WriteReg(0xF000,0x55);
	LCD_WriteReg(0xF001,0xAA);
	LCD_WriteReg(0xF002,0x52);
	LCD_WriteReg(0xF003,0x08);
	LCD_WriteReg(0xF004,0x00);
	LCD_WriteReg(0xB100, 0xCC);
	LCD_WriteReg(0xB101, 0x00);
	LCD_WriteReg(0xB600,0x05);
	LCD_WriteReg(0xB700,0x70);
	LCD_WriteReg(0xB701,0x70);
	LCD_WriteReg(0xB800,0x01);
	LCD_WriteReg(0xB801,0x03);
	LCD_WriteReg(0xB802,0x03);
	LCD_WriteReg(0xB803,0x03);
	LCD_WriteReg(0xBC00,0x02);
	LCD_WriteReg(0xBC01,0x00);
	LCD_WriteReg(0xBC02,0x00);
	LCD_WriteReg(0xC900,0xD0);
	LCD_WriteReg(0xC901,0x02);
	LCD_WriteReg(0xC902,0x50);
	LCD_WriteReg(0xC903,0x50);
	LCD_WriteReg(0xC904,0x50);
	LCD_WriteReg(0x3500,0x00);
	LCD_WriteReg(0x3A00,0x55);
	LCD_WR_REG(0x1100);
	delay_us(120);
	LCD_WR_REG(0x2900);
	
	LCD_Display_Dir(0);
	LCD_LED=1;	
	LCD_Clear(BLACK);
}  

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Clear
*	¹¦ÄÜËµÃ÷: ÇåÆÁ
*	ÐÎ    ²Î£ºcolor:ÒªÇåÆÁµÄÌî³äÉ«
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 	
	LCD_SetCursor(0x00,0x0000);	
	LCD_WriteRAM_Prepare();     		  
	for(index=0;index<totalpoint;index++)LCD->LCD_RAM=color;	
}  

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Fill
*	¹¦ÄÜËµÃ÷: ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äµ¥¸öÑÕÉ«
*	ÐÎ    ²Î£º(sx,sy),(ex,ey):Ìî³ä¾ØÐÎ¶Ô½Ç×ø±ê,ÇøÓò´óÐ¡Îª:(ex-sx+1)*(ey-sy+1)
						color:ÒªÌî³äµÄÑÕÉ«
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{
		LCD_SetCursor(sx,i);      		
		LCD_WriteRAM_Prepare();     		  
		for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	   
	}	 
}  

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Color_Fill
*	¹¦ÄÜËµÃ÷: ÔÚÖ¸¶¨ÇøÓòÄÚÌî³äÖ¸¶¨ÑÕÉ«¿é	
*	ÐÎ    ²Î£º(sx,sy),(ex,ey):Ìî³ä¾ØÐÎ¶Ô½Ç×ø±ê,ÇøÓò´óÐ¡Îª:(ex-sx+1)*(ey-sy+1)   
						color:ÒªÌî³äµÄÑÕÉ«
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 		
	height=ey-sy+1;		
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   
		LCD_WriteRAM_Prepare();    
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];
	}		  
}  

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_DrawLine
*	¹¦ÄÜËµÃ÷: »­Ïß
*	ÐÎ    ²Î£ºx1,y1:Æðµã×ø±ê
						x2,y2:ÖÕµã×ø±ê  
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else{incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{  
		LCD_DrawPoint(uRow,uCol);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance){xerr-=distance;uRow+=incx;} 
		if(yerr>distance){yerr-=distance;uCol+=incy;} 
	}  
}    

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_DrawRectangle
*	¹¦ÄÜËµÃ÷: »­¾ØÐÎ	  
*	ÐÎ    ²Î£º(x1,y1),(x2,y2):¾ØÐÎµÄ¶Ô½Ç×ø±ê 
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Draw_Circle
*	¹¦ÄÜËµÃ÷: ÔÚÖ¸¶¨Î»ÖÃ»­Ò»¸öÖ¸¶¨´óÐ¡µÄÔ²
*	ÐÎ    ²Î£º(x,y):ÖÐÐÄµã
						r    :°ë¾¶
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);           
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);            
 		LCD_DrawPoint(x0+b,y0-a);                     
		LCD_DrawPoint(x0+b,y0+a);                      
		LCD_DrawPoint(x0+a,y0+b);             
		LCD_DrawPoint(x0-a,y0+b);              
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);                   
  	LCD_DrawPoint(x0-b,y0-a);            	         
		a++;  
		if(di<0)di +=4*a+6;	  
		else{di+=10+4*(a-b);b--;} 						    
	}
} 									  

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ShowChar
*	¹¦ÄÜËµÃ÷: ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û
*	ÐÎ    ²Î£ºx,y:ÆðÊ¼×ø±ê
						num:ÒªÏÔÊ¾µÄ×Ö·û:" "--->"~"
						size:×ÖÌå´óÐ¡ 12/16/24
						mode:µþ¼Ó·½Ê½(1)»¹ÊÇ·Çµþ¼Ó·½Ê½(0)
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);	
 	num=num-' ';
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 
		else if(size==16)temp=asc2_1608[num][t];
		else if(size==24)temp=asc2_2412[num][t];	
		else return;								
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	
				break;
			}
		}  	 
	}  	    	   	 	  
}   

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_Pow
*	¹¦ÄÜËµÃ÷: m^nº¯Êý
*	ÐÎ    ²Î£ºm,µ×
						n,ÃÝ
*	·µ »Ø Öµ: ·µ»ØÖµ:m^n´Î·½.
*********************************************************************************************************
*/
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ShowNum
*	¹¦ÄÜËµÃ÷: ÏÔÊ¾Êý×Ö,¸ßÎ»Îª0,Ôò²»ÏÔÊ¾
*	ÐÎ    ²Î£ºx,y :Æðµã×ø±ê	 
						len :Êý×ÖµÄÎ»Êý
						size:×ÖÌå´óÐ¡
						color:ÑÕÉ« 
						num:ÊýÖµ(0~4294967295);
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 	 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 

/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ShowxNum
*	¹¦ÄÜËµÃ÷: ÏÔÊ¾Êý×Ö,¸ßÎ»Îª0,»¹ÊÇÏÔÊ¾
*	ÐÎ    ²Î£ºx,y:Æðµã×ø±ê
						num:ÊýÖµ(0~999999999);	 
						len:³¤¶È(¼´ÒªÏÔÊ¾µÄÎ»Êý)
						size:×ÖÌå´óÐ¡
						mode:
						[7]:0,²»Ìî³ä;1,Ìî³ä0.
						[6:1]:±£Áô
						[0]:0,·Çµþ¼ÓÏÔÊ¾;1,µþ¼ÓÏÔÊ¾.
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1;  
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
 
/*
*********************************************************************************************************
*	º¯ Êý Ãû: LCD_ShowString
*	¹¦ÄÜËµÃ÷: ÏÔÊ¾×Ö·û´®
*	ÐÎ    ²Î£ºx,y:Æðµã×ø±ê
						width,height:ÇøÓò´óÐ¡  
						size:×ÖÌå´óÐ¡
						*p:×Ö·û´®ÆðÊ¼µØÖ·		 
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
	while((*p<='~')&&(*p>=' '))
	{       
		if(x>=width){x=x0;y+=size;}
		if(y>=height)break;
		LCD_ShowChar(x,y,*p,size,0);
		x+=size/2;
		p++;
	}  
}
