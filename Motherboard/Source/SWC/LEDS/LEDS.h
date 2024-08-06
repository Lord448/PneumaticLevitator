/**
 * @file      LEDS.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component helps to make animations
 *            with the LEDs provided by the hardware designed
 *            the main animation error that shows the code of
 *            the fatal errors is the ORLed_ToggleAnim
 *
 * @date      5 ago 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_DIAGAPPL_LEDS_H_
#define SWC_DIAGAPPL_LEDS_H_

#include "main.h"

#define RunningLed #Runnig
#define GPULed #GPU
#define ORLed #LED_OR

#define GPULed_Turn(state) HAL_GPIO_WritePin(GPULed_GPIO_Port, GPULed_Pin, state)
#define RunningLed_Turn(state) HAL_GPIO_WritePin(RunningLed_GPIO_Port, RunningLed_Pin, state)

#define Leds_Toggle(Led) HAL_GPIO_TogglePin(Led##_GPIO_Port, Led##_Pin)

typedef enum ORLed_Colors {
	Red,
	White,
	Toggle,
}ORLed_Colors;

typedef enum LedStates {
	OFF,
	ON
}LedStates;

typedef enum AnimStates{
	Running,
	Finished,
	Wait
}AnimStates;

typedef AnimStates (*AnimFuncPtr)(uint16_t);

void vTaskLeds(void *argument);

#endif /* SWC_DIAGAPPL_LEDS_H_ */
