/**
 * @file      DistanceSensor.c
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

#include "DistanceSensor.h"

extern I2C_HandleTypeDef hi2c1; /*Connection to the VL53L0X*/

static VL53L0X_Dev_t  vl53l0x_c; /*Center module*/
static VL53L0X_DEV    Dev = &vl53l0x_c;
static VL53L0X_RangingMeasurementData_t RangingData;

extern osMessageQueueId_t xFIFO_PIDDistanceHandle;
extern osMessageQueueId_t xFIFO_COMDistanceHandle;
extern osMessageQueueId_t xFIFO_USBDistanceHandle;

extern osEventFlagsId_t xEvent_ControlModesHandle;

static uint16_t samplingDelay = NO_SAMPLING_DELAY;
static TickType_t DelayTick = 0;

/**
 * ---------------------------------------------------------
 * 					  SOFTWARE COMPONENT MAIN THREAD
 * ---------------------------------------------------------
 */
void vTaskSensor(void *argument)
{
	uint32_t refSpadCount;
	uint32_t ModeFlags = 0;
	uint8_t isApertureSpads;
	uint8_t VhvSettings;
	uint8_t PhaseCal;
	int16_t distance;

	Dev->I2cHandle = &hi2c1;
	Dev->I2cDevAddr = VL53L0X_ADDR;
	//Disable XSHUT
	HAL_GPIO_WritePin(TOF_XSHUT_GPIO_Port, TOF_XSHUT_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);

	//Enable XSHUT
	HAL_GPIO_WritePin(TOF_XSHUT_GPIO_Port, TOF_XSHUT_Pin, GPIO_PIN_SET);
	HAL_Delay(20);

	VL53L0X_WaitDeviceBooted( Dev );
	VL53L0X_DataInit( Dev );
	VL53L0X_StaticInit( Dev );
	VL53L0X_PerformRefCalibration(Dev, &VhvSettings, &PhaseCal);
	VL53L0X_PerformRefSpadManagement(Dev, &refSpadCount, &isApertureSpads);
	VL53L0X_SetDeviceMode(Dev, VL53L0X_DEVICEMODE_SINGLE_RANGING);

	//Enable/Disable Sigma and Signal check
	VL53L0X_SetLimitCheckEnable(Dev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
	VL53L0X_SetLimitCheckEnable(Dev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
	VL53L0X_SetLimitCheckValue(Dev, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
	VL53L0X_SetLimitCheckValue(Dev, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
	VL53L0X_SetMeasurementTimingBudgetMicroSeconds(Dev, 33000);
	VL53L0X_SetVcselPulsePeriod(Dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
	VL53L0X_SetVcselPulsePeriod(Dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);

	DelayTick = osKernelGetTickCount();
	for(;;)
	{
		VL53L0X_PerformSingleRangingMeasurement(Dev, &RangingData);
		if(RangingData.RangeStatus == 0)
	  {
			/* Got the data correctly */
			distance = RangingData.RangeMilliMeter > MAX_DISTANCE ? 0 : MAX_DISTANCE - RangingData.RangeMilliMeter; /* Setting bounds */
			osMessageQueuePut(xFIFO_COMDistanceHandle, &distance, 0U, osNoTimeout); /* Sending to COM */
			osMessageQueuePut(xFIFO_PIDDistanceHandle, &distance, 0U, osNoTimeout); /* Sending to PID */
			ModeFlags = osEventFlagsGet(xEvent_ControlModesHandle);
			if(ModeFlags&SLAVE_FLAG)
			{
				/* Need to send the distance to USB FIFO */
				osMessageQueuePut(xFIFO_USBDistanceHandle, &distance, 0U, osNoTimeout); /* Sending to USB in COM */
			}
			else
			{
				/* Do Nothing */
			}
		}
		else
		{
			/* Do Nothing */
		}

		if(samplingDelay > 0)
		{
			/* Fixed frequency */
			DelayTick += samplingDelay;
			osDelayUntil(DelayTick);
		}
		else
		{
			/* No sampling freq, Do Nothing */
		}
	}
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void DistanceSensor_SetSamplingFreq(TickType_t ticks)
{
	samplingDelay = pdMS_TO_TICKS(ticks);
	if(ticks > 0)
	{
		/* Restoring the tick count from kernel */
		DelayTick = osKernelGetTickCount();
	}
	else
	{
		/* No sampling freq, Do Nothing */
	}
}

/*TODO (Ignore if sensor has internal filters)*/
/*Make signal noise analysis and if needed*/
/*Implement a FIR filter*/
/*Implement an averager filter*/
