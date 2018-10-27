/*
*********************************************************************************************************
*	                                  
*	ģ������ : ϵͳ�ж��趨ģ��
*	�ļ����� : nvic.c
*	��    �� : V1.3
*	˵    �� : ϵͳ�ж��趨ģ��
*
*
*********************************************************************************************************
*/

#include "exti.h"
#include "debug_assert.h"
#include "FreeRTOS.h"		 
#include "task.h"

static u32 sysTickCnt=0;

/*
*********************************************************************************************************
*	�� �� ��: nvicInit
*	����˵��: �ж��������ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void nvicInit(void)
{
	//NVIC_SetVectorTable(FIRMWARE_START_ADDR,0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

extern void xPortSysTickHandler(void);

/*
*********************************************************************************************************
*	�� �� ��: SysTick_Handler
*	����˵��: �δ�ʱ���жϷ�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  SysTick_Handler(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	
  {
		xPortSysTickHandler();	
  }else
	{
		sysTickCnt++;	/*���ȿ���֮ǰ����*/
	}
}

/*
*********************************************************************************************************
*	�� �� ��: getSysTickCnt
*	����˵��: ���ȿ���֮ǰ ���� sysTickCnt
						���ȿ���֮ǰ ���� xTaskGetTickCount()
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
u32 getSysTickCnt(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
		return xTaskGetTickCount();
	else
		return sysTickCnt;
}

/*
*********************************************************************************************************
*	�� �� ��: HardFault_Handler
*	����˵��: This function handles Hard Fault exception
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
__asm void HardFault_Handler(void)
{
	//http://www.st.com/mcu/forums-cat-6778-23.html
	//****************************************************
	//To test this application, you can use this snippet anywhere:
	// //Let's crash the MCU!
	// asm (" MOVS r0, #1 \n"
	// " LDM r0,{r1-r2} \n"
	// " BX LR; \n");
	PRESERVE8
    IMPORT printHardFault
    TST r14, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B printHardFault
}

/*
*********************************************************************************************************
*	�� �� ��: printHardFault
*	����˵��: ���Ӳ���жϴ�����Ϣ
*	��    �Σ�hardfaultArgs,������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  printHardFault(u32* hardfaultArgs)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfaultArgs[0]);
	stacked_r1 = ((unsigned long) hardfaultArgs[1]);
	stacked_r2 = ((unsigned long) hardfaultArgs[2]);
	stacked_r3 = ((unsigned long) hardfaultArgs[3]);

	stacked_r12 = ((unsigned long) hardfaultArgs[4]);
	stacked_lr = ((unsigned long) hardfaultArgs[5]);
	stacked_pc = ((unsigned long) hardfaultArgs[6]);
	stacked_psr = ((unsigned long) hardfaultArgs[7]);


	printf("[Hard fault handler]\n");
	printf("R0 = %x\n", stacked_r0);
	printf("R1 = %x\n", stacked_r1);
	printf("R2 = %x\n", stacked_r2);
	printf("R3 = %x\n", stacked_r3);
	printf("R12 = %x\n", stacked_r12);
	printf("LR = %x\n", stacked_lr);
	printf("PC = %x\n", stacked_pc);
	printf("PSR = %x\n", stacked_psr);
	printf("BFAR = %x\n", (*((volatile unsigned int *)(0xE000ED38))));
	printf("CFSR = %x\n", (*((volatile unsigned int *)(0xE000ED28))));
	printf("HFSR = %x\n", (*((volatile unsigned int *)(0xE000ED2C))));
	printf("DFSR = %x\n", (*((volatile unsigned int *)(0xE000ED30))));
	printf("AFSR = %x\n", (*((volatile unsigned int *)(0xE000ED3C))));

//	motorsSetRatio(MOTOR_M1, 0);
//	motorsSetRatio(MOTOR_M2, 0);
//	motorsSetRatio(MOTOR_M3, 0);
//	motorsSetRatio(MOTOR_M4, 0);

//	ledClearAll();
//	ledSet(ERR_LED1, 1);	/*������*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	�� �� ��: MemManage_Handler
*	����˵��: This function handles Memory Manage exception.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
//	motorsSetRatio(MOTOR_M1, 0);
//	motorsSetRatio(MOTOR_M2, 0);
//	motorsSetRatio(MOTOR_M3, 0);
//	motorsSetRatio(MOTOR_M4, 0);

//	ledClearAll();
//	ledSet(ERR_LED1, 1);/*������*/
//	ledSet(ERR_LED2, 1);

	//storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	�� �� ��: BusFault_Handler
*	����˵��: This function handles Bus Fault exception.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
//	motorsSetRatio(MOTOR_M1, 0);
//	motorsSetRatio(MOTOR_M2, 0);
//	motorsSetRatio(MOTOR_M3, 0);
//	motorsSetRatio(MOTOR_M4, 0);

//	ledClearAll();
//	ledSet(ERR_LED1, 1);/*������*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	�� �� ��: UsageFault_Handler
*	����˵��: This function handles Usage Fault exception
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
//	motorsSetRatio(MOTOR_M1, 0);
//	motorsSetRatio(MOTOR_M2, 0);
//	motorsSetRatio(MOTOR_M3, 0);
//	motorsSetRatio(MOTOR_M4, 0);

//	ledClearAll();
//	ledSet(ERR_LED1, 1);/*������*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	�� �� ��: DebugMon_Handler
*	����˵��: This function handles Debug Monitor exception.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  DebugMon_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: DMA2_Stream6_IRQHandler
*	����˵��: 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  DMA2_Stream6_IRQHandler(void)
{
}
