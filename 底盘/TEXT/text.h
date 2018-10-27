/*
*********************************************************************************************************
*	                                  
*	模块名称 : 字体显示处理模块
*	文件名称 : text.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __TEXT_H__
#define __TEXT_H__	 
#include <stm32f4xx.h>
#include "fontupd.h"
 					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);		
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
#endif
