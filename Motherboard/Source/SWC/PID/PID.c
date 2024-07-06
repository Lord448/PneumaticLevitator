/**
 * @file      PID.c
 * @author    TODO
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

static const uint32_t NO_LIMIT = 0;

extern osMessageQueueId_t xFIFO_ControlActionHandle;
extern osMessageQueueId_t xFIFO_DistanceHandle;
extern osMessageQueueId_t xFIFO_PIDConfigsHandle;
extern osMessageQueueId_t xFIFO_COMHandle;

extern osSemaphoreId_t xSemaphore_PIDHandle;
extern osSemaphoreId_t xSemaphore_PID_InitHandle;

static struct PID{
	PIDGains Gains;
	PIDControl Control;
	PIDLimits Limits;
	float ControlAction;
	int32_t Error;
	int32_t Past_Error;
	int32_t Set_Point;
}PID = {
		.Gains.Kp = KP_DEFAULT,
		.Gains.Ki = KI_DEFAULT,
		.Gains.Kd = KD_DEFAULT,
		.Limits.P = NO_LIMIT,
		.Limits.I = NO_LIMIT,
		.Limits.D = NO_LIMIT,
		.Error = 0,
		.Past_Error = 0,
		.Set_Point = 0
};

static void PID_SetConfigs(PIDConfigs *configs)
{
  /*TODO: Change the configs of the global PID structure based on the pointer*/
}

void PID_Init(void)
{
	/* Wait for the confirmation of the memory lecture */
	if(osSemaphoreAcquire(xSemaphore_PID_InitHandle, osWaitForever) == osOK) {
		/*Fill the local PID structure with the values of the EEPROM memory*/
	}
	else {
		/*Fill the structure with the defaults values*/
	}


}

void vTaskPID(void *argument)
{
	uint32_t distance = 0;
	PIDConfigs FIFOConfigs;
	PDU_t ControlAction_PDU;

	for(;;)
	{
		/*Check if there are some new configurations*/
		if(osMessageQueueGet(xFIFO_PIDConfigsHandle, &FIFOConfigs, NULL, 0) == osOK){
			/*There are configurations to set*/
			PID_SetConfigs(&FIFOConfigs);
		}
		/*Get Distance*/
		/*TODO: Possible avoid blocking mode in order to check configs (depends of CPU load)*/
		(void)osMessageQueueGet(xFIFO_DistanceHandle, &distance, NULL, osWaitForever);
		PID.Error = PID.Set_Point - distance;
		PID.Control.P =  (float)PID.Error * PID.Gains.Kp;
		PID.Control.I += (float)PID.Error * PID.Gains.Ki;
		PID.Control.D =  (float)(PID.Error - PID.Past_Error) * PID.Gains.Kd;
		/*Constraints check on the PID algorithm (recommended only for I)*/
		if(NO_LIMIT != PID.Limits.P){
			/*There are constraints on the P control action*/
			if(PID.Control.P > PID.Limits.P) {
				PID.Control.P = PID.Limits.P;
			}
		}
		if(NO_LIMIT != PID.Limits.I){
			/*There are constraints on the I control action*/
			if(PID.Control.I > PID.Limits.I) {
				PID.Control.I = PID.Limits.I;
			}
		}
		if(NO_LIMIT != PID.Limits.D){
			/*There are constraints on the D control action*/
			if(PID.Control.D > PID.Limits.D) {
				PID.Control.D = PID.Limits.D;
			}
		}
		/*Calculating control action*/
		PID.ControlAction = PID.Control.P + PID.Control.I + PID.Control.D;
		/*Constraints for the control action*/
		if(PID.ControlAction < 0)
			PID.ControlAction = ABS(PID.ControlAction);
		if(PID.ControlAction > PID.Limits.Control)
			PID.ControlAction = PID.Limits.Control;
		/*Sending to the actuator*/
		osMessageQueuePut(xFIFO_ControlActionHandle, &PID.ControlAction, 0U, 0U);
		/*Sending to the COM module*/
		ControlAction_PDU = COM_CreatePDU(0, PID.ControlAction);
		osMessageQueuePut(xFIFO_COMHandle, &ControlAction_PDU, 0U, 0U);
		PID.Past_Error = PID.Error;
	}
}
