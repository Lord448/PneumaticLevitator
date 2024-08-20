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

/* TODO: Paavali's code*/
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;
/* Mesure frecuency */
float frecuency = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // <--- delete
	{
		if(Is_First_Captured==0)
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			Is_First_Captured = 1;
		}
		else
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}
			//Register tics with ...
		  else if(IC_Val2 < IC_Val1)
		  {
		  	Difference = (0xffff - IC_Val1) + IC_Val2;
		  }
			uint32_t pclk1 = HAL_RCC_GetPCLK1Freq() * 2;
			uint32_t prescaler = (htim->Instance->PSC + 1);
			float refClock = pclk1/prescaler;
			frecuency = refClock/Difference;
			__HAL_TIM_SET_COUNTER(htim,0);
			Is_First_Captured=0;
		}
	}
}
// 1 comprobar
// 2 dividir
// 3 implementar with SO

// 4 medir RMP max ->> Resolution
// 5 muestrear 10 veces, comprobar n veces


//FIFO and TIC TIME
//mode continious

//Add table of diagnostic
//Add counter
