/*
*********************************************************************************************************
*	                                  
*	模块名称 : SD卡管理
*	文件名称 : sdcard.c
*	版    本 : V1.3
*	说    明 : SD卡管理
*
*
*********************************************************************************************************
*/

#include "sdcard.h"
#include "string.h"	  

SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;   

static u8 CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;	
static u32 CSD_Tab[4],CID_Tab[4],RCA=0;					
static u8 DeviceMode=SD_DMA_MODE;		   				
static u8 StopCondition=0; 							
volatile SD_Error TransferError=SD_OK;					
volatile u8 TransferEnd=0;							
SD_CardInfo SDCardInfo;								

__align(4) u8 SDIO_DATA_BUFFER[512];						  
 
/*
*********************************************************************************************************
*	函 数 名: SDIO_Register_Deinit
*	功能说明: 注册信息
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SDIO_Register_Deinit(void)
{
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	 
}

/*
*********************************************************************************************************
*	函 数 名: SD_Init
*	功能说明: 初始化SD卡
*	形    参：无
*	返 回 值: 错误代码;(0,无错误)
*********************************************************************************************************
*/
SD_Error SD_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	SD_Error errorstatus=SD_OK;	 
  u8 clkdiv=0;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);	
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);		   
	SDIO_Register_Deinit();
	
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
	
 	errorstatus=SD_PowerON();
 	if(errorstatus==SD_OK)errorstatus=SD_InitializeCards();														  
  if(errorstatus==SD_OK)errorstatus=SD_GetCardInfo(&SDCardInfo);
 	if(errorstatus==SD_OK)errorstatus=SD_SelectDeselect((u32)(SDCardInfo.RCA<<16));
 	if(errorstatus==SD_OK)errorstatus=SD_EnableWideBusOperation(SDIO_BusWide_4b);
  if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
	{  		    
		if(SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)clkdiv=SDIO_TRANSFER_CLK_DIV+2;	
		else clkdiv=SDIO_TRANSFER_CLK_DIV;	
		SDIO_Clock_Set(clkdiv);	
		errorstatus=SD_SetDeviceMode(SD_POLLING_MODE);
 	}
	return errorstatus;		 
}

/*
*********************************************************************************************************
*	函 数 名: SDIO_Clock_Set
*	功能说明: SDIO时钟初始化设置
*	形    参：clkdiv:时钟分频系数
						CK时钟=SDIOCLK/[clkdiv+2];(SDIOCLK时钟固定为48Mhz)
*	返 回 值: 无
*********************************************************************************************************
*/
void SDIO_Clock_Set(u8 clkdiv)
{
	u32 tmpreg=SDIO->CLKCR; 
  tmpreg&=0XFFFFFF00; 
 	tmpreg|=clkdiv;   
	SDIO->CLKCR=tmpreg;
} 

/*
*********************************************************************************************************
*	函 数 名: CmdError
*	功能说明: 检查CMD0的执行状态
*	形    参：无
*	返 回 值: sd卡错误码
*********************************************************************************************************
*/
SD_Error CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	u32 timeout=SDIO_CMD0TIMEOUT;	   
	while(timeout--)
	{
		if(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)break;
	}	    
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;  
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	return errorstatus;
}	

/*
*********************************************************************************************************
*	函 数 名: CmdResp7Error
*	功能说明: 检查R7响应的错误状态
*	形    参：无
*	返 回 值: sd卡错误码
*********************************************************************************************************
*/
SD_Error CmdResp7Error(void)
{
	SD_Error errorstatus=SD_OK;
	u32 status;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	}
 	if((timeout==0)||(status&(1<<2))){errorstatus=SD_CMD_RSP_TIMEOUT;SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return errorstatus;}	 
	if(status&1<<6){errorstatus=SD_OK;SDIO_ClearFlag(SDIO_FLAG_CMDREND);}
	return errorstatus;
}	

/*
*********************************************************************************************************
*	函 数 名: CmdResp1Error
*	功能说明: 检查R1响应的错误状态
*	形    参：cmd:当前命令
*	返 回 值: sd卡错误码
*********************************************************************************************************
*/
SD_Error CmdResp1Error(u8 cmd)
{	  
   	u32 status; 
	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	} 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return SD_CMD_RSP_TIMEOUT;}	
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);return SD_CMD_CRC_FAIL;}		
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	return (SD_Error)(SDIO->RESP1&SD_OCR_ERRORBITS);
}

