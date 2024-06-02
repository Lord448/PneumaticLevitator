/*
 * DistanceSensor.h
 *
 *  Created on: May 29, 2024
 *      Author: lord448
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

#define VL53L0X_ADDR 0x52

void DistanceSensor_Init(void);
void DistanceSensor_MainRunnable(void);

#endif /* SWC_DISTANCESENSOR_DISTANCESENSOR_H_ */
