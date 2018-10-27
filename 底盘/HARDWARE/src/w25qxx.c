/*
*********************************************************************************************************
*	                                  
*	模块名称 : flash管理模块
*	文件名称 : w25qxx.c
*	版    本 : V1.3
*	说    明 : flash管理模块
*
*
*********************************************************************************************************
*/

#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   

u16 W25QXX_TYPE=W25Q128;

u32 FLASH_SIZE=16*1024*1024;

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Init
*	功能说明: 初始化SPI FLASH的IO口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;           
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          
  GPIO_Init(GPIOG, &GPIO_InitStructure);              
 
	W25QXX_CS=1;
	SPI1_Init();		   
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);
	W25QXX_TYPE=W25QXX_ReadID();
}  

/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadSR
*	功能说明: 读取W25QXX的状态寄存器
*	形    参：无
*	返 回 值: BIT7  6   5   4   3   2   1   0
						SPR   RV  TB BP2 BP1 BP0 WEL BUSY
						SPR:默认0,状态寄存器保护位,配合WP使用
						TB,BP2,BP1,BP0:FLASH区域写保护设置
						WEL:写使能锁定
						BUSY:忙标记位(1,忙;0,空闲)
						默认:0x00
*********************************************************************************************************
*/
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                           
	SPI1_ReadWriteByte(W25X_ReadStatusReg);     
	byte=SPI1_ReadWriteByte(0Xff);           
	W25QXX_CS=1;                        
	return byte;   
} 

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_SR
*	功能说明: 写W25QXX状态寄存器
*	形    参：只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_SR(u8 sr)   
{   
	W25QXX_CS=0;                            
	SPI1_ReadWriteByte(W25X_WriteStatusReg);     
	SPI1_ReadWriteByte(sr);              
	W25QXX_CS=1;                            
}   

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Enable
*	功能说明: W25QXX写使能，将WEL置位   
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                           
  SPI1_ReadWriteByte(W25X_WriteEnable);      
	W25QXX_CS=1;                              	      
} 

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Disable
*	功能说明: WW25QXX写禁郑玏EL清零
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                           
  SPI1_ReadWriteByte(W25X_WriteDisable);     
	W25QXX_CS=1;                             	      
} 		

/*
*********************************************************************************************************
*	函 数 名: W25QXX_ReadID
*	功能说明: 读取芯片ID
*	形    参：无
*	返 回 值: 0XEF13,表示芯片型号为W25Q80  
						0XEF14,表示芯片型号为W25Q16    
						0XEF15,表示芯片型号为W25Q32  
						0XEF16,表示芯片型号为W25Q64 
						0XEF17,表示芯片型号为W25Q128 	 
*********************************************************************************************************
*/
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI1_ReadWriteByte(0x90);   
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return Temp;
}   		    

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Read
*	功能说明: 读取SPI FLASH，在指定地址开始读取指定长度的数据
*	形    参：pBuffer:数据存储区
						ReadAddr:开始读取的地址(24bit)
						NumByteToRead:要读取的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	W25QXX_CS=0;                           
	SPI1_ReadWriteByte(W25X_ReadData);        
	SPI1_ReadWriteByte((u8)((ReadAddr)>>16));  
	SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPI1_ReadWriteByte((u8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)pBuffer[i]=SPI1_ReadWriteByte(0XFF);   
	W25QXX_CS=1;  				    	      
}  
 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_Page
*	功能说明: SPI在一页(0~65535)内写入少于256个字节的数据
						在指定地址开始写入最大256字节的数据
*	形    参：pBuffer:数据存储区
						WriteAddr:开始写入的地址(24bit)
						NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  W25QXX_Write_Enable();                
	W25QXX_CS=0;                           
	SPI1_ReadWriteByte(W25X_PageProgram);       
	SPI1_ReadWriteByte((u8)((WriteAddr)>>16));  
	SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
	SPI1_ReadWriteByte((u8)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);
	W25QXX_CS=1;                           
	W25QXX_Wait_Busy();					  
} 

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write_NoCheck
*	功能说明: 无检验写SPI FLASH 
						必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败
						具有自动换页功能 
						在指定地址开始写入指定长度的数据,但是要确保地址不越界!
*	形    参：pBuffer:数据存储区
						WriteAddr:开始写入的地址(24bit)
						NumByteToWrite:要写入的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256;  
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;
	 	else 
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			 
			if(NumByteToWrite>256)pageremain=256;
			else pageremain=NumByteToWrite; 	  
		}
	};	    
} 
 
u8 W25QXX_BUFFER[4096];		 
/*
*********************************************************************************************************
*	函 数 名: W25QXX_Write
*	功能说明: 写SPI FLASH  
						在指定地址开始写入指定长度的数据
						该函数带擦除操作!
*	形    参：pBuffer:数据存储区
						WriteAddr:开始写入的地址(24bit)
						NumByteToWrite:要写入的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;
	secoff=WriteAddr%4096;
	secremain=4096-secoff;
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
		for(i=0;i<secremain;i++)
			if(W25QXX_BUF[secoff+i]!=0XFF)break;
		if(i<secremain)
		{
			W25QXX_Erase_Sector(secpos);
			for(i=0;i<secremain;i++)W25QXX_BUF[i+secoff]=pBuffer[i];	  
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);
		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);
		if(NumByteToWrite==secremain)break;
		else
		{
			secpos++;
			secoff=0;
		  pBuffer+=secremain;  
			WriteAddr+=secremain;  
		  NumByteToWrite-=secremain;				
			if(NumByteToWrite>4096)secremain=4096;	
			else secremain=NumByteToWrite;		
		}	 
	};	 
}

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Erase_Chip
*	功能说明: 擦除整个芯片
*	形    参：pBuffer:数据存储区
						WriteAddr:开始写入的地址(24bit)
						NumByteToWrite:要写入的字节数(最大65535)
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Erase_Chip(void)   
{                                   
	W25QXX_Write_Enable();                
	W25QXX_Wait_Busy();   
	W25QXX_CS=0;                           
	SPI1_ReadWriteByte(W25X_ChipErase);      
	W25QXX_CS=1;                           	      
	W25QXX_Wait_Busy();   				
}   

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Erase_Sector
*	功能说明: 擦除一个扇区
*	形    参：Dst_Addr:扇区地址 根据实际容量设置
						擦除一个山区的最少时间:150ms
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{   
 	Dst_Addr*=4096;
	W25QXX_Write_Enable();                 
	W25QXX_Wait_Busy();   
	W25QXX_CS=0;                           
	SPI1_ReadWriteByte(W25X_SectorErase);      
	SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  
	SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI1_ReadWriteByte((u8)Dst_Addr);  
	W25QXX_CS=1;                                 
  W25QXX_Wait_Busy();   				 
}  

/*
*********************************************************************************************************
*	函 数 名: W25QXX_Wait_Busy
*	功能说明: 等待空闲
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01); 
}  

/*
*********************************************************************************************************
*	函 数 名: W25QXX_PowerDown
*	功能说明: 进入掉电模式
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS=0;                           
  SPI1_ReadWriteByte(W25X_PowerDown);       
	W25QXX_CS=1;                            	      
  delay_us(3);                              
}   

/*
*********************************************************************************************************
*	函 数 名: W25QXX_WAKEUP
*	功能说明: 唤醒
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25QXX_WAKEUP(void)   
{  
	W25QXX_CS=0;                           
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   
	W25QXX_CS=1;                                
  delay_us(3);                             
}   
