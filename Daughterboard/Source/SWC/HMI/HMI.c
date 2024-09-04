/**
 * @file      HMI.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This component processes the press of the buttons
 *            as the enables and the disables and FIFO logic or
 *            event logic, when FIFO logic puts a button whenever
 *            it's pressed, otherwise if the user wants to detect
 *            by event flags and no use FIFO style, it can call
 *            the API that disables the FIFO feature and start
 *            using the mode of event flags.
 *            Also This component process the encoder and
 *            if the feature is enabled it send his move
 *            direction to a Queue
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

extern UART_HandleTypeDef huart1;

extern TIM_HandleTypeDef htim1; /* Timer used for the encoder decodification */

extern osMessageQueueId_t xFIFO_ButtonsHandle;
extern osMessageQueueId_t xFIFO_EncoderDataHandle;

extern osEventFlagsId_t xEvent_ButtonsFIFOEnabledHandle;
extern osEventFlagsId_t xEvent_ButtonsEnabledHandle;
extern osEventFlagsId_t XEvent_ButtonPressedHandle;

/**
 * @brief Enum used for the state machine on the debounce logic
 */
typedef enum Stages
{
	WaitingHigh,
	WaitingLow
}Stages;

/**
 * @brief Struct used on the whole debounce scheme & logic
 */
