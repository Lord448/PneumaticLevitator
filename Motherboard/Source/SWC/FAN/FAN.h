/**
 * @file      FAN.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This component handles all the timers that
 *            works with the FAN, the input capture in
 *            non blocking mode (interrupts), the PWM
 *            generator for the FAN control and the data
 *            send to COM as a periodic data
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

#ifndef SWC_FAN_FAN_H_
#define SWC_FAN_FAN_H_

#include "main.h"
#include "cmsis_os2.h"

/* If defined set the frequency variables on global to see in the "live expressions" */
//#define DEBUG_IC
//#define CALCULATE_RPM_IN_HZ

/* If defined the task will enter on a loop for the test of the components */
//#define HARD_CODED_TEST

#define CPU_CLOCK 96000000 / 100 /* Divided by 100 in order to efficient more the IC ISR */

#define setPWM_FAN(x)        TIM3->CCR1 = x < 100 ? ((x*htim3.Init.Period)/100) : htim3.Init.Period /* Set the duty cycle of the PWM in percentage */
#define setPWM_FAN_Counts(x) TIM3->CCR1 = x < htim3.Init.Period ? x : htim3.Init.Period             /* Set the duty cycle of the PWM in counts (cannot be more than "htim3.Init.Period")*/

#define EnableFAN(_)         HAL_GPIO_WritePin(EnableFAN_GPIO_Port, EnableFAN_Pin, _)               /* Enables or disables the AND gate for the FAN control */

void vTaskFAN(void *argument);

#endif /* SWC_FAN_FAN_H_ */
