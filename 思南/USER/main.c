#include "system.h"	
#include "hmi.h"	

TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);
void tx_task(void *pvParameters);

int main(void)
{
	systemInit();
	xTaskCreate(start_task,"start_task",250,NULL,1,&StartTask_Handler);           
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
	xTaskCreate(maslinkTask, "MASLINK", 150, NULL, 3, NULL);//创建数据打包发送任务
	xTaskCreate(commanderTask, "COMMANDER", 150, NULL, 3, NULL);  //创建数据准备发送任务
	xTaskCreate(atkpRxAnlTask, "ATKP", 200, NULL, 4, NULL);//创建数据解析任务
	xTaskCreate(radiolinkDataProcessTask, "RADIOLINK", 200, NULL, 4, NULL);//创建数据接收任务
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
