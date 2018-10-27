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
*	�� �� ��: start_task
*	����˵��: ��ʼ����
*	��    �Σ�pvParameters,����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void start_task(void *pvParameters)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);
	
	taskENTER_CRITICAL();
	xTaskCreate(maslinkTask, "MASLINK", 150, NULL, 3, NULL);//�������ݴ����������
	xTaskCreate(commanderTask, "COMMANDER", 150, NULL, 3, NULL);  //��������׼����������
	xTaskCreate(atkpRxAnlTask, "ATKP", 200, NULL, 4, NULL);//�������ݽ�������
	xTaskCreate(radiolinkDataProcessTask, "RADIOLINK", 200, NULL, 4, NULL);//�������ݽ�������
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
	
	__WFI();	/*����͹���ģʽ*/
}
