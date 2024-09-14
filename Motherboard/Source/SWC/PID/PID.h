/**
 * @file      PID.h
 * @author    David Ramirez (dardito23@gmail.com)
 *						Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component implemented the internal
 *            PID logic for the control mode AutoPID
 *
 * @date      May 29, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 * 			  	  the Mozilla Public License, v. 2.0. If a copy of
 * 		 	      the MPL was not distributed with this file, You
 * 			      can obtain one at https://mozilla.org/MPL/2.0/.
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
/**
 * ---------------------------------------------------------
 * 					         PID GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define HZToMS(x) (1/x)*1000                             /* Converts frequency in HZ to period in MS */
#define FtoT(x) HZToMS(x)                                /* Another name for the first macro */

#define PID_LimitActionControl(x) x = x > 100 ? 100 : x  /* Limits the action control to 100 */

#define DEFAULT_CONTROL_SAMPLING_RATE 1000 /* HZ */      /* Default sampling rate of the PID control */

#define KP_DEFAULT (float) 0.32                          /* Default value of the KP constant */
#define KI_DEFAULT (float) 0.000015											 /* Default value of the KI constant */
#define KD_DEFAULT (float) 0.02                          /* Default value of the KD constant */
/**
 * ---------------------------------------------------------
 * 					          PID GENERAL TYPES
 * ---------------------------------------------------------
 */
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
/**
 * ---------------------------------------------------------
 * 					        PID GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void PID_TogglePID(void);
void PID_TurnOn(void);
void PID_TurnOff(void);
void PID_Reset(void);
bool PID_isActive(void);
void PID_GetControlGains(float *kp, float *ki, float *kd);
void PID_SetControlGains(float kp, float ki, float kd);
void PID_SetOffset(int32_t offset);

void vTaskPID(void *argument);

#endif /* SWC_PID_PID_H_ */
