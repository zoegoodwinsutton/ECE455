/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4_discovery.h"

/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100



/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );
static void shiftRegister(int on);

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */

static void trafficFlowAdjustmentTask(void *pvParameters);
static void trafficGenerationTask(void *pvParameters);
static void trafficLightStateTask(void *pvParameters);
static void systemDisplayTask(void *pvParameters);

void amberLightCallback();
void greenLightCallback();
void redLightCallback();



xQueueHandle xQueue; //adc val
xQueueHandle xTrafficQueue; //car array

xTimerHandle xRedLightTimer;
xTimerHandle xAmberLightTimer;
xTimerHandle xGreenLightTimer;

/*-----------------------------------------------------------*/

int main(void)
{
	prvSetupHardware();
	/* Initialize LEDs */

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */

	xQueue = xQueueCreate(1, sizeof(int));
	xTrafficQueue = xQueueCreate(1, sizeof(int));


	xTaskCreate(trafficFlowAdjustmentTask, "flowAdjustment", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(trafficGenerationTask, "trafficGeneration", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(trafficLightStateTask, "trafficLightState", configMINIMAL_STACK_SIZE,NULL,1, NULL);
	xTaskCreate(systemDisplayTask, "systemDisplay", configMINIMAL_STACK_SIZE, NULL,1, NULL);

	xRedLightTimer = xTimerCreate("RedLightTimer",1000,pdFALSE,(void *)0, redLightCallback);
	xGreenLightTimer = xTimerCreate("GreenLightTimer",1000,pdFALSE,(void *)0, greenLightCallback);
	xAmberLightTimer = xTimerCreate("AmberLightTimer",2000,pdFALSE,(void *)0, amberLightCallback);

	xTimerStart(xGreenLightTimer,0); //start Green light
	GPIO_SetBits(GPIOC, GPIO_Pin_2); //turn on green

	vTaskStartScheduler();
	return 0;
}
void amberLightCallback(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_1); //turn off amber
	GPIO_SetBits(GPIOC, GPIO_Pin_0);//turn on red
	xTimerStart(xRedLightTimer,0);

}
void greenLightCallback(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_2); //turn off green
	GPIO_SetBits(GPIOC, GPIO_Pin_1);//turn on amber
	xTimerStart(xAmberLightTimer,0);

}
void redLightCallback(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_0); //turn off red
	GPIO_SetBits(GPIOC, GPIO_Pin_2);//turn on green
	xTimerStart(xGreenLightTimer,0);

}

/*-----------------------------------------------------------*/
/* Purpose: This task stores an array of 19 integers which models the LEDs.
 * 			It shifts the integers in the array according to what light is on.
 * 			It receives a new random bit from the trafficGenerationTask.
 * 			Then it calls the shiftRegister function to display the correct lights on the board.
 */
