/*
*********************************************************************************************************
*	                                  
*	模块名称 : 系统中断设定模块
*	文件名称 : nvic.c
*	版    本 : V1.3
*	说    明 : 系统中断设定模块
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
*	函 数 名: nvicInit
*	功能说明: 中断向量表初始化
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: SysTick_Handler
*	功能说明: 滴答定时器中断服务函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void  SysTick_Handler(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	
  {
		xPortSysTickHandler();	
  }else
	{
		sysTickCnt++;	/*调度开启之前计数*/
	}
}

/*
*********************************************************************************************************
*	函 数 名: getSysTickCnt
*	功能说明: 调度开启之前 返回 sysTickCnt
						调度开启之前 返回 xTaskGetTickCount()
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: HardFault_Handler
*	功能说明: This function handles Hard Fault exception
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: printHardFault
*	功能说明: 输出硬件中断错误信息
*	形    参：hardfaultArgs,错误码
*	返 回 值: 无
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
//	ledSet(ERR_LED1, 1);	/*错误检测*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	函 数 名: MemManage_Handler
*	功能说明: This function handles Memory Manage exception.
*	形    参：无
*	返 回 值: 无
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
//	ledSet(ERR_LED1, 1);/*错误检测*/
//	ledSet(ERR_LED2, 1);

	//storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	函 数 名: BusFault_Handler
*	功能说明: This function handles Bus Fault exception.
*	形    参：无
*	返 回 值: 无
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
//	ledSet(ERR_LED1, 1);/*错误检测*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	函 数 名: UsageFault_Handler
*	功能说明: This function handles Usage Fault exception
*	形    参：无
*	返 回 值: 无
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
//	ledSet(ERR_LED1, 1);/*错误检测*/
//	ledSet(ERR_LED2, 1);

//	storeAssertSnapshotData(__FILE__, __LINE__);
	while (1)
	{}
}

/*
*********************************************************************************************************
*	函 数 名: DebugMon_Handler
*	功能说明: This function handles Debug Monitor exception.
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void  DebugMon_Handler(void)
{
}

/*
*********************************************************************************************************
*	函 数 名: DMA2_Stream6_IRQHandler
*	功能说明: 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void  DMA2_Stream6_IRQHandler(void)
{
}
