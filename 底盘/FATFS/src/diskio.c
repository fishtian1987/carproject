/*
*********************************************************************************************************
*	                                  
*	模块名称 : FATFS底层驱动管理
*	文件名称 : diskio.c
*	版    本 : V1.3
*	说    明 : FATFS底层驱动管理
*
*
*********************************************************************************************************
*/

#include "diskio.h"		
#include "sdcard.h"
#include "w25qxx.h"
#include "FreeRTOS.h"

#define SD_CARD	 0  
#define EX_FLASH 1	

#define FLASH_SECTOR_SIZE 	512	
#define FLASH_BLOCK_SIZE   	8   
u16	    FLASH_SECTOR_COUNT=2048*10;	  	

/*
*********************************************************************************************************
*	函 数 名: disk_initialize
*	功能说明: 初始化磁盘
*	形    参：pdrv,磁盘编号0~9
*	返 回 值: 0,成功
						其他,失败
*********************************************************************************************************
*/
DSTATUS disk_initialize (BYTE pdrv)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:
			res = SD_Init();
  		break;
		case EX_FLASH:
			W25QXX_Init();
			FLASH_SECTOR_COUNT=2048*10;
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 
}  

/*
*********************************************************************************************************
*	函 数 名: disk_status
*	功能说明: 获得磁盘状态
*	形    参：pdrv,磁盘编号0~9
*	返 回 值: 0,成功
						其他,失败
*********************************************************************************************************
*/
DSTATUS disk_status (BYTE pdrv)
{ 
	return 0;
} 

/*
*********************************************************************************************************
*	函 数 名: disk_read
*	功能说明: 读扇区
*	形    参：drv:磁盘编号0~9
						buff:数据接收缓冲首地址
						sector:扇区地址
						count:需要读取的扇区数
*	返 回 值: 0,成功
						其他,失败
*********************************************************************************************************
*/
DRESULT disk_read (BYTE pdrv,BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0; 
  if (!count)return RES_PARERR;
	switch(pdrv)
	{
		case SD_CARD:
			res=SD_ReadDisk(buff,sector,count);	  
			break;
		case EX_FLASH:
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
	if(res==0x00)return RES_OK;	 
	else return RES_ERROR;	   
}

#if _USE_WRITE
/*
*********************************************************************************************************
*	函 数 名: disk_write
*	功能说明: 写扇区
*	形    参：drv:磁盘编号0~9
						buff:发送数据首地址
						sector:扇区地址
						count:需要写入的扇区数
*	返 回 值: 0,成功
						其他,失败
*********************************************************************************************************
*/
DRESULT disk_write (BYTE pdrv,const BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0;  
  if (!count)return RES_PARERR;
	switch(pdrv)
	{
		case SD_CARD:
			res=SD_WriteDisk((u8*)buff,sector,count);
			break;
		case EX_FLASH:
			for(;count>0;count--)
			{										    
				W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		default:
			res=1; 
	}
	if(res == 0x00)return RES_OK;	 
	else return RES_ERROR;	
}
#endif

#if _USE_IOCTL
/*
*********************************************************************************************************
*	函 数 名: disk_ioctl
*	功能说明: 其他表参数的获得
*	形    参：drv:磁盘编号0~9
						ctrl:控制代码
						buff:发送/接收缓冲区指针
*	返 回 值: 0,成功
						其他,失败
*********************************************************************************************************
*/
DRESULT disk_ioctl (BYTE pdrv,BYTE cmd,void *buff)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD)
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK; 
				break;	 
			case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
				res = RES_OK;
				break;	 
			case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
				res = RES_OK;
				break;	 
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SDCardInfo.CardCapacity/512;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
		}
	}else if(pdrv==EX_FLASH)
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK; 
				break;	 
			case GET_SECTOR_SIZE:
				*(WORD*)buff = FLASH_SECTOR_SIZE;
				res = RES_OK;
				break;	 
			case GET_BLOCK_SIZE:
				*(WORD*)buff = FLASH_BLOCK_SIZE;
				res = RES_OK;
				break;	 
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = FLASH_SECTOR_COUNT;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
		}
	}else res=RES_ERROR;
    return res;
}
#endif
  
/*
*********************************************************************************************************
*	函 数 名: get_fattime
*	功能说明: 获得时间
						31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31)                                                                                                                                                                                                                                        
						15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)   
*	形    参：无
*	返 回 值: 时间
*********************************************************************************************************
*/
DWORD get_fattime (void)
{				 
	return 0;
}			 

/*
*********************************************************************************************************
*	函 数 名: ff_memalloc
*	功能说明: 动态分配内存
*	形    参：size,大小
*	返 回 值: 无
*********************************************************************************************************
*/
void *ff_memalloc (UINT size)			
{
	//return (void*)mymalloc(SRAMIN,size);
	return pvPortMalloc(size);
}

/*
*********************************************************************************************************
*	函 数 名: ff_memfree
*	功能说明: 释放内存
*	形    参：mf,地址
*	返 回 值: 无
*********************************************************************************************************
*/
void ff_memfree (void* mf)		 
{
	//myfree(SRAMIN,mf);
	vPortFree(mf);
}