/*
*********************************************************************************************************
*	函 数 名: CmdResp3Error
*	功能说明: 检查R3响应的错误状态
*	形    参：无
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error CmdResp3Error(void)
{
	u32 status;						 
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return SD_CMD_RSP_TIMEOUT;}	 
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
 	return SD_OK;								  
}

/*
*********************************************************************************************************
*	函 数 名: SD_PowerON
*	功能说明: 卡上电，查询所有SDIO接口上的卡设备,并查询其电压和配置时钟
*	形    参：无
*	返 回 值: 错误代码;(0,无错误)
*********************************************************************************************************
*/
SD_Error SD_PowerON(void)
{
 	u8 i=0;
	SD_Error errorstatus=SD_OK;
	u32 response=0,count=0,validvoltage=0;
	u32 SDType=SD_STD_CAPACITY;
	
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;	
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;  
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;	
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;	 			
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);
	SDIO_SetPowerState(SDIO_PowerState_ON);	
  SDIO->CLKCR|=1<<8;		
 	for(i=0;i<74;i++)
	{
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; 
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable; 
    SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdError();	
		if(errorstatus==SD_OK)break;
 	}
 	if(errorstatus)return errorstatus;
  SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;	
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;		
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus=CmdResp7Error();						
	
 	if(errorstatus==SD_OK) 								
	{
		CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;
		SDType=SD_HIGH_CAPACITY;			   		
	}
	  
	SDIO_CmdInitStructure.SDIO_Argument = 0x00;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);		
	errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 		
	
	if(errorstatus==SD_OK)
	{																  
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   
		  SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;	 
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);				
			errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 		
 			if(errorstatus!=SD_OK)return errorstatus;   	

      SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;	
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; 
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);			
			errorstatus=CmdResp3Error(); 				
			
 			if(errorstatus!=SD_OK)return errorstatus;   
			response=SDIO->RESP1;;			   				
			validvoltage=(((response>>31)==1)?1:0);			
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 
		if(response&=SD_HIGH_CAPACITY)
			CardType=SDIO_HIGH_CAPACITY_SD_CARD;
 	}else
	{
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   				   
			SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; 
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);			
			errorstatus=CmdResp3Error(); 					
 			if(errorstatus!=SD_OK)return errorstatus;   
			response=SDIO->RESP1;;			   		
			validvoltage=(((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 			    
		CardType=SDIO_MULTIMEDIA_CARD;	  
  }  
  	return(errorstatus);		
}

/*
*********************************************************************************************************
*	函 数 名: SD_PowerOFF
*	功能说明: SD卡 Power OFF
*	形    参：无
*	返 回 值: 错误代码;(0,无错误)
*********************************************************************************************************
*/
SD_Error SD_PowerOFF(void)
{
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  return SD_OK;	  
}   

/*
*********************************************************************************************************
*	函 数 名: CmdResp2Error
*	功能说明: 检查R2响应的错误状态
*	形    参：无
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error CmdResp2Error(void)
{
	SD_Error errorstatus=SD_OK;
	u32 status;
	u32 timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	}
  if((timeout==0)||(status&(1<<2))){errorstatus=SD_CMD_RSP_TIMEOUT;SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return errorstatus;}	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){errorstatus=SD_CMD_CRC_FAIL;SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
 	return errorstatus;								    		 
} 

/*
*********************************************************************************************************
*	函 数 名: CmdResp6Error
*	功能说明: 检查R6响应的错误状态
*	形    参：cmd:之前发送的命令
						prca:卡返回的RCA地址
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error CmdResp6Error(u8 cmd,u16*prca)
{
	SD_Error errorstatus=SD_OK;
	u32 status;					    
	u32 rspr1;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return SD_CMD_RSP_TIMEOUT;}	 	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);return SD_CMD_CRC_FAIL;}
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD; 		    
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	rspr1=SDIO->RESP1;					
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED))){*prca=(u16)(rspr1>>16);return errorstatus;}
	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
	if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
	if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
	return errorstatus;
}

/*
*********************************************************************************************************
*	函 数 名: SD_InitializeCards
*	功能说明: 初始化所有的卡,并让卡进入就绪状态
*	形    参：无
*	返 回 值: 错误代码
*********************************************************************************************************
*/
SD_Error SD_InitializeCards(void)
{
 	SD_Error errorstatus=SD_OK;
	u16 rca = 0x01;
  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

 	if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)	
	{
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
		errorstatus=CmdResp2Error(); 						
		if(errorstatus!=SD_OK)return errorstatus;   	
		
 		CID_Tab[0]=SDIO->RESP1;
		CID_Tab[1]=SDIO->RESP2;
		CID_Tab[2]=SDIO->RESP3;
		CID_Tab[3]=SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdResp6Error(SD_CMD_SET_REL_ADDR,&rca);		
		if(errorstatus!=SD_OK)return errorstatus;   	
	}   
	if (SDIO_MULTIMEDIA_CARD==CardType)
	{
		SDIO_CmdInitStructure.SDIO_Argument = (u32)(rca<<16);
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; 
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdResp2Error(); 					
		if(errorstatus!=SD_OK)return errorstatus;   	
	}
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)
	{
		RCA = rca;
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdResp2Error(); 				
		if(errorstatus!=SD_OK)return errorstatus;   			
		CSD_Tab[0]=SDIO->RESP1;
	  CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;					    
	}
	return SD_OK;
} 

