/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��ʱ����ģ��
*	�ļ����� : delay.c
*	��    �� : V1.3
*	˵    �� : ��ʱ����ģ��
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
*	�� �� ��: delay_init
*	����˵��: ��ʼ���ӳٺ���
						SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӣ�������������SYSTICKʱ��Ƶ��ΪAHB/8
						����Ϊ�˼���FreeRTOS�����Խ�SYSTICK��ʱ��Ƶ�ʸ�ΪAHB��Ƶ�ʣ�
*	��    �Σ�SYSCLK:ϵͳʱ��Ƶ��
*	�� �� ֵ: ��
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
*	�� �� ��: delay_us
*	����˵��: ��ʱnus
*	��    �Σ�nus:Ҫ��ʱ��us��.	
						nus:0~204522252(���ֵ��2^32/fac_us@fac_us=168)	
*	�� �� ֵ: ��
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
*	�� �� ��: delay_ms
*	����˵��: ��ʱnms
*	��    �Σ�nms:Ҫ��ʱ��ms��
						nms:0~65535
*	�� �� ֵ: ��
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
*	�� �� ��: delay_xms
*	����˵��: ��ʱnms,��ʱnms,���������������
*	��    �Σ�nmsΪҪ��ʱ��ms��.	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void delay_xms(u32 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}
