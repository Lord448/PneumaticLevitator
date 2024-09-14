/**
 * @file      PID.c
 * @author    David Ramirez (dardito23@gmail.com)
 *						Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component implemented the internal
 *            PID logic for the control mode AutoPID
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

extern osMessageQueueId_t xFIFO_PIDDistanceHandle;
extern osMessageQueueId_t xFIFO_PIDConfigsHandle;
extern osMessageQueueId_t xFIFO_FANDutyCycleHandle;
extern osMessageQueueId_t xFIFO_PIDSetPointHandle;

static struct PID {
	PIDGains Gains;
	PIDControl Control;
	PIDLimits Limits;
	float ControlAction;
	float Error;
	int32_t Past_Error;
	int32_t Set_Point;
	int32_t Offset;
	uint32_t SamplingRate;
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
	.SamplingRate = 1,
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
/**
* @brief Function implementing the TaskPID thread.
* @param argument: Not used
* @retval None
*/
void vTaskPID(void *argument)
{
	uint32_t distance = 0;
	PIDConfigs FIFOConfigs;
	TickType_t ticks = 0;
	float restActControl = 100 - PID.Offset;
	int8_t dataToSend = 0;
	int16_t fifoSetPoint = 0;
	float distancePercentage = 0;
	float setPointPercetage = 0;
	osStatus_t DistStatus;

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
#ifdef ENABLE_EEPROM
  if(OK != NVM_Read(KP_PID_BASE_ADDR, &PID.Gains.Kp))
#endif
  {
  	/* Could not read the value */
  	PID.Gains.Kp = KP_DEFAULT;
  }
#ifdef ENABLE_EEPROM
  if(OK != NVM_Read(KI_PID_BASE_ADDR, &PID.Gains.Ki))
#endif
  {
  	/* Could not read the value */
  	PID.Gains.Ki = KI_DEFAULT;
  }
#ifdef ENABLE_EEPROM
  if(OK != NVM_Read(KD_PID_BASE_ADDR, &PID.Gains.Kd))
#endif
  {
  	/* Could not read the value */
  	PID.Gains.Kd = KD_DEFAULT;
  }
	ticks = osKernelGetTickCount();

	for (;;)
	{
		/* CHECK IF THERE ARE NEW CONFIGURATIONS */
		{
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
		}
		/* EXECUTE PID ALGORITHM */
		{
			/* Get Distance */
			DistStatus = osMessageQueueGet(xFIFO_PIDDistanceHandle, &distance, NULL, osNoTimeout);
			/* TODO: New sync logic not tested, be careful with task starving */
			if(osOK == DistStatus)
			{
				/* Can execute the control algorithm */
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
			}
			else
			{
				/* There's no distance to process, Do Nothing */
			}
		}

		/* PID OFF/ON LOGIC */
		{
			if(!PID.isActive)
			{
				/* Disabling PID */
				PID_Reset();
				dataToSend = 0;
				osMessageQueuePut(xFIFO_FANDutyCycleHandle, (int8_t*)&dataToSend, 0U, osNoTimeout);
				while(!PID.isActive)
				{
					/* Wait 50 ticks to check if the PID has turned on */
					osDelay(50);
				}
				continue; /* Skip the end of loop delay */
			}
			else
			{
				/* Do Nothing */
			}
		}

		/* END OF LOOP POLLING */
		{
			if(0 < PID.SamplingRate)
			{
				/* There's a fixed sample rate */
				ticks += PID.SamplingRate;
				osDelayUntil(ticks);
			}
			else
			{
				/* Free sampling rate, Do nothing*/
			}
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
 * @note   This function it's meant to be
 *         accessed from other software
 *         components
 * @param  none
 * @retval none
 */
void PID_TogglePID(void)
{
	PID.isActive = !PID.isActive;
	PID_Reset();
}

/**
 * @brief  Turns on the PID
 *         components
 * @param  none
 * @retval none
 */
void PID_TurnOn(void)
{
	PID.isActive = true;
	PID_Reset();
}

/**
 * @brief  Turns off the PID
 *         components
 * @param  none
 * @retval none
 */
void PID_TurnOff(void)
{
	PID.isActive = false;
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
 * @brief  Delivers the state of the PID
 * @param  none
 * @retval The state of the PID
 */
bool PID_isActive(void)
{
	return PID.isActive;
}


/**
 * @brief  Get the value of the local gains
 *         and deliver on the pointers
 * @param  Kp out constant value
 * @param  Ki out constant value
 * @param  Kd out constant value
 * @retval none
 */
void PID_GetControlGains(float *kp, float *ki, float *kd)
{
	*kp = PID.Gains.Kp;
	*ki = PID.Gains.Ki;
	*kd = PID.Gains.Kd;
}

/**
 * @brief  Set the value of the local gains
 * @param  Kp out constant value
 * @param  Ki out constant value
 * @param  Kd out constant value
 * @retval none
 */
void PID_SetControlGains(float kp, float ki, float kd)
{
	PID.Gains.Kp = kp;
	PID.Gains.Ki = ki;
	PID.Gains.Kd = kd;
	NVM_Save(KP_PID_BASE_ADDR, kp);
	NVM_Save(KI_PID_BASE_ADDR, ki);
	NVM_Save(KD_PID_BASE_ADDR, kd);
	PID_Reset();
}
/**
 * @brief  Set the PID offset
 * @param  offset : The calibrated PID offset
 * @retval none
 */
void PID_SetOffset(int32_t offset)
{
	PID.Offset = offset;
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
 * @note TODO: Implement in future versions
 *             Round from 0.5 to upper
 * @param
 * @retval
 */
//static int8_t sPID_Round(float ControlAction)
//{

//}
