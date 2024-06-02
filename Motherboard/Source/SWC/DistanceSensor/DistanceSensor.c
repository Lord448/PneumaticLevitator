/*
 * DistanceSensor.c
 *
 *  Created on: May 29, 2024
 *      Author: lord448
 */

#include "DistanceSensor.h"

extern I2C_HandleTypeDef hi2c1; /*Connection to the VL53L0X*/

static VL53L0X_Dev_t  vl53l0x_c; /*Center module*/
static VL53L0X_DEV    Dev = &vl53l0x_c;
static VL53L0X_RangingMeasurementData_t RangingData;

extern osMessageQueueId_t xFIFO_COMHandle;
extern osMessageQueueId_t xFIFO_DistanceHandle;

void DistanceSensor_Init(void)
{
	uint32_t refSpadCount;
	uint8_t isApertureSpads;
	uint8_t VhvSettings;
	uint8_t PhaseCal;

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
}

void DistanceSensor_MainRunnable(void)
{
	const uint32_t MeasureTolerance = 5;
	const uint32_t Reference = 520;
	static uint16_t distance, past_measure;
	PDU_t distance_PDU;

	VL53L0X_PerformSingleRangingMeasurement(Dev, &RangingData);
	if(RangingData.RangeStatus == 0)
  {
		if(RangingData.RangeMilliMeter > past_measure+MeasureTolerance || RangingData.RangeMilliMeter < past_measure-MeasureTolerance)
		{
			distance = Reference - RangingData.RangeMilliMeter;
			distance_PDU.rawData = distance;
			distance_PDU.chunks[4] = 0; /*TODO: Add the message ID*/
			osMessageQueuePut(xFIFO_DistanceHandle, &distance, 0U, 0U); /*Sending to PID*/
			osMessageQueuePut(xFIFO_COMHandle, &distance_PDU, 0U, 0U); /*Sending to COM*/
		}
	}
	past_measure = RangingData.RangeMilliMeter;
}

/*TODO (Ignore if sensor has internal filters)*/
/*Make signal noise analysis and if needed*/
/*Implement a FIR filter*/
/*Implement an averager filter*/
