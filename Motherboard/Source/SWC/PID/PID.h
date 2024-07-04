/*
 * PID.h
 *
 *  Created on: May 29, 2024
 *      Author: lord448
 */

#ifndef SWC_PID_PID_H_
#define SWC_PID_PID_H_

#include "main.h"
#include "cmsis_os2.h"
#include "COM/COM.h"
#include "COM/Signals.h"

/*TODO: Calculate gains*/
#define KP_DEFAULT (float) 0
#define KI_DEFAULT (float) 0
#define KD_DEFAULT (float) 0

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
}PIDConfigs;

void PID_Init(void);

void vTaskPID(void *argument);

#endif /* SWC_PID_PID_H_ */
