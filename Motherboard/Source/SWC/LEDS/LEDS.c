/**
 * @file      LEDS.c
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
#include "LEDS.h"

extern osEventFlagsId_t xEvent_FatalErrorHandle;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static AnimStates ORLed_ToggleAnim(AnimFuncPtr func,  uint16_t times);
static AnimStates RunningLed_BlinkAnim(uint16_t times);
static void GPULed_BlinkAnim(void);
static void ORLed_SetColor(ORLed_Colors color);

/**
 * ---------------------------------------------------------
 * 					             OS TASK
 * ---------------------------------------------------------
 */

/**
 * @brief  This function set the initial states
 *         of the leds in the Motherboard
 * @param  none
 * @retval none
 */
static void Leds_Init(void)
{
	ORLed_SetColor(White);
	GPULed_Turn(OFF);
	RunningLed_Turn(ON);
}

/**
* @brief Function implementing the TaskLeds thread.
* @param argument: Not used
* @retval None
*/
void vTaskLeds(void *argument)
{
	const TickType_t taskResolutionMS = 10;
	TickType_t tick = 0;
	uint32_t eventFlags = 0;

	Leds_Init();
	tick = osKernelGetTickCount();

  for(;;)
  {
  	/* Check for fatal error events */
  	eventFlags = osEventFlagsGet(xEvent_FatalErrorHandle);

  	if(eventFlags & FATAL_ERROR_SYSTEM)
  	{
  		/* FATAL_ERROR_SYSTEM flag turned on */
  	}
  	if(eventFlags & FATAL_ERROR_GPU)
  	{
  		/* FATAL_ERROR_GPU flag turned on */
  		GPULed_BlinkAnim();
  	}
  	if(eventFlags & FATAL_ERROR_EEPROM)
  	{
  		/* FATAL_ERROR_EEPROM flag turned on */
  		ORLed_ToggleAnim(&RunningLed_BlinkAnim, FATAL_ERROR_GPU_TIMES);
  	}

  	/* Parsed loop with 10 ms of resolution*/
  	tick += pdMS_TO_TICKS(taskResolutionMS);
    osDelayUntil(tick);
  }
}

/**
 * ---------------------------------------------------------
 * 					    SOFTWARE COMPONENT ANIMATIONS
 * ---------------------------------------------------------
 */
static AnimStates ORLed_ToggleAnim(AnimFuncPtr func, uint16_t times)
{
	enum SMStates {
		sInitialToggle,
		sAnim,
		sFinalToggle
	}static stateHandler = sInitialToggle;

	const uint16_t makeToggleCountMS = 25;
	const uint16_t numberOfToggles = 5;
	static uint16_t tickCounter = 0;
	static uint16_t toggleCounter = 0;
	AnimStates retval = Running;

	switch(stateHandler)
	{
		case sInitialToggle:
			if(makeToggleCountMS == tickCounter)
			{
				/* The count of the toggle has been reached */
				ORLed_SetColor(Toggle);
				toggleCounter++;
				tickCounter = 0;
			}
			else if(numberOfToggles == toggleCounter)
			{
				/* The number of toggles has been reached */
				//ORLed_SetColor(White);
				toggleCounter = 0;
				stateHandler = sAnim;
			}
			else
			{
				/* Do Nothing */
			}
		break;
		case sAnim:
			if(Finished == func(times))
			{
				stateHandler = sFinalToggle;
				tickCounter = 0;
			}
			else
			{
				/* Do Nothing */
			}
		break;
		case sFinalToggle:
			if(makeToggleCountMS == tickCounter)
			{
				/* The count of the toggle has been reached */
				ORLed_SetColor(Toggle);
				toggleCounter++;
				tickCounter = 0;
			}
			else if(numberOfToggles == toggleCounter)
			{
				/* The number of toggles has been reached */
				//ORLed_SetColor(White);
				toggleCounter = 0;
				stateHandler = sInitialToggle;
				retval = Finished;
			}
			else
			{
				/* Do Nothing */
			}
		break;
	}
	tickCounter++;
	return retval;
}

static AnimStates RunningLed_BlinkAnim(uint16_t times)
{
	const uint16_t blinkPeriodMS = 100;
	static uint16_t blinkCounter = 0;
	static uint16_t timesCounter = 0;
	AnimStates retval = Running;

	if(2*times == timesCounter)
	{
		/* Reached the number of blinks */
		RunningLed_Turn(ON);
		timesCounter = 0;
		retval = Finished;
	}
	if(blinkPeriodMS == blinkCounter)
	{
		/* Time to make a blink */
		Leds_Toggle(RunningLed);
		blinkCounter = 0;
		timesCounter++;
	}
	blinkCounter++;
	return retval;
}

static void GPULed_BlinkAnim(void)
{
	const uint16_t blinkPeriodMS = 25;
	static uint16_t blinkCounter = 0;

	if(blinkPeriodMS == blinkCounter)
	{
		/* Time to toggle */
		Leds_Toggle(GPULed);
		blinkCounter = 0;
	}
	else
	{
		/* Do Nothing */
	}
	blinkCounter++;
}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */

/**
 * @brief
 * @param
 * @retval
 */
static void ORLed_SetColor(ORLed_Colors color)
{
	switch(color)
	{
		case White:
		case Red:
			HAL_GPIO_WritePin(LED_OR_GPIO_Port, LED_OR_Pin, color);
		break;
		case Toggle:
			HAL_GPIO_TogglePin(LED_OR_GPIO_Port, LED_OR_Pin);
		break;
		default:
			/* Do Nothing */
		break;
	}
}