/*
*********************************************************************************************************
*	函 数 名: SD_GetCardInfo
*	功能说明: 得到卡信息
*	形    参：cardinfo:卡信息存储区
*	返 回 值: 错误代码
*********************************************************************************************************
*/
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
 	SD_Error errorstatus=SD_OK;
	u8 tmp=0;	   
	cardinfo->CardType=(u8)CardType; 				
	cardinfo->RCA=(u16)RCA;						
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	
	cardinfo->SD_csd.Reserved1=tmp&0x03;			
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);		
	cardinfo->SD_csd.TAAC=tmp;				   	
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);	  	
	cardinfo->SD_csd.NSAC=tmp;		  				
	tmp=(u8)(CSD_Tab[0]&0x000000FF);			
	cardinfo->SD_csd.MaxBusClkFrec=tmp;		  	 
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);		
	cardinfo->SD_csd.CardComdClasses=tmp<<4;    
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);	 		
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);			
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0; 					
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;
	 	tmp=(u8)(CSD_Tab[1]&0x000000FF); 		
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);	
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);		
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);	  	
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	
	{
 		tmp=(u8)(CSD_Tab[1]&0x000000FF); 		
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;
 		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24); 
 		cardinfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);
 		cardinfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8); 	
 		cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;
		cardinfo->CardBlockSize=512; 			
	}	  
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(u8)(CSD_Tab[2]&0x000000FF);			
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);		
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);	
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);	
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(u8)(CSD_Tab[3]&0x000000FF);		
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);		
	cardinfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);	
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);	
	cardinfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(u8)(CID_Tab[0]&0x000000FF);	
	cardinfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24); 		
	cardinfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);	   
	cardinfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);		
	cardinfo->SD_cid.ProdName1|=tmp;		   
	tmp=(u8)(CID_Tab[1]&0x000000FF);	  		
	cardinfo->SD_cid.ProdName2=tmp;			  
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24); 		
	cardinfo->SD_cid.ProdRev=tmp;		 
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);		
	cardinfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8); 	
	cardinfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(u8)(CID_Tab[2]&0x000000FF);   			
	cardinfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24); 	
	cardinfo->SD_cid.ProdSN|=tmp;			     
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);	 	
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);		
	cardinfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(u8)(CID_Tab[3]&0x000000FF);		
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;	 
	return errorstatus;
}
  
