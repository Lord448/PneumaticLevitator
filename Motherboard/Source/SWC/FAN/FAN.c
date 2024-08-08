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
extern TIM_HandleTypeDef htim4; /* Fan RPM measure (Input Capture) @ */

/**
* @brief Function implementing the TaskFAN thread.
* @param argument: Not used
* @retval None
*/
void vTaskFAN(void *argument)
{
	//uint16_t MaxCCR = htim3.Init.Period;
	EnableFAN(0);
	setPWM_FAN(0);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	for(;;)
	{
		osDelay(1);
	}
}
