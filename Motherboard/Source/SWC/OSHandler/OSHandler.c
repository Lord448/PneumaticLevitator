/**
 * @file      OSHandler.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Aug 5, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#include "OSHandler.h"

extern TIM_HandleTypeDef htim2;
extern osThreadId_t TaskLedsHandle;

/*
 * -------------------------------------------------------------
 *
 * 										O S   H A N D L I N G
 *
 * -------------------------------------------------------------
 */
result_t OS_ResetThread(osThreadId_t *thread_id, osThreadFunc_t func, const osThreadAttr_t *attr)
{
	result_t result = OK;
	if(NULL != *thread_id)
	{
		/* Terminating the task */
		if(osOK == osThreadTerminate(*thread_id))
		{
			/* Restarting the thread */
			*thread_id = osThreadNew(func, NULL, attr);
			if(NULL == *thread_id)
			{
				/* Could not create the thread */
				/* TODO: Trigger DTC OS Fatal error: Could not create the thread*/
				result = Error;
			}
			else
			{
				/* All Ok */
				result = OK;
			}
		}
		else
		{
			/* Fatal error */
			/* TODO: Trigger DTC OS Fatal error: finished thread not restarted*/
		}
	}
	else
	{
		/* The task has been deleted once or doesn't exist */
		/* TODO: Trigger DTC OS Fatal error: missing thread */
		result = Error;
	}
	return result;
}

/*
 * -------------------------------------------------------------
 *
 * 												O S   H O O K S
 *
 * -------------------------------------------------------------
 */

/**
 * @brief  This function it's called when a stack
 *         overflow is detected by the OS
 * @param  xTask: Task handle of the task that had a stack overflow
 * @param  pcTaskName: Task name of the task that had a stack overflow
 * @retval none
 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */

	/*TODO Trigger DTC Stack Overflow*/

	/* Mark animation with leds */
	osThreadSuspend(TaskLedsHandle); /* Cancel other led animations */
	HAL_GPIO_WritePin(LED_OR_GPIO_Port, LED_OR_Pin, 0);
	HAL_GPIO_WritePin(GPULed_GPIO_Port, GPULed_Pin, 0);
	for(uint16_t i = 0; i < 10; i++)
	{
		HAL_GPIO_TogglePin(RunningLed_GPIO_Port, RunningLed_Pin);
		HAL_Delay(50);
	}
	HAL_GPIO_WritePin(RunningLed_GPIO_Port, RunningLed_Pin, 0);
	HAL_Delay(100);
	/* TODO: Send to USB if possible */

	/* Reset the system */
	HAL_NVIC_SystemReset();
}

/**
 * @brief  Function defined by the freeRTOS API to
 *         configure the timer for the CPU load stats
 * @note   This function it's used for CPU load measure
 * @param  none
 * @revtal none
 */
void configureTimerForRunTimeStats(void)
{
	/* 32 bit timer with preescaler configured @ 1MHz */
	HAL_TIM_Base_Start(&htim2);
}

/**
 * @brief  Function defined by the freeRTOS API
 *         to get the current counts of the timer
 * @note   This function it's used for CPU load measure
 * @param  none
 * @retval The counts of the timer
 */
unsigned long getRunTimeCounterValue(void)
{
	return __HAL_TIM_GET_COUNTER(&htim2);
}

/*
 * -------------------------------------------------------------
 *
 * 										 S T A T S   C H E C K S
 *
 * -------------------------------------------------------------
 */

/**
 * @brief  returns the heap total usage in percentage
 * @param  none
 * @retval the heap total use in percentage
 */
uint16_t usCheckHeapUsage(void)
{
	uint16_t Percentage;

	size_t TotalHeapSize = configTOTAL_HEAP_SIZE;
	size_t UsedHeapSize = TotalHeapSize - xPortGetFreeHeapSize();
	Percentage = (UsedHeapSize / TotalHeapSize) * 100;

	return Percentage;
}


/**
 * @brief  returns the stack usage of a task
 * @param  xTask: Task handle of the task that will be analyzed
 * @retval percentage of the stack usage
 */
uint16_t usCheckStackUsage(xTaskHandle xTask, uint32_t *pwStackBuffer)
{
	return 0;
}

/**
 * @brief
 * @param
 * @retval
 */
result_t xResetTask(xTaskHandle xTask)
{
	return OK; //TODO: Stubbed code
}
