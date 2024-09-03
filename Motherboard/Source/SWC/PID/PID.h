/**
 * @file      PID.h
 * @author    David Ramirez (dardito23@gmail.com)
 *
 * @brief     TODO
 *
 * @date      May 29, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 * 			  the Mozilla Public License, v. 2.0. If a copy of
 * 		 	  the MPL was not distributed with this file, You
 * 			  can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#ifndef SWC_PID_PID_H_
#define SWC_PID_PID_H_

#define NO_LIMIT 0
#define EEPROM_OK 0

#include "main.h"
#include "DistanceSensor.h"
#include "cmsis_os2.h"
#include "COM.h"
#include "NVM.h"

#define HZToMS(x) (1/x)*1000
#define FtoT(x) HZToMS(x)

#define PID_LimitActionControl(x) x = x > 100 ? 100 : x

#define DEFAULT_CONTROL_SAMPLING_RATE 1000 /* HZ */

/*TODO: Calculate gains*/
#define KP_DEFAULT (float) 0.32
#define KI_DEFAULT (float) 0.000015
#define KD_DEFAULT (float) 0.02

typedef struct PIDLimits {
	int32_t P;
	int32_t I;
	int32_t D;
	float Control;
}PIDLimits;

typedef struct PIDGains{
	float Kp;
	float Ki;
	float Kd;
}PIDGains;

typedef struct PIDControl{
	float P;
	float I;
	float D;
}PIDControl;

typedef struct PIDConfigs{
	PIDGains Gains;
	PIDLimits Limits;
	uint8_t Is_Active;
}PIDConfigs;

void PID_TogglePID(void);
void PID_Reset(void);
void PID_GetControlGains(float *kp, float *ki, float *kd);
void PID_SetControlGains(float kp, float ki, float kd);

void vTaskPID(void *argument);

#endif /* SWC_PID_PID_H_ */