typedef struct Button
{
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	uint16_t Highs;
	uint16_t Lows;
	Buttons ID;
	Stages Stages;
}Button;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void makeDebounce(Button *Button, uint32_t flags);
static void Button_Init(Button *Button, Buttons ID, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
static ButtonStates Button_Debounce(Button *Button);
/**
 * ---------------------------------------------------------
 * 					  SOFTWARE COMPONENT MAIN THREAD
 * ---------------------------------------------------------
 */
/**
* @brief Function implementing the TaskHMI thread.
* @param argument: Not used
* @retval None
*/
void vTaskHMI(void *argument)
{
	Button BtnOK, BtnUp, BtnDown, BtnLeft, BtnRight, BtnMenu, BtnEncoderSW;
	uint32_t BtnEnabledFlags = 0;
	uint32_t EncoderCounts, EncoderPastCounts;
	EncoderDir enconderDir;
	TickType_t ticks;

	/* Building the buttons */
	Button_Init(&BtnOK, iOk, BtnOK_GPIO_Port, BtnOK_Pin);
	Button_Init(&BtnUp, iUp, BtnUP_GPIO_Port, BtnUP_Pin);
	Button_Init(&BtnDown, iDown, BtnDown_GPIO_Port, BtnDown_Pin);
	Button_Init(&BtnLeft, iLeft, BtnLeft_GPIO_Port, BtnLeft_Pin);
	Button_Init(&BtnRight, iRight, BtnRight_GPIO_Port, BtnRight_Pin);
	Button_Init(&BtnMenu, iMenu, BtnMenu_GPIO_Port, BtnMenu_Pin);
	Button_Init(&BtnEncoderSW, iEncoderSW, EncoderSW_GPIO_Port, EncoderSW_Pin);

	/* Waiting for UI to init */

	/* TODO: Debug code */
	HMI_EnableAllButtons();
	HMI_FIFOEnable();
	/* TODO: Debug code */
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	EncoderCounts = EncoderPastCounts = EncoderVal;
	ticks = osKernelGetTickCount();
	for(;;)
	{
		/* Debouncing the buttons */
		BtnEnabledFlags = osEventFlagsGet(xEvent_ButtonsEnabledHandle);
		makeDebounce(&BtnOK, BtnEnabledFlags);
		makeDebounce(&BtnUp, BtnEnabledFlags);
		makeDebounce(&BtnDown, BtnEnabledFlags);
		makeDebounce(&BtnLeft, BtnEnabledFlags);
		makeDebounce(&BtnRight, BtnEnabledFlags);
		makeDebounce(&BtnMenu, BtnEnabledFlags);
		makeDebounce(&BtnEncoderSW, BtnEnabledFlags);

		/* Encoder logic */
		if(BtnEnabledFlags&ENCODER_ENABLED_FLAG)
		{
			/* Encoder is enabled */
			EncoderGetVal(EncoderCounts);
			if(EncoderCounts > EncoderPastCounts)
			{
				/* The Encoder it's going plus */
				enconderDir = Plus;
				osMessageQueuePut(xFIFO_EncoderDataHandle, &enconderDir, 0U, osNoTimeout);
			}
			else if(EncoderCounts < EncoderPastCounts)
			{
				/* The Encoder it's going minus */
				enconderDir = Minus;
				osMessageQueuePut(xFIFO_EncoderDataHandle, &enconderDir, 0U, osNoTimeout);
			}
			else /* if(EncoderCounts == EncoderPastCounts) */
			{
				/* Do Nothing */
			}
			EncoderPastCounts = EncoderCounts;
		}
		else
		{
			/* Do Nothing */
		}

		ticks += TASK_RESOLUTION;
		osDelayUntil(ticks);
	}
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  Check if a button has been pressed
 *         on no FIFO mode
 * @param  Button : Button that will be checked
 * @retval States of the button
 */
ButtonStates HMI_CheckButtonPress(Buttons button)
{
	ButtonStates retval = Idle;
	uint32_t flags = 0;
	flags = osEventFlagsGet(XEvent_ButtonPressedHandle);
	if(flags&button)
	{
		/* A press has been detected from HMI Task */
		osEventFlagsClear(XEvent_ButtonPressedHandle, button);
		retval = Pressed;
	}
	else
	{
		/* No press detected */
		retval = Idle;
	}
	return retval;
}

/**
 * @brief  This function enables the buttons selected
 *  			 which means that the button will be
 *  			 debounced on the HMI task
 * @param  Buttons that will be enabled
 * @retval result of the operation
 */
result_t HMI_EnableButtons(uint32_t flags)
{
	result_t retval = OK;
	uint32_t result = 0;

	result = osEventFlagsSet(xEvent_ButtonsEnabledHandle, flags);

	if(result != osFlagsErrorUnknown ||
		 result != osFlagsErrorParameter ||
		 result != osFlagsErrorResource)
	{
		/* All ok */
		retval = OK;
	}
	else
	{
		/* An error occurred */
		retval = Error;
	}
	return retval;
}

/**
 * @brief  This function disables the buttons selected
 *         which means that the button won't be
 *         debounced on the HMI task
 * @param  Buttons that will be disabled
 * @retval result of the operation
 */
result_t HMI_DisableButtons(uint32_t flags)
{
	result_t retval = OK;
	uint32_t result = 0;

	result = osEventFlagsClear(xEvent_ButtonsEnabledHandle, flags);

	if(result != osFlagsErrorUnknown ||
		 result != osFlagsErrorParameter ||
		 result != osFlagsErrorResource)
	{
		/* All ok */
		retval = OK;
	}
	else
	{
		/* An error occurred */
		retval = Error;
	}
	return retval;
}

/**
 * @brief  Enables the fifo style button process
 *         for the other task button reception
 * @param  none
 * @retval result of the operation
 */
result_t HMI_FIFOEnable(void)
{
	result_t result;
	result = osEventFlagsSet(xEvent_ButtonsFIFOEnabledHandle, FIFO_ENABLED_FLAG);
	if(result == OK)
	{
		/* All OK, reset the FIFO */
		result = osMessageQueueReset(xFIFO_ButtonsHandle);
	}
	else
	{
		/* Do Nothing */
	}
	return result;
}

/**
 * @brief  Disbles the FIFO data transmission for the
 *         buttons press and starts communicate the button
 *         press by event flags
 * @param  none
 * @retval result of the operation
 */
result_t HMI_FIFODisable(void)
{
	result_t result;
	result = osEventFlagsClear(xEvent_ButtonsFIFOEnabledHandle, FIFO_ENABLED_FLAG);
	if(OK == result)
	{
		/* All OK, reset the FIFO */
		result = osMessageQueueReset(xFIFO_ButtonsHandle);
	}
	else
	{
		/* Do nothing */
	}
	return result;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  This function executes the debounce for each
 *         button selected as a parameter and communicates
 *         to the system by the type selected with event flags
 *         can be FIFO or EventFlags
 * @param  *Button : Button structure
 * @param  flags : Enabled buttons received by the task
 * @retval none
 */
static void makeDebounce(Button *Button, uint32_t flags)
{
	uint32_t FIFOEnabledFlags = 0;
	Buttons buttonsFlag = Button->ID;
	if(flags&Button->ID)
	{
		/* The button it's enabled */
		if(Pressed == Button_Debounce(Button))
		{
			/* The button has been pressed */
			FIFOEnabledFlags = osEventFlagsGet(xEvent_ButtonsFIFOEnabledHandle);
			if(FIFOEnabledFlags&FIFO_ENABLED_FLAG)
			{
				/* FIFO style enabled */
				osMessageQueuePut(xFIFO_ButtonsHandle, &buttonsFlag, 0U, osNoTimeout);
			}
			else
			{
				/* FIFO Style disabled */
				osEventFlagsSet(XEvent_ButtonPressedHandle, buttonsFlag);
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		/* The button it's disabled */
	}
}

/**
 * @brief  This function builds the structure for each
 *         button created
 * @note   This function it's thinked on the use of
 *         pull up resistors
 * @param  *GPIOx : Port of the button
 * @param  GPIO_Pin : Pin of the button
 * @retval none
 */
static void Button_Init(Button *Button, Buttons ID, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	Button -> GPIOx = GPIOx;
	Button -> GPIO_Pin = GPIO_Pin;
	Button -> ID = ID;
	Button -> Highs = 0;
	Button -> Lows = 0;
	Button -> Stages = WaitingHigh;
}

/**
 * @brief  This function it's meant to debounce a button using
 *         state machines, so for it's correct implementation
 *         and usage it's needed a parsed loop style
 * @note   This function it's thinked on the use of
 *         pull up resistors
 * @param  *Button Pointer to the button
 * @retval State of the button that is debounced
 */
static ButtonStates Button_Debounce(Button *Button)
{
	ButtonStates retval = Idle;
	if(0 == HAL_GPIO_ReadPin(Button->GPIOx, Button->GPIO_Pin))
	{
		/* The pin is low */
		Button -> Highs = 0;
		Button -> Lows = Button -> Lows + 1;
	}
	else
	{
		/* The pin is in high */
		Button -> Highs = Button -> Highs + 1;
		Button -> Lows = 0;
	}

	switch(Button->Stages)
	{
		case WaitingHigh:
			/* The pin it's currently on Low state and waiting high state */
			if(Button -> Highs >= DEBOUNCE_SAMPLES)
			{
				/* Rising edge detected, Bouncing has stopped */
				Button -> Stages = WaitingLow;
			}
			else
			{
				/* Do Nothing */
			}
			/* As we are using pull up's were not searching for a rising edge */
			retval = Idle;
		break;
		case WaitingLow:
			/* The pin it's currently on high state and waiting a low state */
			if(Button -> Lows >= DEBOUNCE_SAMPLES)
			{
				/* Falling edge detected, Bouncing has stopped */
				Button -> Stages = WaitingHigh;
				retval = Pressed;
			}
			else
			{
				/* Waiting for a stable state */
				retval = Idle;
			}
		break;
		default:
			/* The code doesn't to reach this part */
			retval = Idle;
		break;
	}

	return retval;
}
/**
 * ---------------------------------------------------------
 * 					       ISR COMPONENT CALLBACKS
 * ---------------------------------------------------------
 */
/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t data = RESET;
	if(Reset_IT_Pin == GPIO_Pin)
	{
		/* Communicate the Motherboard the reset request */
		HAL_UART_Transmit(&huart1, &data, 1, 1000);
		HAL_NVIC_SystemReset();
	}
}
