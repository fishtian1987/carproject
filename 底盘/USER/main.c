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
	xTaskCreate(armlinkReciveTask, "ARMRTASK", 150, NULL, 4, NULL);//������arm���������
	xTaskCreate(armlinkTask, "ARMTASK", 150, NULL, 4, NULL);//������arm�巢������
	xTaskCreate(maslinkTask, "MASLINK", 200, NULL, 2, NULL);//��������������������
	xTaskCreate(atkpRxAnlTask, "ATKPRXANL", 200, NULL, 2, NULL);//����atkp��������	
	xTaskCreate(stabilizerTask, "STABILIZER", 300, NULL, 3, NULL);//������̬����
	xTaskCreate(ioTask, "IO", 160, NULL, 1, NULL);//�������Դ洢����
	xTaskCreate(configParamTask, "CONFIG_TASK", 150, NULL, 4, NULL);/*����������������*/
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
