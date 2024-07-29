/**
 * @file      OSHandler.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
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

