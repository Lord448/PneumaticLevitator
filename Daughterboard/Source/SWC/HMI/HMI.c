/**
 * @file      HMI.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      28 jul 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#include "HMI.h"

extern osMessageQueueId_t xFIFOButtonsHandle;
extern osEventFlagsId_t xEventButtonsFIFOEnabledHandle;

void vTaskHMI(void *argument)
{
	uint32_t FIFOEnabled = osEventFlagsGet(xEventButtonsFIFOEnabledHandle);

	for(;;)
	{
		/* Debouncing the buttons */

		if(FIFOEnabled)
		{
			/* FIFO Style required */
		}
		else
		{
			/* No FIFO logic required */
		}
	}
}

/**
 * @brief
 * @param
 * @retval
 */
result_t HMI_FIFOEnable(void)
{
	result_t result;
	result = osEventFlagsSet(xEventButtonsFIFOEnabledHandle, FIFO_ENABLED_FLAG);
	if(result == OK)
	{
		/* All OK, reset the FIFO */
		result = osMessageQueueReset(xFIFOButtonsHandle);
	}
	else
	{
		/* Do Nothing */
	}
	return result;
}

/**
 * @brief
 * @param
 * @retval
 */
result_t HMI_FIFODisable(void)
{
	result_t result;
	result = osEventFlagsClear(xEventButtonsFIFOEnabledHandle, FIFO_ENABLED_FLAG);
	if(OK == result)
	{
		/* All OK, reset the FIFO */
		result = osMessageQueueReset(xFIFOButtonsHandle);
	}
	else
	{
		/* Do nothing */
	}
	return result;
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(Reset_IT_Pin == GPIO_Pin)
	{
		/* Communicate the Motherboard the reset request */
		HAL_NVIC_SystemReset(); /* TODO Instrumented code */
	}
}
