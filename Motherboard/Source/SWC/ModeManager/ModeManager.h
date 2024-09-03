/**
 * @file      ModeManager.h
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

#ifndef SWC_MODEMANAGER_MODEMANAGER_H_
#define SWC_MODEMANAGER_MODEMANAGER_H_

#include "main.h"
#include "cmsis_os.h"

#define AUTO_PID_FLAG 1U << 0
#define SLAVE_FLAG    1U << 1
#define MANUAL_FLAG   1U << 2
#define DEV_MODE_FLAG 1U << 3

#define ALL_CONTROL_MODE_FLAGS AUTO_PID_FLAG | SLAVE_FLAG | MANUAL_FLAG | DEV_MODE_FLAG

#define THREAD_FLAG_DISABLE_PID 1U << 0
#define THREAD_FLAG_ENABLE_PID  1U << 1

typedef enum ControlModes{
	Manual,
	AutoPID,
	Slave
}ControlModes;

void vTaskModeManager(void *argument);

#endif /* SWC_MODEMANAGER_MODEMANAGER_H_ */
