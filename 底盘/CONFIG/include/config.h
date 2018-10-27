/*
*********************************************************************************************************
*	                                  
*	模块名称 : 配置参数模块
*	文件名称 : config.h
*	版    本 : V1.3
*	说    明 : 头文件
*********************************************************************************************************
*/


#ifndef __CONFIG_H
#define __CONFIG_H
#include "nvic.h"
#include "stdio.h"	
#include "w25qxx.h" 

#define BOOTLOADER_SIZE		(16*1024)	
#define CONFIG_PARAM_SIZE	 800

#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE + CONFIG_PARAM_SIZE)	/*16K bootloader+ 16 模拟eeprom*/


#define DEG2RAD		0.017453293f	/* 度转弧度 π/180 */
#define RAD2DEG		57.29578f		/* 弧度转度 180/π */

#define P_NAME "MiniFly"
#define MCU_ID_ADDRESS          0x1FFF7A10
#define MCU_FLASH_SIZE_ADDRESS  0x1FFF7A22


#endif
