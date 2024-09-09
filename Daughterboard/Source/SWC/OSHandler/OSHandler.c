/**
 * @file      OSHandler.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This component implements the OS hooks such as
 *            stack overflow detection, configurations of the
 *            SWIT (Software integration) statistics like the
 *            CPU Load measures and the check of the heap usage
 *            in runtime
 *
 * @date      Jul 9, 2024
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
uint16_t usCheckStackUsage(xTaskHandle xTask)
{
	return 0;
}

