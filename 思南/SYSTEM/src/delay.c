/*
*********************************************************************************************************
*	                                  
*	模块名称 : 延时函数模块
*	文件名称 : delay.c
*	版    本 : V1.3
*	说    明 : 延时函数模块
*
*
*********************************************************************************************************
*/

#include "delay.h"
#include "sys.h"

#include "FreeRTOS.h"				
#include "task.h"

static u8  fac_us=0;					   
static u16 fac_ms=0;	

/*
*********************************************************************************************************
*	函 数 名: delay_init
*	功能说明: 初始化延迟函数
						SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8
						这里为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率！
*	形    参：SYSCLK:系统时钟频率
*	返 回 值: 无
*********************************************************************************************************
*/
void delay_init(u8 SYSCLK)
{
	u32 reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 
	fac_us=SYSCLK;					
	reload=SYSCLK;						
	reload*=1000000/configTICK_RATE_HZ;		
	fac_ms=1000/configTICK_RATE_HZ;			  
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD=reload; 				
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   
}								    

/*
*********************************************************************************************************
*	函 数 名: delay_us
*	功能说明: 延时nus
*	形    参：nus:要延时的us数.	
						nus:0~204522252(最大值即2^32/fac_us@fac_us=168)	
*	返 回 值: 无
*********************************************************************************************************
*/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;			 
	ticks=nus*fac_us; 				
	told=SysTick->VAL;        			
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;
		}  
	};										    
}  

/*
*********************************************************************************************************
*	函 数 名: delay_ms
*	功能说明: 延时nms
*	形    参：nms:要延时的ms数
						nms:0~65535
*	返 回 值: 无
*********************************************************************************************************
*/
void delay_ms(u32 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{		
		if(nms>=fac_ms)					
   			vTaskDelay(nms/fac_ms);	 	
		nms%=fac_ms;		
	}
	delay_us((u32)(nms*1000));	
}

/*
*********************************************************************************************************
*	函 数 名: delay_xms
*	功能说明: 延时nms,延时nms,不会引起任务调度
*	形    参：nms为要延时的ms数.	
*	返 回 值: 无
*********************************************************************************************************
*/
void delay_xms(u32 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}
