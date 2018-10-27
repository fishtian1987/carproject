/*
*********************************************************************************************************
*	                                  
*	模块名称 : LCD处理
*	文件名称 : lcd.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h" 

typedef struct  
{										    
	u16 width;		
	u16 height;		
	u16 id;				
	u8  dir;			//0，竖屏；1，横屏。	
	u16	wramcmd;		
	u16  setxcmd;		
	u16  setycmd;		
}_lcd_dev; 	  

extern _lcd_dev lcddev;	   
extern u16  POINT_COLOR;
extern u16  BACK_COLOR; 

#define	LCD_LED PFout(10)  	 

typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((u32)(0x6C000000 | 0x00001FFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
	 
#define L2R_U2D  0 
#define L2R_D2U  1 
#define R2L_U2D  2 
#define R2L_D2U  3 

#define U2D_L2R  4 
#define U2D_R2L  5
#define D2U_L2R  6 
#define D2U_R2L  7 

#define DFT_SCAN_DIR  L2R_U2D  

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK          	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 
#define BRRED 			     0XFC07 
#define GRAY  			     0X8430 
//GUI颜色
#define DARKBLUE      	 0X01CF	
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458 
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F 
#define LGRAY 			     0XC618

#define LGRAYBLUE        0XA651 
#define LBBLUE           0X2B12

void LCD_Init(void);													   
void LCD_DisplayOn(void);													
void LCD_DisplayOff(void);													
void LCD_Clear(u16 Color);	 																																																		
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);						 			
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   				   							
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
#endif  