/*
*********************************************************************************************************
*	函 数 名: FindSCR
*	功能说明: 查找SD卡的SCR寄存器值
*	形    参：rca:卡相对地址
						pscr:数据缓存区(存储SCR内容)
*	返 回 值: 错误状态		  
*********************************************************************************************************
*/
SD_Error FindSCR(u16 rca,u32 *pscr)
{ 
	u32 index = 0; 
	SD_Error errorstatus = SD_OK;
	u32 tempscr[2]={0,0};  
	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; 
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
 	errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;	 
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; 
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 8; 
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b; 
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);		

  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
	
 	errorstatus=CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							   
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
		{
			*(tempscr+index)=SDIO->FIFO;	
			index++;
			if(index>=2)break;
		}
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)	
	{										   
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
		return SD_DATA_TIMEOUT;
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
		return SD_DATA_CRC_FAIL;		   
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		
		return SD_RX_OVERRUN;		 
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);
		return SD_START_BIT_ERR;		 
	}  
   SDIO_ClearFlag(SDIO_STATIC_FLAGS); 	
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	return errorstatus;
}

/*
*********************************************************************************************************
*	函 数 名: SDEnWideBus
*	功能说明: SDIO使能宽总线模式
*	形    参：enx:0,不使能;1,使能;
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SDEnWideBus(u8 enx)
{
	SD_Error errorstatus = SD_OK;
 	u32 scr[2]={0,0};
	u8 arg=0X00;
	if(enx)arg=0X02;
	else arg=0X00;
 	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;	    
 	errorstatus=FindSCR(RCA,scr);					
 	if(errorstatus!=SD_OK)return errorstatus;
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		
	{
		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
	 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);	
	 	if(errorstatus!=SD_OK)return errorstatus; 	
		SDIO_CmdInitStructure.SDIO_Argument = arg;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);		
    errorstatus=CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				
}								

/*
*********************************************************************************************************
*	函 数 名: SD_EnableWideBusOperation
*	功能说明: 设置SDIO总线宽度(MMC卡不支持4bit模式)
*	形    参：wmode:位宽模式.0,1位数据宽度;1,4位数据宽度;2,8位数据宽度
						设置SDIO总线宽度(MMC卡不支持4bit模式)
				    @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
					  @arg SDIO_BusWide_4b: 4-bit data transfer
					 @arg SDIO_BusWide_1b: 1-bit data transfer (默认)
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_EnableWideBusOperation(u32 WideMode)
{
  SD_Error errorstatus=SD_OK;
  if (SDIO_MULTIMEDIA_CARD == CardType)
  {
    errorstatus = SD_UNSUPPORTED_FEATURE;
    return(errorstatus);
  }else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		if (SDIO_BusWide_8b == WideMode) 
    {
      errorstatus = SD_UNSUPPORTED_FEATURE;
      return(errorstatus);
    }else   
		{
			errorstatus=SDEnWideBus(WideMode);
 			if(SD_OK==errorstatus)
			{
				SDIO->CLKCR&=~(3<<11);		
				SDIO->CLKCR|=WideMode;
				SDIO->CLKCR|=0<<14;	
			}
		}  
	}
	return errorstatus; 
}

/*
*********************************************************************************************************
*	函 数 名: SD_SetDeviceMode
*	功能说明: 设置SD卡工作模式
*	形    参：Mode:
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_SetDeviceMode(u32 Mode)
{
	SD_Error errorstatus = SD_OK;
 	if((Mode==SD_DMA_MODE)||(Mode==SD_POLLING_MODE))DeviceMode=Mode;
	else errorstatus=SD_INVALID_PARAMETER;
	return errorstatus;	    
}

/*
*********************************************************************************************************
*	函 数 名: SD_SelectDeselect
*	功能说明: 选卡,发送CMD7,选择相对地址(rca)为addr的卡,取消其他卡.如果为0,则都不选择.
*	形    参：addr:卡的RCA地址
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_SelectDeselect(u32 addr)
{

  SDIO_CmdInitStructure.SDIO_Argument =  addr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
 	return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);	  
}

/*
*********************************************************************************************************
*	函 数 名: convert_from_bytes_to_power_of_two
*	功能说明: 得到NumberOfBytes以2为底的指数.
*	形    参：NumberOfBytes:字节数.
*	返 回 值: 以2为底的指数值		  
*********************************************************************************************************
*/
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
	u8 count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
} 