static void systemDisplayTask(void *pvParameters){
	const TickType_t xTicksToWait = pdMS_TO_TICKS(0);
	int traffic[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int newCar = 0;
	while(1){
		vTaskDelay(1000);
		//get new bit from TrafficGenerationTask
		xQueueReceive(xTrafficQueue, &newCar, xTicksToWait);
		if(xTimerIsTimerActive(xGreenLightTimer)){
			//shift all LEDs down by one
			for(int i = 17; i >= 0; i--){
				traffic[i+1]= traffic[i];
			}
			//randomly generated new traffic
			traffic[0] = newCar;
		}else if(xTimerIsTimerActive(xRedLightTimer)|| xTimerIsTimerActive(xAmberLightTimer)){
			//Shift all LEDs on the right side of the stop down by one
			for(int i = 17; i >= 8; i--){
				traffic[i+1]= traffic[i];
			}
			traffic[8] = 0;
			// this flag indicates whether or not to add new traffic
			int arrayFullFlag = 0;
			//Compresses the traffic on the left hand side of the stop
			for(int i = 7; i >= 0; i--){
				if(traffic[i] == 0){
					for(int j = i; j > 0; j--){
						traffic[j] = traffic[j-1];
					}
					break;
				}
				if(i ==0){
					arrayFullFlag = 1;
				}

			}
			//if array is not full add new traffic
			if(arrayFullFlag != 1){
				traffic[0] = newCar;
			}
		}
		//Display bits on board
		for(int i = 18; i >= 0 ; i--){
			shiftRegister(traffic[i]);
		}
	}

}

/*-----------------------------------------------------------*/
/*
 * Purpose: This task reads values from the ADC passed through the xQueue.
 * 			Then modifies the periods of the traffic light timers according to the new
 * 			adc value. The value being passed from the TrafficFlowAdjustment task
 * 			should be between 0(minimum traffic) and 5(maximum traffic).
 */
static void trafficLightStateTask(void *pvParameters){
	const TickType_t xTicksToWait = pdMS_TO_TICKS(0);
	int trafficFlow, trafficFlowOriginal;
	xQueueReceive(xQueue, &trafficFlowOriginal, xTicksToWait);
	while(1){
		xQueueReceive(xQueue, &trafficFlow, xTicksToWait);
		if(trafficFlow != trafficFlowOriginal){
			trafficFlowOriginal = trafficFlow;
			if(xTimerIsTimerActive(xRedLightTimer)){
				xTimerStop(xRedLightTimer, 0);
				xTimerChangePeriod(xRedLightTimer,-420*trafficFlow + 4200 , 0);
				xTimerChangePeriod(xGreenLightTimer, 420*trafficFlow + 2100, 0);
				xTimerStop(xGreenLightTimer, 0);
			}else if(xTimerIsTimerActive(xGreenLightTimer)){
				xTimerChangePeriod(xRedLightTimer,-420*trafficFlow + 4200, 0);
				xTimerStop(xRedLightTimer, 0);
				xTimerStop(xGreenLightTimer, 0);
				xTimerChangePeriod(xGreenLightTimer, 420*trafficFlow + 2100, 0);
			}else if(xTimerIsTimerActive(xAmberLightTimer)){
				xTimerChangePeriod(xRedLightTimer, -420*trafficFlow + 4200, 0);
				xTimerStop(xRedLightTimer, 0);
				xTimerChangePeriod(xGreenLightTimer,  420*trafficFlow + 2100, 0);
				xTimerStop(xGreenLightTimer, 0);
			}
		}
	}

}

/*-----------------------------------------------------------*/
/* Purpose: Based on the potentiometer value this task generates a random value
 * 			between 0 and 1. 1 represents a car, and 0 represents not a car.
 * 			The value is sent through the xTrafficQueue to the SystemDisplayTask.
 *
 */
static void trafficGenerationTask(void *pvParameters){
	const TickType_t xTicksToWait = pdMS_TO_TICKS(0);
	int trafficFlow = 0;
	int val = 0;
	while(1){
		vTaskDelay(1000);
		xQueuePeek(xQueue, &trafficFlow, xTicksToWait);
		int randomNum = rand() % 6;
		if(randomNum <=  trafficFlow){
			val = 1;
		}else{
			val = 0;
		}
		xQueueSendToBack(xTrafficQueue, &val, 0);
	}

}

/*-----------------------------------------------------------*/
/*
 * Purpose: This task reads value from the ADC and divides it by 682.
 * 			The resulting values of trafficFlow are between 0 and 5.
 * 			0 represents low traffic and 5 is max traffic. The value is then
 * 			sent using xQueue to the other tasks.
 */
static void trafficFlowAdjustmentTask(void *pvParameters){
	int trafficFlow;
	while(1){
		if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)){
			trafficFlow = ADC_GetConversionValue(ADC1)/682;
			xQueueSendToBack(xQueue,&trafficFlow , 0);
			ADC_SoftwareStartConv(ADC1);
		}
	}

}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
static void shiftRegister(int on){
	if(on == 1){ //push an LED that is on
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
	}else{//Push LED that is off
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
	/*Enable clock for ADC and GPIOB peripherals */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Initialize GPIOC for ADC */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* Initialize GPIOC for LEDs */
	GPIO_InitTypeDef LED_GPIO_InitStruct;
	LED_GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	LED_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	LED_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	LED_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &LED_GPIO_InitStruct);

	/* Initialize GPIOC for Shift Registers */
	GPIO_InitTypeDef Shift_GPIO_InitStruct;
	Shift_GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	Shift_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	Shift_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	Shift_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Shift_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(GPIOC, &Shift_GPIO_InitStruct);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);

	/* Initialize ADC */
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1, &ADC_InitStruct);

	/* Start ADC */
	ADC_Cmd(ADC1, ENABLE);

	/* Initialize Channel */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);

	/* Start conversion */
	ADC_SoftwareStartConv(ADC1);

}
