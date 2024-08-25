/**
 * @file      FAN.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      May 29, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#include "FAN.h"

extern TIM_HandleTypeDef htim3; /* Fan PWM controller @ 24KHz (manufacturer recommended frequency) */
extern TIM_HandleTypeDef htim4; /* Fan RPM measure (Input Capture) @ 1MHz */

extern osMessageQueueId_t xFIFO_COMRPMHandle;
extern osMessageQueueId_t xFIFO_COMActionControlHandle;

/**
* @brief Function implementing the TaskFAN thread.
* @param argument: Not used
* @retval None
*/
void vTaskFAN(void *argument)
{
	int8_t dutyCycle = 0;
	uint16_t timeCounter = 0;
	TickType_t ticks;

	EnableFAN(false);
	EnableFAN(true);
	setPWM_FAN(0);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	ticks = osKernelGetTickCount();
	for(;;)
	{
		if(timeCounter > 10) /* Each 100ms */
		{
			setPWM_FAN(dutyCycle);
			dutyCycle++;
			if(dutyCycle > 100)
				dutyCycle = 0;
			timeCounter = 0;
		}
		osMessageQueuePut(xFIFO_COMActionControlHandle, &dutyCycle, 0U, osNoTimeout);
		timeCounter++;
		ticks+=pdMS_TO_TICKS(10);
		osDelayUntil(ticks);
	}
}

/**
 * ---------------------------------------------------------
 * 					      ISR COMPONENT CALLBACKS
 * ---------------------------------------------------------
 */
#ifdef DEBUG_IC
	#ifdef CALCULATE_RPM_IN_HZ
		uint32_t frequencyHZ; /* Debug proposes */
	#endif
	int16_t rpm;
#endif

/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  htim TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static uint16_t IC_Val1 = 0;
	static uint16_t IC_Val2 = 0;
	static bool Is_First_Captured = true;
	uint16_t countsDifference = 0;
#ifndef DEBUG_IC
	#ifdef CALCULATE_RPM_IN_HZ
		uint32_t frequencyHZ; /* Debug proposes */
	#endif
		int32_t rpm;
#endif

	if(Is_First_Captured)
	{
		/* It's the first capture of the IC */
		IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		Is_First_Captured = false;
	}
	else
	{
		/* Normal code flow */
		IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		/* When using 16 bit arithmetics the value it's correct even if IC_Val2 < IC_Val1 */
		countsDifference = IC_Val2 - IC_Val1;
		/* Saving the past value of IC */
		IC_Val1 = IC_Val2;
#ifdef CALCULATE_RPM_IN_HZ
		frequencyHZ = (CPU_CLOCK/countsDifference);
#endif
		rpm = (CPU_CLOCK/countsDifference) * 60;
		osMessageQueuePut(xFIFO_COMRPMHandle, &rpm, 0U, osNoTimeout); /*Sending to COM*/
	}
}