/*
*********************************************************************************************************
*	函 数 名: SD_ReadBlock
*	功能说明: SD卡读取一个块 
*	形    参：buf:读数据缓存区(必须4字节对齐!!)
						addr:读取地址
						blksize:块大小
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_ReadBlock(u8 *buf,long long addr,u16 blksize)
{	  
	SD_Error errorstatus=SD_OK;
	u8 power;
  u32 count=0,*tempbuff=(u32*)buf;
	u32 timeout=SDIO_DATATIMEOUT;   
  if(NULL==buf)return SD_INVALID_PARAMETER; 
  SDIO->DCTRL=0x0;	
  
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD){blksize=512;addr>>=9;}   
	SDIO_DataInitStructure.SDIO_DataBlockSize= SDIO_DataBlockSize_1b ;
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	 
		SDIO_CmdInitStructure.SDIO_Argument =  blksize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);	
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);		
		if(errorstatus!=SD_OK)return errorstatus;   	
	}else return SD_INVALID_PARAMETER;	  	 
	SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4 ;
	SDIO_DataInitStructure.SDIO_DataLength= blksize ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);

	SDIO_CmdInitStructure.SDIO_Argument =  addr;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	errorstatus=CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);
	if(errorstatus!=SD_OK)return errorstatus;   		
 	if(DeviceMode==SD_POLLING_MODE)						
	{
 		INTX_DISABLE();
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))
		{
			if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)					
			{
				for(count=0;count<8;count++)			*(tempbuff+count)=SDIO->FIFO;
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	
			}else 	
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
		{
	 		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);	
			return SD_RX_OVERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	
		{
			*tempbuff=SDIO->FIFO;	
			tempbuff++;
		}
		INTX_ENABLE();
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	}else if(DeviceMode==SD_DMA_MODE)
	{
 		TransferError=SD_OK;
		StopCondition=0;			
		TransferEnd=0;			
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	
	 	SDIO->DCTRL|=1<<3;		 
 	  SD_DMA_Config((u32*)buf,blksize,DMA_DIR_PeripheralToMemory); 
 		while(((DMA2->LISR&(1<<27))==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
		if(timeout==0)return SD_DATA_TIMEOUT;
		if(TransferError!=SD_OK)errorstatus=TransferError;  
  }   
 	return errorstatus; 
}

__align(4) u32 *tempbuff;
/*
*********************************************************************************************************
*	函 数 名: SD_ReadMultiBlocks
*	功能说明: SD卡读取多个块 
*	形    参：buf:读数据缓存区
						addr:读取地址
						blksize:块大小
						nblks:要读取的块数
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_ReadMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks)
{
  SD_Error errorstatus=SD_OK;
	u8 power;
  u32 count=0;
	u32 timeout=SDIO_DATATIMEOUT;  
	tempbuff=(u32*)buf;
	
  SDIO->DCTRL=0x0;
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD){blksize=512;addr>>=9;}  
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
	SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    	
	  SDIO_CmdInitStructure.SDIO_Argument =  blksize;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	
		if(errorstatus!=SD_OK)return errorstatus;   		
	}else return SD_INVALID_PARAMETER;	  
	if(nblks>1)										
	{									    
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;
	
		SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4;
		SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
		SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
		SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
		SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
		SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStructure);

		SDIO_CmdInitStructure.SDIO_Argument =  addr;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_READ_MULT_BLOCK);	
		if(errorstatus!=SD_OK)return errorstatus;   		
 		if(DeviceMode==SD_POLLING_MODE)
		{
			INTX_DISABLE();
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))
			{
				if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)					
				{
					for(count=0;count<8;count++)*(tempbuff+count)=SDIO->FIFO;
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	
				}else 	
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
			if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
			{										   
				SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
				return SD_DATA_TIMEOUT;
			}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
			{
				SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
			{
				SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		
				return SD_RX_OVERRUN;		 
			}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
			{
				SDIO_ClearFlag(SDIO_FLAG_STBITERR);
				return SD_START_BIT_ERR;		 
			}   
	    
			while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	
			{
				*tempbuff=SDIO->FIFO;	
				tempbuff++;
			}
			if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{				
					SDIO_CmdInitStructure.SDIO_Argument =  0;
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);				
					errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);		
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			INTX_ENABLE();
			SDIO_ClearFlag(SDIO_STATIC_FLAGS);
		}else if(DeviceMode==SD_DMA_MODE)
		{
	   	TransferError=SD_OK;
			StopCondition=1;			
			TransferEnd=0;			
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	
		 	SDIO->DCTRL|=1<<3;		 						
	 	  SD_DMA_Config((u32*)buf,nblks*blksize,DMA_DIR_PeripheralToMemory); 
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;
			if(timeout==0)return SD_DATA_TIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)); 
			if(TransferError!=SD_OK)errorstatus=TransferError;  	 
		}		 
  	}
	return errorstatus;
}			    																  

/*
*********************************************************************************************************
*	函 数 名: IsCardProgramming
*	功能说明: 检查卡是否正在执行写操作
*	形    参：pstatus:当前状态.
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error IsCardProgramming(u8 *pstatus)
{
 	vu32 respR1 = 0, status = 0;  
  
  SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; 
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);		
	status=SDIO->STA;
	while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;
  if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET){SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);return SD_CMD_CRC_FAIL;}
  if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET){SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);return SD_CMD_RSP_TIMEOUT;}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	respR1=SDIO->RESP1;
	*pstatus=(u8)((respR1>>9)&0x0000000F);
	return SD_OK;
}

/*
*********************************************************************************************************
*	函 数 名: SD_WriteBlock
*	功能说明: SD卡写1个块 
*	形    参：buf:数据缓存区
						addr:写地址
						blksize:块大小	  
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_WriteBlock(u8 *buf,long long addr,  u16 blksize)
{
	SD_Error errorstatus = SD_OK;	
	u8  power=0,cardstate=0;
	u32 timeout=0,bytestransferred=0;
	u32 cardstatus=0,count=0,restwords=0;
	u32	tlen=blksize;					
	u32*tempbuff=(u32*)buf;					
	
 	if(buf==NULL)return SD_INVALID_PARAMETER;	
  SDIO->DCTRL=0x0;								
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0;
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
  SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD){blksize=512;addr>>=9;}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);		
		SDIO_CmdInitStructure.SDIO_Argument = blksize;	
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);		
		if(errorstatus!=SD_OK)return errorstatus;   	
	}else return SD_INVALID_PARAMETER;	
	
	SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA<<16;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);		
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
 	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 
	{
		timeout--;  
		SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA<<16;
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);			
		if(errorstatus!=SD_OK)return errorstatus;			
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;

	SDIO_CmdInitStructure.SDIO_Argument = addr;
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	errorstatus=CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);
	if(errorstatus!=SD_OK)return errorstatus;   	 
	StopCondition=0;							

	SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	
	SDIO_DataInitStructure.SDIO_DataLength= blksize ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
  SDIO_DataConfig(&SDIO_DataInitStructure);
	timeout=SDIO_DATATIMEOUT;
	
	if (DeviceMode == SD_POLLING_MODE)
	{
		INTX_DISABLE();
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))
		{
			if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)					
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)
				{
					restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);					
					for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)SDIO->FIFO=*tempbuff;
				}else
				{
					for(count=0;count<8;count++)SDIO->FIFO=*(tempbuff+count);
					tempbuff+=8;
					bytestransferred+=32;
				}
				timeout=0X3FFFFFFF;	
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)	
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  	
			return SD_DATA_CRC_FAIL;		   
		}	else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	
		{
	 		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		
			return SD_TX_UNDERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);
			return SD_START_BIT_ERR;		 
		}         
		INTX_ENABLE();
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	}else if(DeviceMode==SD_DMA_MODE)
	{
   	TransferError=SD_OK;
		StopCondition=0;			
		TransferEnd=0;				
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	
		SD_DMA_Config((u32*)buf,blksize,DMA_DIR_MemoryToPeripheral);				
 	 	SDIO->DCTRL|=1<<3;							
 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;
		if(timeout==0){SD_Init();return SD_DATA_TIMEOUT;		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}   
	return errorstatus;
}
	
/*
*********************************************************************************************************
*	函 数 名: SD_WriteMultiBlocks
*	功能说明: SD卡写多个块 
*	形    参：buf:数据缓存区
						addr:写地址
						blksize:块大小
						nblks:要写入的块数
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_WriteMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks)
{
	SD_Error errorstatus = SD_OK;
	u8  power = 0, cardstate = 0;
	u32 timeout=0,bytestransferred=0;
	u32 count = 0, restwords = 0;
	u32 tlen=nblks*blksize;			
	u32 *tempbuff = (u32*)buf;  
  if(buf==NULL)return SD_INVALID_PARAMETER; 
  SDIO->DCTRL=0x0;				
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0;
	SDIO_DataInitStructure.SDIO_DataLength= 0;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
  SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD){blksize=512;addr>>=9;}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	
		SDIO_CmdInitStructure.SDIO_Argument = blksize;	
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);		
		if(errorstatus!=SD_OK)return errorstatus;   	 	
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
				SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA<<16;			
				SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
				SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
				SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
				SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
				SDIO_SendCommand(&SDIO_CmdInitStructure);			
				errorstatus=CmdResp1Error(SD_CMD_APP_CMD);				
				if(errorstatus!=SD_OK)return errorstatus;				 
				
				SDIO_CmdInitStructure.SDIO_Argument =nblks; 	 
				SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
				SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
				SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
				SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
				SDIO_SendCommand(&SDIO_CmdInitStructure);
				errorstatus=CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);		
				if(errorstatus!=SD_OK)return errorstatus;			    
			} 
			SDIO_CmdInitStructure.SDIO_Argument =addr;	
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);	
			errorstatus=CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	 
			if(errorstatus!=SD_OK)return errorstatus;

			SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4;
			SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
			SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
			SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
			SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
			SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
			SDIO_DataConfig(&SDIO_DataInitStructure);
				
			if(DeviceMode==SD_POLLING_MODE)
	    {
				timeout=SDIO_DATATIMEOUT;
				INTX_DISABLE();
				while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))
				{
					if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							
					{	  
						if((tlen-bytestransferred)<SD_HALFFIFOBYTES)
						{
							restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
							for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
								SDIO->FIFO=*tempbuff;
						}else 									
						{
							for(count=0;count<SD_HALFFIFO;count++)SDIO->FIFO=*(tempbuff+count);
							tempbuff+=SD_HALFFIFO;
							bytestransferred+=SD_HALFFIFOBYTES;
						}
						timeout=0X3FFFFFFF;	
					}else
					{
						if(timeout==0)return SD_DATA_TIMEOUT; 
						timeout--;
					}
				} 
				if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
				{										   
					SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 
					return SD_DATA_TIMEOUT;
				}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
				{
					SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
					return SD_DATA_CRC_FAIL;		   
				}else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	
				{
					SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		
					return SD_TX_UNDERRUN;		 
				}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
				{
					SDIO_ClearFlag(SDIO_FLAG_STBITERR);
					return SD_START_BIT_ERR;		 
				}       										   
				if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		
				{															 
					if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
					{   
						SDIO_CmdInitStructure.SDIO_Argument =0;  
						SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
						SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
						SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
						SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
						SDIO_SendCommand(&SDIO_CmdInitStructure);			
						errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
						if(errorstatus!=SD_OK)return errorstatus;	 
					}
				}
				INTX_ENABLE();
				SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	    }else if(DeviceMode==SD_DMA_MODE)
			{
				TransferError=SD_OK;
				StopCondition=1;		
				TransferEnd=0;			
				SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	
				SD_DMA_Config((u32*)buf,nblks*blksize,DMA_DIR_MemoryToPeripheral);	
				SDIO->DCTRL|=1<<3;							
				timeout=SDIO_DATATIMEOUT;
				while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;
				if(timeout==0){SD_Init();return SD_DATA_TIMEOUT;}
				timeout=SDIO_DATATIMEOUT;
				while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
				if(timeout==0)return SD_DATA_TIMEOUT;			
				if(TransferError!=SD_OK)return TransferError;	 
			}
	}
 	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}   
	return errorstatus;	   
}

/*
*********************************************************************************************************
*	函 数 名: SDIO_IRQHandler
*	功能说明: SDIO中断服务函数	
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SDIO_IRQHandler(void) 
{											
 	SD_ProcessIRQSrc();
}	 																    

/*
*********************************************************************************************************
*	函 数 名: SD_ProcessIRQSrc
*	功能说明: SDIO中断处理函数，处理SDIO传输过程中的各种中断事务
*	形    参：无
*	返 回 值: 错误代码
*********************************************************************************************************
*/
SD_Error SD_ProcessIRQSrc(void)
{
	if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
	{	 
		if (StopCondition==1)
		{  
			SDIO_CmdInitStructure.SDIO_Argument =0;  
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);			
			TransferError=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
 		SDIO->ICR|=1<<8;
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
 		TransferEnd = 1;
		return(TransferError);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
		TransferError = SD_DATA_CRC_FAIL;
		return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);  			
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
		TransferError = SD_DATA_TIMEOUT;
		return(SD_DATA_TIMEOUT);
	}
  if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);  		
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
		TransferError = SD_RX_OVERRUN;
		return(SD_RX_OVERRUN);
	}
  if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);  			
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
		TransferError = SD_TX_UNDERRUN;
		return(SD_TX_UNDERRUN);
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);  	
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));
		TransferError = SD_START_BIT_ERR;
		return(SD_START_BIT_ERR);
	}
	return(SD_OK);
}    				   

