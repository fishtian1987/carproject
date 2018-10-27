#include "system.h"	

TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

int main(void)
{
	systemInit();
  xTaskCreate(start_task,"start_task",256,NULL,1,&StartTask_Handler);           
  vTaskStartScheduler();         
}

/*
*********************************************************************************************************
*	函 数 名: start_task
*	功能说明: 开始任务
*	形    参：pvParameters,参数
*	返 回 值: 无
*********************************************************************************************************
*/
void start_task(void *pvParameters)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);
		
	taskENTER_CRITICAL();    
	xTaskCreate(armlinkReciveTask, "ARMRTASK", 150, NULL, 4, NULL);//创建与arm板接收任务
	xTaskCreate(armlinkTask, "ARMTASK", 150, NULL, 4, NULL);//创建与arm板发送任务
	xTaskCreate(maslinkTask, "MASLINK", 200, NULL, 2, NULL);//创建与下主板连接任务
	xTaskCreate(atkpRxAnlTask, "ATKPRXANL", 200, NULL, 2, NULL);//创建atkp解析任务	
	xTaskCreate(stabilizerTask, "STABILIZER", 300, NULL, 3, NULL);//创建姿态任务
	xTaskCreate(ioTask, "IO", 160, NULL, 1, NULL);//创建数显存储任务
	xTaskCreate(configParamTask, "CONFIG_TASK", 150, NULL, 4, NULL);/*创建参数配置任务*/
	vTaskDelete(StartTask_Handler); 
	taskEXIT_CRITICAL();    
}

void vApplicationIdleHook( void )
{
	//static u32 tickWatchdogReset = 0;

	portTickType tickCount = getSysTickCnt();

//	if (tickCount - tickWatchdogReset > WATCHDOG_RESET_MS)
//	{
//		tickWatchdogReset = tickCount;
//		watchdogReset();
//	}
	
	__WFI();	/*进入低功耗模式*/
}
