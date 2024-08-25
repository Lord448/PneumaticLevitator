/**
 * @file      FAN.h
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

#ifndef SWC_FAN_FAN_H_
#define SWC_FAN_FAN_H_

#include "main.h"
#include "cmsis_os2.h"

//#define DEBUG_IC /* If defined set the frequency variables on global to see in the "live expressions" */
//#define CALCULATE_RPM_IN_HZ

#define CPU_CLOCK 96000000 / 100

#define setPWM_FAN(x) TIM3->CCR1 = ((x*htim3.Init.Period)/100) /* Set the duty cycle of the PWM in percentage */
#define setPWM_FAN_Counts(x) TIM3->CCR1 = x /* Set the duty cycle of the PWM in counts (cannot be more than "MaxCCR")*/

#define EnableFAN(_) HAL_GPIO_WritePin(EnableFAN_GPIO_Port, EnableFAN_Pin, _) /* Enables or disables the AND gate for the FAN control */

void vTaskFAN(void *argument);

#endif /* SWC_FAN_FAN_H_ */
