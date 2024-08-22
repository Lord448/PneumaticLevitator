/**
 * @file      HMI.h
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
#ifndef SWC_HMI_HMI_H_
#define SWC_HMI_HMI_H_

#include "main.h"
#include "cmsis_os.h"

/* Flags for xEvent_ButtonsFIFOEnabledHandle */
#define FIFO_ENABLED_FLAG        (uint32_t) 1U << 0

/* Flags for xEvent_ButtonsEnabledHandle & XEvent_ButtonPressedHandle*/
#define BTN_OK_ENABLED_FLAG          (uint32_t) 1U << 0
#define BTN_UP_ENABLED_FLAG          (uint32_t) 1U << 2
#define BTN_DOWN_ENABLED_FLAG        (uint32_t) 1U << 3
#define BTN_LEFT_ENABLED_FLAG        (uint32_t) 1U << 4
#define BTN_RIGHT_ENABLED_FLAG       (uint32_t) 1U << 5
#define BTN_MENU_ENABLED_FLAG        (uint32_t) 1U << 6
#define BTN_ENCODER_SW_ENABLED_FLAG  (uint32_t) 1U << 7
#define ENCODER_ENABLED_FLAG         (uint32_t) 1U << 8

typedef enum EncoderDir {
	Minus,
	Plus
}EncoderDir;

typedef enum ButtonStates {
	Idle,
	Pressed
}ButtonStates;

typedef enum Buttons {
	Ok = BTN_OK_ENABLED_FLAG,
	Up = BTN_UP_ENABLED_FLAG,
	Down = BTN_DOWN_ENABLED_FLAG,
	Left = BTN_LEFT_ENABLED_FLAG,
	Right = BTN_RIGHT_ENABLED_FLAG,
	Menu = BTN_MENU_ENABLED_FLAG,
	EncoderSW = BTN_ENCODER_SW_ENABLED_FLAG,
	Reset,
}Buttons;

#define TimerMiddleValue 0x7FFF //0xFFFF/2
#define TimerValue TimerMiddleValue>>2 //1FFF
#define EncoderGetVal(x) x = (int32_t)(((TIM1 -> CNT)>>2) - (TimerValue))
#define EncoderVal (int32_t)(((TIM1 -> CNT)>>2) - (TimerValue))

#define HMI_EnableAllButtons() HMI_EnableButtons(Ok | Up | Down | Left | Right | Menu | EncoderSW)
#define HMI_DisableAllButtons() HMI_DisableButtons(Ok | Up | Down | Left | Right | Menu | EncoderSW)

void vTaskHMI(void *argument);
ButtonStates HMI_CheckButtonPress(Buttons button);
result_t HMI_EnableButtons(uint32_t flags);
result_t HMI_DisableButtons(uint32_t flags);
result_t HMI_FIFOEnable(void);
result_t HMI_FIFODisable(void);

#endif /* SWC_HMI_HMI_H_ */
