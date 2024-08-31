/**
 * @file      PID.c
 * @author    David Ramirez (dardito23@gmail.com)
 *						Pedro Rojo (pedroeroca@outlook.com)
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

#include "PID.h"

extern osThreadId_t TaskPIDHandle;

extern osMessageQueueId_t xFIFO_PIDDistanceHandle;
extern osMessageQueueId_t xFIFO_PIDConfigsHandle;
extern osMessageQueueId_t xFIFO_FANDutyCycleHandle;
extern osMessageQueueId_t xFIFO_PIDSetPointHandle;

extern osSemaphoreId_t xSemaphore_PID_InitHandle;

static struct PID {
	PIDGains Gains;
	PIDControl Control;
	PIDLimits Limits;
	float ControlAction;
	float Error;
	int32_t Past_Error;
	int32_t Set_Point;
	int32_t Offset;
	bool isActive;
} PID = {
	.Gains.Kp = KP_DEFAULT,
	.Gains.Ki = KI_DEFAULT,
	.Gains.Kd = KD_DEFAULT,
	.Limits.P = NO_LIMIT,
	.Limits.I = 20,
	.Limits.D = NO_LIMIT,
	.Limits.Control = NO_LIMIT,
	.Error = 0,
	.Past_Error = 0,
	.Set_Point = 260,
	.Offset = 65, /* Need calibration and configuration */
	.isActive = true
};

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sPID_SetConfigs(PIDConfigs *configs, float *restActControl);
/**
 * ---------------------------------------------------------
 * 					  SOFTWARE COMPONENT MAIN THREAD
 * ---------------------------------------------------------
 */