/*
*********************************************************************************************************
*	函 数 名: SD_SendStatus
*	功能说明: 读取当前卡状态
*	形    参：pcardstatus:卡状态
*	返 回 值: 错误状态
*********************************************************************************************************
*/
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
	SD_Error errorstatus = SD_OK;
	if(pcardstatus==NULL){errorstatus=SD_INVALID_PARAMETER;return errorstatus;}
	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; 
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);		
	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	
	if(errorstatus!=SD_OK)return errorstatus;
	*pcardstatus=SDIO->RESP1;
	return errorstatus;
} 

/*
*********************************************************************************************************
*	函 数 名: SD_GetState
*	功能说明: 读取当前卡状态
*	形    参：无
*	返 回 值: SD卡的状态
*********************************************************************************************************
*/
SDCardState SD_GetState(void)
{
	u32 resp1=0;
	if(SD_SendStatus(&resp1)!=SD_OK)return SD_CARD_ERROR;
	else return (SDCardState)((resp1>>9) & 0x0F);
}	 

/*
*********************************************************************************************************
*	函 数 名: SD_DMA_Config
*	功能说明: 配置SDIO DMA  
*	形    参：mbuf:存储器地址
						bufsize:传输数据量
						dir:方向;DMA_DIR_MemoryToPeripheral  存储器-->SDIO(写数据);DMA_DIR_PeripheralToMemory SDIO-->存储器(读数据);
*	返 回 值: 无
*********************************************************************************************************
*/
void SD_DMA_Config(u32*mbuf,u32 bufsize,u32 dir)
{		 
  DMA_InitTypeDef  DMA_InitStructure;	
	while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}	
  DMA_DeInit(DMA2_Stream3);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SDIO->FIFO;
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)mbuf;
  DMA_InitStructure.DMA_DIR = dir;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA2_Stream3, &DMA_InitStructure);
	DMA_FlowControllerConfig(DMA2_Stream3,DMA_FlowCtrl_Peripheral);
  DMA_Cmd(DMA2_Stream3 ,ENABLE);
}   

/*
*********************************************************************************************************
*	函 数 名: SD_ReadDisk
*	功能说明: 读SD卡
*	形    参：buf:读数据缓存区
						sector:扇区地址
						cnt:扇区个数	
*	返 回 值: 错误状态;0,正常;其他,错误代码;	
*********************************************************************************************************
*/
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	long long lsector=sector;
	u8 n;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=SD_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=SD_ReadBlock(buf,lsector,512);    	
		else sta=SD_ReadMultiBlocks(buf,lsector,512,cnt);
	}
	return sta;
}

/*
*********************************************************************************************************
*	函 数 名: SD_WriteDisk
*	功能说明: 写SD卡
*	形    参：buf:写数据缓存区
						sector:扇区地址
						cnt:扇区个数	
*	返 回 值: 错误状态;0,正常;其他,错误代码;	
*********************************************************************************************************
*/
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 sta=SD_OK;
	u8 n;
	long long lsector=sector;
	lsector<<=9;
	if((u32)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buf,512);
		 	sta=SD_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=SD_WriteBlock(buf,lsector,512);    
		else sta=SD_WriteMultiBlocks(buf,lsector,512,cnt);
	}
	return sta;
}
