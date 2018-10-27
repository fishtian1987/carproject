/*
*********************************************************************************************************
*	                                  
*	ģ������ : flash����ģ��
*	�ļ����� : w25qxx.c
*	��    �� : V1.3
*	˵    �� : flash����ģ��
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
*	�� �� ��: W25QXX_Init
*	����˵��: ��ʼ��SPI FLASH��IO��
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_ReadSR
*	����˵��: ��ȡW25QXX��״̬�Ĵ���
*	��    �Σ���
*	�� �� ֵ: BIT7  6   5   4   3   2   1   0
						SPR   RV  TB BP2 BP1 BP0 WEL BUSY
						SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
						TB,BP2,BP1,BP0:FLASH����д��������
						WEL:дʹ������
						BUSY:æ���λ(1,æ;0,����)
						Ĭ��:0x00
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
*	�� �� ��: W25QXX_Write_SR
*	����˵��: дW25QXX״̬�Ĵ���
*	��    �Σ�ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Write_Enable
*	����˵��: W25QXXдʹ�ܣ���WEL��λ   
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Write_Disable
*	����˵��: WW25QXXд��֣���WEL����
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_ReadID
*	����˵��: ��ȡоƬID
*	��    �Σ���
*	�� �� ֵ: 0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
						0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
						0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
						0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
						0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	 
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
*	�� �� ��: W25QXX_Read
*	����˵��: ��ȡSPI FLASH����ָ����ַ��ʼ��ȡָ�����ȵ�����
*	��    �Σ�pBuffer:���ݴ洢��
						ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
						NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Write_Page
*	����˵��: SPI��һҳ(0~65535)��д������256���ֽڵ�����
						��ָ����ַ��ʼд�����256�ֽڵ�����
*	��    �Σ�pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Write_NoCheck
*	����˵��: �޼���дSPI FLASH 
						����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��
						�����Զ���ҳ���� 
						��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*	��    �Σ�pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���65535)
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Write
*	����˵��: дSPI FLASH  
						��ָ����ַ��ʼд��ָ�����ȵ�����
						�ú�������������!
*	��    �Σ�pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���65535)
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Erase_Chip
*	����˵��: ��������оƬ
*	��    �Σ�pBuffer:���ݴ洢��
						WriteAddr:��ʼд��ĵ�ַ(24bit)
						NumByteToWrite:Ҫд����ֽ���(���65535)
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Erase_Sector
*	����˵��: ����һ������
*	��    �Σ�Dst_Addr:������ַ ����ʵ����������
						����һ��ɽ��������ʱ��:150ms
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_Wait_Busy
*	����˵��: �ȴ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01); 
}  

/*
*********************************************************************************************************
*	�� �� ��: W25QXX_PowerDown
*	����˵��: �������ģʽ
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: W25QXX_WAKEUP
*	����˵��: ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void W25QXX_WAKEUP(void)   
{  
	W25QXX_CS=0;                           
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   
	W25QXX_CS=1;                                
  delay_us(3);                             
}   
