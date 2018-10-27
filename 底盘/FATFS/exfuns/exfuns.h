/*
*********************************************************************************************************
*	                                  
*	模块名称 : 文件管理模块
*	文件名称 : exfuns.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/

#ifndef __EXFUNS_H
#define __EXFUNS_H 					   
#include <stm32f4xx.h>
#include "ff.h"

extern FATFS *fs[2];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern u8 *fatbuf;

#define T_BIN		0X00	
#define T_LRC		0X10	
#define T_NES		0X20	
#define T_TEXT	0X30
#define T_C			0X31	
#define T_H			0X32    
#define T_FLAC	0X4C	
#define T_BMP		0X50	
#define T_JPG		0X51	
#define T_JPEG	0X52	
#define T_GIF		0X53	

u8 exfuns_init(void);							
u8 f_typetell(u8 *fname);					
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);	
u32 exf_fdsize(u8 *fdname);					 																		   
#endif