float distancePercentage = 0;
float setPointPercetage = 0;
void vTaskPID(void *argument)
{
	uint32_t distance = 0;
	//uint32_t controlSamplingRate = HZToMS(DEFAULT_CONTROL_SAMPLING_RATE);
	uint32_t controlSamplingRate = 1; /* TODO: Adjust no freq limits, using preemption */
	PIDConfigs FIFOConfigs;
	TickType_t ticks = 0;
	float restActControl = 100 - PID.Offset;
	int8_t dataToSend = 0;
	int16_t fifoSetPoint = 0;

	/* Reading constants from NVM */
	/* TODO: Check here possible concurrency on EEPROM component with
	 *
	 * Posible avoid of concurrency in the NVM component with a new
	 * thread or a semaphore with timeout
	 *
	 * If semaphore used, the overload function will need a _Generic macro
	 * with variadic arguments
	 *
	 * At the moment it won't happen anything since the functions are
	 * not implemented yet
	 */
  //if(OK != NVM_Read(KP_PID_BASE_ADDR, &PID.Gains.Kp))
  {
  	/* Could not read the value */
  	PID.Gains.Kp = KP_DEFAULT;
  }
  //if(OK != NVM_Read(KI_PID_BASE_ADDR, &PID.Gains.Ki))
  {
  	/* Could not read the value */
  	PID.Gains.Ki = KI_DEFAULT;
  }
  //if(OK != NVM_Read(KD_PID_BASE_ADDR, &PID.Gains.Kd))
  {
  	/* Could not read the value */
  	PID.Gains.Kd = KD_DEFAULT;
  }
	ticks = osKernelGetTickCount();

	for (;;)
	{
		/* Check if there are some new configurations */
		if (osMessageQueueGet(xFIFO_PIDConfigsHandle, &FIFOConfigs, NULL, osNoTimeout) == osOK) {
				/* There are configurations to set */
				sPID_SetConfigs(&FIFOConfigs, &restActControl);
		}
		if(osMessageQueueGet(xFIFO_PIDSetPointHandle, &fifoSetPoint, NULL, osNoTimeout) == osOK) {
			/* New set Point */
			if(fifoSetPoint < MAX_DISTANCE && fifoSetPoint > 0) {
				/* Inside the bounds */
				PID.Set_Point = fifoSetPoint;
			}
			else
			{
				/* Out of bounds */
				if(fifoSetPoint > MAX_DISTANCE) {
					/* Higher value than permited */
					PID.Set_Point = MAX_DISTANCE;
				}
				else {
					/* Negative value, Do nothing */
				}
			}
		}
		/* Get Distance */
		osMessageQueueGet(xFIFO_PIDDistanceHandle, &distance, NULL, osNoTimeout);
		distancePercentage = (distance * 100) / MAX_DISTANCE; /* Normalizing the distance */
		setPointPercetage = (PID.Set_Point * 100) / MAX_DISTANCE;
		PID.Error = setPointPercetage - distancePercentage;
		PID.Control.P = (float)PID.Error * PID.Gains.Kp;
		PID.Control.I += (float)PID.Error * PID.Gains.Ki;
		PID.Control.D = (float)(PID.Error - PID.Past_Error) * PID.Gains.Kd;
		/* Constraints check on the PID algorithm (recommended only for I) */
		if (NO_LIMIT != PID.Limits.P && PID.Control.P > PID.Limits.P) {
			/* There are constraints on the P control action */
			PID.Control.P = PID.Limits.P;
		}
		if (NO_LIMIT != PID.Limits.I ) {
			/* There are constraints on the I control action */
			if(PID.Control.I > PID.Limits.I)
				PID.Control.I = PID.Limits.I;
			else if(PID.Control.I < -PID.Limits.I)
				PID.Control.I = -PID.Limits.I;
		}
		if (NO_LIMIT != PID.Limits.D && PID.Control.D > PID.Limits.D) {
			/* There are constraints on the D control action */
			PID.Control.D = PID.Limits.D;
		}

		/* Calculating control action */
		PID.ControlAction = PID.Offset + PID.Control.P + PID.Control.I + PID.Control.D; /* Expected value on percetage */
		/* Constraints for the control action */
		if (PID.ControlAction < 0) {
			PID.ControlAction = 0;
		}
		PID_LimitActionControl(PID.ControlAction);
		PID.Past_Error = PID.Error;

		dataToSend = (int8_t)PID.ControlAction;
		/* Sending to FAN processed data */
		/* TODO: Traduce the control action to porcentage */
		osMessageQueuePut(xFIFO_FANDutyCycleHandle, (int8_t*)&dataToSend, 0U, osNoTimeout);
		/* Off/On logic */
		if(!PID.isActive)
		{
			/* Disabling PID */
			PID_Reset();
			dataToSend = 0;

			while(!PID.isActive)
			{
				osMessageQueuePut(xFIFO_FANDutyCycleHandle, (int8_t*)&dataToSend, 0U, osNoTimeout);
				/* Wait 50 ticks to check if the PID has turned on */
				osDelay(50);
			}
			continue; /* Skip the end of loop delay */
		}
		else
		{
			/* Do Nothing */
		}
		/* End of loop */
		if(0 < controlSamplingRate)
		{
			/* There's a fixed sample rate */
			ticks += controlSamplingRate;
			osDelayUntil(ticks);
		}
		else
		{
			/* Free sampling rate, Do nothing*/
		}
	}
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  Turns off or on the PID
 * @param  none
 * @retval none
 */
void PID_TogglePID(void)
{
	PID.isActive = !PID.isActive;
}

/**
 * @brief  Reset all the operative values
 *         of the PID structure like the action
 *         control
 * @param  none
 * @retval none
 */
void PID_Reset(void)
{
	PID.Error = 0;
	PID.Past_Error = 0;
	PID.Control.P = 0;
	PID.Control.I = 0;
	PID.Control.D = 0;
	PID.ControlAction = 0;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */

/**
 * @brief
 * @param
 * @retval
 */
static void sPID_SetConfigs(PIDConfigs *configs, float *restActControl)
{
	/*TODO: Change the configs of the global PID structure based on the pointer*/
	/* TODO: Add the sampling rate as a new config */
	if (configs) {
		PID.Gains.Kp 	= configs->Gains.Kp;
		PID.Gains.Ki 	= configs->Gains.Ki;
		PID.Gains.Kd 	= configs->Gains.Kd;
		PID.Limits.P 	= configs->Limits.P;
		PID.Limits.I 	= configs->Limits.I;
		PID.Limits.D 	= configs->Limits.D;
		PID.Limits.Control = configs->Limits.Control;
		//EEPROM_Write(&PID);
		*restActControl = 100 - PID.Offset;
	}
}

/**
 * @brief
 * @param
 * @retval
 */
static int8_t sPID_Round(float ControlAction)
{

}
