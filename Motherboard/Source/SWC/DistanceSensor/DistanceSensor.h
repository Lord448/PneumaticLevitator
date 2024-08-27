/**
 * @file      DistanceSensor.h
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

#ifndef SWC_DISTANCESENSOR_DISTANCESENSOR_H_
#define SWC_DISTANCESENSOR_DISTANCESENSOR_H_

#include "main.h"
#include "COM/COM.h"
#include "COM/Signals.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include <stdio.h>
#include <string.h>

#define MAX_DISTANCE 530

#define VL53L0X_ADDR 0x52

void vTaskSensor(void *argument);

#endif /* SWC_DISTANCESENSOR_DISTANCESENSOR_H_ */
