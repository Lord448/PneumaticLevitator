/**
 * @file      HMI.h
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
#ifndef SWC_HMI_HMI_H_
#define SWC_HMI_HMI_H_

#include "main.h"
#include "cmsis_os.h"

#define FIFO_ENABLED_FLAG 1

typedef enum Buttons {
	Ok,
	Up,
	Down,
	Left,
	Right,
	Menu,
	Reset,
}Buttons;

void vTaskHMI(void *argument);
result_t HMI_FIFOEnable(void);
result_t HMI_FIFODisable(void);

#endif /* SWC_HMI_HMI_H_ */
