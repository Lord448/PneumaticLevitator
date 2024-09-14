/**
 * @file      COM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component leads the all the
 *            communication ports on the Motherboard, UART
 *            for the communication with the Daughterboard,
 *            USB for the communication with Matlab or any PC
 *            program (Arduino monitor can be used to).
 *
 *            This component implements two subthreads in order to
 *            make a fast process and response for the USB messages
 *            as well as the on demand UART messages, and save CPU
 *            load charge by polling this tasks until there's a
 *            message to process.
 *
 *            This component implements soft-timers with the help
 *            of the kernel predefined objects for the periodic
 *            frame sending
 *
 *            For the usage of the Arduino Monitor, please check
 *            the user manual in the section USB communication.
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

#include "COM.h"

#define UARTfirstFrame UART_RxBuffer[0]

extern UART_HandleTypeDef huart1;

extern osThreadId_t TaskSensorHandle;

extern osMessageQueueId_t xFIFO_COMDistanceHandle;
extern osMessageQueueId_t xFIFO_COMRPMHandle;
extern osMessageQueueId_t xFIFO_COMActionControlHandle;
extern osMessageQueueId_t xFIFO_PIDSetPointHandle;
extern osMessageQueueId_t xFIFO_FANDutyCycleHandle;
extern osMessageQueueId_t xFIFO_USBDistanceHandle;

extern osTimerId_t xTimer_UARTSendHandle;
extern osTimerId_t xTimer_WdgUARTHandle;
extern osTimerId_t xTimer_RestartSensorTaskHandle;

extern osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorRxCpltHandle;
extern osSemaphoreId_t xSemaphore_InitDaughterHandle;
extern osSemaphoreId_t xSemaphore_SensorErrorHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;
extern osSemaphoreId_t xSemaphore_UARTTxCpltHandle;

extern osEventFlagsId_t xEvent_USBHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;
extern osEventFlagsId_t xEvent_ControlModesHandle;

extern char CDC_ResBuffer[64];

uint8_t UART_RxBuffer[COM_UART_PERIODIC_NUMBER_FRAMES_RX] = {0};
bool firstInit = true;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sendInitBuffer(void);
static void syncComm(void);
static void sCOM_SendConstants(float kp, float ki, float kd);
static result_t sUSB_ParseGains(float *kp, float *ki, float *kd);
static result_t sCOM_SendUSB(uint8_t *buf, uint16_t len);
/**
 * ---------------------------------------------------------
 * 					  SOFTWARE COMPONENT MAIN THREAD
 * ---------------------------------------------------------
 */
/**
  * @brief Function implementing the TaskCOM thread.
	* @param argument: Not used
	* @retval None
	*/
void vTaskCOM(void *argument)
{
	uint32_t ErrFlags = 0;
	uint32_t modeFlags = 0;
	int16_t distance = 0;
	uint16_t datalen = 0;
	char distBuff[32] = "";

	syncComm();
	for(;;)
	{
		ErrFlags = osEventFlagsGet(xEvent_FatalErrorHandle);
		if(ErrFlags&FATAL_ERROR_GPU)
		{
			/* Handling the fatal error */
			syncComm();
		}
		/* Send distance to USB logic */
		modeFlags = osEventFlagsGet(xEvent_ControlModesHandle);
		if(modeFlags&SLAVE_FLAG)
		{
			/* Need to send distance */
			if(osOK == osMessageQueueGet(xFIFO_USBDistanceHandle, &distance, NULL, 10))
			{
				/* There is a distance message */
#ifdef COM_SEND_DISTANCE_NORMALIZED
				distance = (distance * 100) / MAX_DISTANCE; /* Normalizing distance */
#endif
				datalen = strlen(distBuff);
				memset(distBuff, '\0', datalen);
				sprintf(distBuff, COM_DISTANCE_SEND_FORMAT, distance);
				datalen = strlen(distBuff);
				sCOM_SendUSB((uint8_t *)distBuff, datalen);
			}
			else
			{
				/* Do Nothing */
			}
			if(osOK == osSemaphoreAcquire(xSemaphore_SensorErrorHandle, osNoTimeout))
			{
				/* Errors on the VL53L0X communication */
				osThreadTerminate(TaskSensorHandle);
				osTimerStart(xTimer_RestartSensorTaskHandle, pdMS_TO_TICKS(COM_TIME_TO_RESET_SENSOR_TASK));
			}
		}
		else
		{
			/* Other mode it's selected */
			if(osOK == osSemaphoreAcquire(xSemaphore_SensorErrorHandle, 10))
			{
				/* Errors on the VL53L0X communication */
				osThreadTerminate(TaskSensorHandle);
				osTimerStart(xTimer_RestartSensorTaskHandle, pdMS_TO_TICKS(COM_TIME_TO_RESET_SENSOR_TASK));
			}
		}
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX);
	}
}
/**
 * ---------------------------------------------------------
 * 					    SOFTWARE COMPONENT SUB THREADS
 * ---------------------------------------------------------
 */
/**
* @brief Function implementing the SubTaskUSB thread.
* @note  Thread that process the incoming data from
 *       the USB, this data it's transmitted with a global
 *       buffer called "CDC_ResBuffer"
* @param argument: Not used
* @retval None
*/
void vSubTaskUSB(void *argument)
{
	char statsBuffer[1024] = {0};
	char USBBuffer[1024] = {0};
	float Kp, Ki, Kd;
	uint32_t usbFlags, modeFlags;
	uint16_t datalen;
	int16_t setPoint;
	int32_t actionControl;
	int32_t period, freq;

	for(;;)
	{
		usbFlags = osEventFlagsWait(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX, osFlagsWaitAny, osWaitForever);
		if(usbFlags&CDC_FLAG_MESSAGE_RX)
		{
			/* An USB message arrived */
			/* Software Integration */
			if(strcmp(CDC_ResBuffer, COM_CPU_LOAD_USB_MSG) == 0)
			{
				/* Make a CPU load measure - No args*/
				datalen = strlen(statsBuffer);
				memset(statsBuffer, '\0', datalen);
				vTaskGetRunTimeStats(statsBuffer);
				strcat(statsBuffer, "\nend\n"); /* Data specific for CPULoad script */
				datalen = strlen(statsBuffer);
				sCOM_SendUSB((uint8_t *)statsBuffer, datalen);
				datalen = 0;
			}
			/* PID Manipulation */
			else if(strcmp(CDC_ResBuffer, COM_CONSTANT_USB_MSG) == 0)
			{
				/* Change the constants of the PID controller - Args: KP, KI, KD!*/
				COM_WaitForArgs();
				if(OK != sUSB_ParseGains(&Kp, &Ki, &Kd))
				{
					/* Errors on the parse request the constants again */
					datalen = strlen(USBBuffer);
					memset(USBBuffer, '\0', datalen);
					sprintf(USBBuffer, "ERR\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
					datalen = 0;
				}
				else
				{
					/* All Ok */
					datalen = strlen(USBBuffer);
					memset(USBBuffer, '\0', datalen);
					sprintf(USBBuffer, "OK\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
					PID_SetControlGains(Kp, Ki, Kd);
					sCOM_SendConstants(Kp, Ki, Kd);
				}
			}
			else if(strcmp(CDC_ResBuffer, COM_GET_CONSTANTS_USB_MSG) == 0)
			{
				/* Request the value of the PID constants - No Args */
				PID_GetControlGains(&Ki, &Kd, &Kp);
				datalen = strlen(USBBuffer);
				memset(USBBuffer, '\0', datalen);
				sprintf(USBBuffer, "KP:%f!KI:%f!KD:%f\n", Ki, Kd, Kp);
				datalen = strlen(USBBuffer);
				sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
			}
			else if(strcmp(CDC_ResBuffer, COM_SET_FIXED_FREQ_DIST) == 0)
			{
				/* Set a fixed frequency for distance report - Args: Frequency on HZ */
				COM_WaitForArgs();
				if(EOF != sscanf(CDC_ResBuffer, "%d", (int *)&freq))
				{
					float periodF = 1/freq;
					periodF *= 1000; /* Changing to ms */
					period = (int32_t)periodF;
					if (period>0)
					{
						DistanceSensor_SetSamplingFreq(period);
					}
				}
			}
			else if(strcmp(CDC_ResBuffer, COM_SET_FIXED_PERIOD_DIST) == 0)
			{
				/* Set a fixed period for distance report - Args: period on ms */
				COM_WaitForArgs();
				if(EOF != sscanf(CDC_ResBuffer, "%d", (int *)&period))
				{
					/*All good*/
					if(period > 0)
					{
						DistanceSensor_SetSamplingFreq(period);
					}
				}
				else
				{
					/* Errors on the parse, deprecate send */
					datalen = strlen(USBBuffer);
					memset(USBBuffer, '\0', datalen);
					sprintf(USBBuffer, "ERR\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
				}
			}
			else if(strcmp(CDC_ResBuffer, COM_SET_FREE_FREQ_DIST) == 0)
			{
				/* Set free sampling frequency on distance sensor - No Args*/
				DistanceSensor_SetSamplingFreq(0);
			}
			/* Modes manipulation */
			else if(strcmp(CDC_ResBuffer, COM_CONTROL_ACTION_USB_MSG) == 0)
			{
				/* Desired action control (only valid when mode is on slave) - Args: ControlAction 0:100*/
				COM_WaitForArgs();
				if(EOF != sscanf(CDC_ResBuffer, "%d", (int *)&actionControl))
				{
					/* Correct parse of the buffer */
					osMessageQueuePut(xFIFO_FANDutyCycleHandle, &actionControl, 0U, osNoTimeout);
				}
				else
				{
					/* Errors on the parse, deprecate send */
					datalen = strlen(USBBuffer);
					memset(USBBuffer, '\0', datalen);
					sprintf(USBBuffer, "ERR\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
				}
			}
			else if(strcmp(CDC_ResBuffer, COM_SET_POINT_USB_MSG) == 0)
			{
				/* Communicate the set point - Args Setpoint */
				COM_WaitForArgs();
				if(EOF != sscanf(CDC_ResBuffer, "%d", (int *)&setPoint))
				{
					/* Correct Parse of the buffer */
					modeFlags = osEventFlagsGet(xEvent_ControlModesHandle);
					if(modeFlags&SLAVE_FLAG)
					{
						/* Can send the Set point */
						COM_SendMessage(SET_POINT, MSG_TYPE_ONDEMAND, PRIORITY_MEDIUM, setPoint);
					}
				}
				else
				{
					/* Errors on the parse, deprecate send */
					datalen = strlen(USBBuffer);
					memset(USBBuffer, '\0', datalen);
					sprintf(USBBuffer, "ERR\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
				}
			}
			/* NVM manipulation */
			else if(strcmp(CDC_ResBuffer, COM_SET_NVM_DEFAULT) == 0)
			{
				/* Set the default values of the EEPROM variables - No Args */
				datalen = strlen(USBBuffer);
				memset(USBBuffer, '\0', datalen);
				if(OK == NVM_loadDefaultValues())
				{
					/* Loaded correctly */

					sprintf(USBBuffer, "OK\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
				}
				else
				{
					/* Errors on the write */
					sprintf(USBBuffer, "ERR\n");
					datalen = strlen(USBBuffer);
					sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
				}
			}
			else if(strcmp(CDC_ResBuffer, COM_SET_NVM_DUMP) == 0)
			{
				/* Make a memory dump of the EEPROM data - No Args*/
				char *bufPointer = (char *)NVM_memoryDump(osWaitForever); /*TODO: Test here*/
				datalen = strlen(USBBuffer);
				memset(USBBuffer, '\0', datalen);
				memcpy(USBBuffer, bufPointer, EEPROM_SIZE);
				datalen = strlen(USBBuffer);
				sCOM_SendUSB((uint8_t *)USBBuffer, datalen);
			}
			else
			{
				/* Unknown message, do nothing */
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
}

/**
* @brief Function implementing the SubTaskUART thread.
* @param argument: Not used
* @retval None
*/
void vSubTaskUART(void *argument)
{
	char bufferCache[COM_UART_PERIODIC_NUMBER_FRAMES_RX];
	const char usbBuffer[8] = COM_CONFIRM_SLAVE_MODE;
	uint32_t flags = 0;
	int16_t setPoint = 0;
	int8_t controlAction = 0;
	uint8_t datalen = sizeof(COM_CONFIRM_SLAVE_MODE);
	ControlModes mode = AutoPID;

	for(;;)
	{
		osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
		memcpy(bufferCache, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX);
		/* Reseting the Watch dog timer for UART */
		osTimerStart(xTimer_WdgUARTHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));
		switch(UARTfirstFrame)
		{
			case COM_UART_INIT_FRAME_VALUE:
				sendInitBuffer();
			break;
			case SET_POINT:
				/* Communicate with PID component */
				setPoint = UART_RxBuffer[1] | (UART_RxBuffer[2] << 8);
				osMessageQueuePut(xFIFO_PIDSetPointHandle, &setPoint, 0U, osNoTimeout);
			break;
			case TOGGLE_PID:
				PID_TogglePID();
			break;
			case MODE_SELECTION:
				mode = UART_RxBuffer[1]; /* Directly compatible from Daughterboard */
				osEventFlagsClear(xEvent_ControlModesHandle, AUTO_PID_FLAG | SLAVE_FLAG | MANUAL_FLAG);
				switch(mode)
				{
					case Manual:
						if(PID_isActive())
						{
							/* Need to turn off */
							PID_TurnOff();
						}
						osEventFlagsSet(xEvent_ControlModesHandle, MANUAL_FLAG);
					break;
					case AutoPID:
						if(!PID_isActive())
						{
							/* Need to turn on */
							PID_TurnOn();
						}
						osEventFlagsSet(xEvent_ControlModesHandle, AUTO_PID_FLAG);
					break;
					case Slave:
						if(PID_isActive())
						{
							/* Need to turn off */
							PID_TurnOff();
						}
						osEventFlagsSet(xEvent_ControlModesHandle, SLAVE_FLAG);
						sCOM_SendUSB((uint8_t *)usbBuffer, datalen);
					break;
				}
			break;
			case ACTION_CTRL:
				flags = osEventFlagsGet(xEvent_ControlModesHandle);
				if(flags&MANUAL_FLAG)
				{
					/* Only in this case the message is valid */
					controlAction = bufferCache[1];
					osMessageQueuePut(xFIFO_FANDutyCycleHandle, &controlAction, 0U, osNoTimeout);
				}
				else
				{
					/* Message invalid */
				}
			break;
			case RESET:
				HAL_NVIC_SystemReset();
			break;
			case NO_MSG_ID:
			default:
				/* Do Nothing */
			break;
		}

	}
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/* Approach for the COM message sender */
/*
 * @brief		Send the message to the buffer
 * @return	result of the operation
 */
result_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) {
	static uint8_t buffer[sizeof(uint32_t)] = {0};
	uint8_t *bytePointer = (uint8_t *)&payload;

	osTimerStop(xTimer_UARTSendHandle);
	buffer[0] = messageID;
	for(uint16_t i = 1; i < sizeof(uint32_t)+1; i++, bytePointer++)
	{
		buffer[i] = *bytePointer;
	}
	HAL_UART_Transmit_DMA(&huart1, buffer, sizeof(uint32_t));
	osTimerStart(xTimer_UARTSendHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));
	return OK;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  Syncronize the communication by sending
 *         the init frames when called
 * @note   This function may poll the task when it's called
 *         do not call this function on an ISR
 * @param  none
 * @retval none
 */
static void syncComm(void)
{
	osStatus_t status;
	bool errorFlagWereSet = false;

	do {
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX); /* Jump the ISR when first arrived to receive the data */
		/* Send initial buffer */
		sendInitBuffer();
		/* Wait for the UART to receive the init message of Daughter */
		status = osSemaphoreAcquire(xSemaphore_InitDaughterHandle, pdMS_TO_TICKS(COM_SECONDS_TO_WAIT_DAUGHTER_INIT*1000)); /* Wait for daughter to init*/
		if(osOK == status)
		{
			/* All Ok to go */
			/* Start soft-timer for UART send */
			osTimerStart(xTimer_UARTSendHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));
		}
		else
		{
			/* Timeout, asumming errors on the Daughterboard init */
			/* Reporting to LEDS SWC */
			osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
			errorFlagWereSet = true;
			/* TODO: Trigger DTC Not GPU Comm */
		}
	}while(osOK != status);
	firstInit = false;

	if(errorFlagWereSet)
	{
		/* Disable the flag */
		osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
	}
	else
	{
		/* Do nothing */
	}
}
/**
 * @brief  Send through the UART Bus the "INIT TX BUS"
 * @param  none
 * @retval none
 */
static void sendInitBuffer(void)
{
	static uint8_t buffer[COM_UART_INIT_NUMBER_FRAMES] = {0}; /* Fixed size following the doc */
	NVMType32 Kp, Ki, Kd; /* PID constants */
	uint32_t i = 1; /* Iterator used as index for the buffer filling */

#ifdef ENABLE_EEPROM
	if(OK != NVM_Read(KP_PID_BASE_ADDR, &Kp))
#endif
	{
		/* Could not read the NVM */
		Kp.dataFloat = KP_DEFAULT;
	}
#ifdef ENABLE_EEPROM
	if(OK != NVM_Read(KI_PID_BASE_ADDR, &Ki))
#endif
	{
		/* Could not read the NVM */
		Ki.dataFloat = KI_DEFAULT;
	}
#ifdef ENABLE_EEPROM
	if(OK != NVM_Read(KD_PID_BASE_ADDR, &Kd))
#endif
	{
		/* Could not read the NVM */
		Kd.dataFloat = KD_DEFAULT;
	}

	/* Building the buffer transmit */
	buffer[0] = COM_UART_INIT_FRAME_VALUE;
	/* Filling Kp */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
			buffer[i] = Kp.rawData[j];
	}
	/* Filling Ki */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
		buffer[i] = Ki.rawData[j];
	}
	/* Filling Kd */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
		buffer[i] = Kd.rawData[j];
	}
	HAL_UART_Transmit_DMA(&huart1, buffer, COM_UART_INIT_NUMBER_FRAMES);
	osSemaphoreAcquire(xSemaphore_UARTTxCpltHandle, osWaitForever);
	osSemaphoreRelease(xSemaphore_UARTTxCpltHandle);
}

/**
 * @brief  This function send the new values
 * 			   of the control gains to the daughterboard
 * @param  kp cosntant
 * @param  ki constant
 * @param  kd constant
 * @retval none
 */
static void sCOM_SendConstants(float kp, float ki, float kd)
{
	static uint8_t buffer[COM_UART_INIT_NUMBER_FRAMES] = {0}; /* Fixed size following the doc */
	NVMType32 Kp, Ki, Kd; /* PID constants */
	uint32_t i = 1; /* Iterator used as index for the buffer filling */
	osTimerStop(xTimer_UARTSendHandle);
	Kp.dataFloat = kp;
	Ki.dataFloat = ki;
	Kd.dataFloat = kd;

	/* Building the buffer transmit */
	buffer[0] = COM_UART_INIT_FRAME_VALUE;
	/* Filling Kp */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
			buffer[i] = Kp.rawData[j];
	}
	/* Filling Ki */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
		buffer[i] = Ki.rawData[j];
	}
	/* Filling Kd */
	for(uint32_t j = 0; j < sizeof(NVMType32); i++, j++)
	{
		buffer[i] = Kd.rawData[j];
	}
	HAL_UART_Transmit_DMA(&huart1, buffer, COM_UART_INIT_NUMBER_FRAMES);
	osSemaphoreAcquire(xSemaphore_UARTTxCpltHandle, osWaitForever);
	osSemaphoreRelease(xSemaphore_UARTTxCpltHandle);
	osTimerStart(xTimer_UARTSendHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));
}

/**
 * @brief  This function parse the Gains and delivers
 *         as a pointer in order to change the gains
 * @note   Expected Args: KP, KI, KD
 * @param  *kp value
 * @param  *ki value
 * @param  *kd value
 * @retval result of the operation
 */
static result_t sUSB_ParseGains(float *kp, float *ki, float *kd)
{
	char MsgBuffer[sizeof(CDC_ResBuffer)] = "";
	uint16_t len = strlen(CDC_ResBuffer);
	result_t retval = OK;
	uint16_t msgIndex = 0;
	char KpBuf[32] = "";
	char KiBuf[32] = "";
	char KdBuf[32] = "";

	memcpy(MsgBuffer, CDC_ResBuffer, len);
	/* Dividing KP */
	for(uint16_t bufIndex = 0; msgIndex < len; msgIndex++, bufIndex++)
	{
		if(MsgBuffer[msgIndex] != ',')
		{
			/* Can fill the buffer */
			KpBuf[bufIndex] = MsgBuffer[msgIndex];
		}
		else
		{
			/* End of the buffer */
			msgIndex++;
			break;
		}
	}
	/* Dividing KI */
	for(uint16_t bufIndex = 0; msgIndex < len; msgIndex++, bufIndex++)
		{
			if(MsgBuffer[msgIndex] != ',')
			{
				/* Can fill the buffer */
				KiBuf[bufIndex] = MsgBuffer[msgIndex];
			}
			else
			{
				/* End of the buffer */
				msgIndex++;
				break;
			}
		}
	/* Dividing KD */
	for(uint16_t bufIndex = 0; msgIndex < len; msgIndex++, bufIndex++)
		{
			if(MsgBuffer[msgIndex] != ',')
			{
				/* Can fill the buffer */
				KdBuf[bufIndex] = MsgBuffer[msgIndex];
			}
			else
			{
				/* End of the buffer */
				break;
			}
		}
	/* Parsing the strings */
	if(EOF == sscanf(KpBuf, "%f", kp))
	{
		/* Parse with errors */
		retval = Error;
	}
	if(EOF == sscanf(KiBuf, "%f", ki) || Error == retval)
	{
		/* Parse with errors */
		retval = Error;
	}
	if(EOF == sscanf(KdBuf, "%f", kd) || Error == retval)
	{
		/* Parse with errors */
		retval = Error;
	}
	return retval;
}


/**
 * @brief  Sends USB information
 * @param  buf Buffer that has the information
 * @param  len Lenght of the message to transmit
 * @retval result of the operation
 */
static result_t sCOM_SendUSB(uint8_t *buf, uint16_t len)
{
	const uint16_t maxIntents = 50;
	uint16_t intents = 0;
	result_t retval = OK;
	uint8_t CDC_res;
	do{
		/* USB send */
		if(USBD_OK == CDC_getReady())
		{
			/* USB it's free */
			CDC_res = CDC_Transmit_FS(buf, len);
			if(CDC_res != USBD_OK)
			{
				/* Transmission with errors */
				retval = Error;
			}
			else
			{
				/* All OK */
				retval = OK;
			}
		}
		else
		{
			/* Wait for usb to send */
			intents++;
		}
		if(maxIntents < intents)
		{
			/* Limit of the intents */
			retval = Error;
			break;
		}

	}while(CDC_res != OK);
	return retval;
}
/**
 * ---------------------------------------------------------
 * 					        SOFT-TIMERS CALLBACKS
 * ---------------------------------------------------------
 */
/**
 * @brief  Callback for the UART periodical send
 * @param  *argument none
 * @retval none
 */
void vTimer_UARTSendCallback(void *argument)
{
	static int16_t distance = 0;
	static int16_t RPM = 0;
	static int16_t actionControl = 0;
	static uint16_t emptyFIFOCounterRPM = 0;
	static uint8_t buffer[COM_UART_PERIODIC_NUMBER_FRAMES_TX+1] = {0};
	volatile uint8_t *bytePointer;
	uint8_t acknowledge = 1;
	uint8_t confirmReset = 0;
	uint32_t fatalErrorFlags = 0;
	osStatus_t status;

	fatalErrorFlags = osEventFlagsGet(xEvent_FatalErrorHandle);
	if(fatalErrorFlags&FATAL_ERROR_GPU)
	{
		return; /* Cancel the send */
	}
	else
	{
		/* Do Nothing */
	}

	buffer[0] = ACK_DATA | acknowledge | confirmReset << 1;


	/* DISTANCE PROCESSING */
	{
		/* Getting the distance data */
		osMessageQueueGet(xFIFO_COMDistanceHandle, &distance, NULL, osNoTimeout);
		/* Load Distance data */
		{
			/* If there's no data on the FIFO, the bus will take the last value*/
			bytePointer = (uint8_t *)&distance;
			/* Sending first the LSB (Big Endian) */
			buffer[1] = *bytePointer;
			bytePointer++;
			buffer[2] = *bytePointer;
		}
	}

	/* RPM PROCESSING */
	{
		/* Getting the RPM data */
		status = osMessageQueueGet(xFIFO_COMRPMHandle, &RPM, NULL, osNoTimeout);
		if(osOK != status)
		{
			/* The FIFO is empty */
			emptyFIFOCounterRPM++;
		}
		else
		{
			/* All Good */
			emptyFIFOCounterRPM = 0;
		}

		/* Load RPM data */
		if(COM_FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterRPM)
		{
			/* Send error code */
			buffer[3] = COM_MSG_ERROR_CODE;
			buffer[4] = COM_MSG_ERROR_CODE;
			/* TODO Trigger DTC Not Receiving Distance FIFO */
		}
		else
		{
			/* If there's no data on the FIFO, the bus will take the last value*/
			bytePointer = (uint8_t *)&RPM;
			/* Sending first the LSB (Big Endian) */
			buffer[3] = *bytePointer;
			bytePointer++;
			buffer[4] = *bytePointer;
		}
	}
	/* ACTION CONTROL PROCESSING */
	{
		status = osMessageQueueGet(xFIFO_COMActionControlHandle, &actionControl, NULL, osNoTimeout);
		/* If there's no data on the FIFO, the bus will take the last value*/
		buffer[5] = actionControl;
	}
	osSemaphoreAcquire(xSemaphore_UARTTxCpltHandle, osWaitForever);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buffer, COM_UART_PERIODIC_NUMBER_FRAMES_TX);
}

/**
 * @brief  Callback for the watchdog soft-timer UART
 *         receive acknowledge
 * @param  *argument none
 * @retval none
 */
void vTimer_WdgUARTCallback(void *argument)
{
	/* Watchdog soft-timer */
	/* Reporting to LEDS SWC */
	osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
	/* TODO: Trigger DTC Not GPU Comm */
}

/**
 * @brief
 * @param  *argument none
 * @retval none
 */
void vTimer_RestartSensorTaskCallback(void *argument)
{
	/* Restart the task */
	//TaskSensorHandle = osThreadNew(vTaskSensor, NULL, &TaskSensor_attributes); TODO No secure call before scheduler started
}
/**
 * ---------------------------------------------------------
 * 					        HARD-TIMERS CALLBACKS
 * ---------------------------------------------------------
 */
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(TIM1 == htim->Instance)
	{
		/* Watchdog timer for UART */
		/* Reporting to LEDS SWC */
		//osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_GPU); /* TODO Enable Code here */
		/* TODO: Trigger DTC Not GPU Comm */
	}
}
/**
 * ---------------------------------------------------------
 * 					      ISR COMPONENT CALLBACKS
 * ---------------------------------------------------------
 */

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(USART1 == huart -> Instance)
	{
		osSemaphoreRelease(xSemaphore_UARTTxCpltHandle);
	}
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);

	osSemaphoreRelease(xSemaphore_InitDaughterHandle);
	/* It's a periodic frame */
	if(NO_MSG_ID != UART_RxBuffer[0])
	{
		/* Indicating that the component has to process the data */
		osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
	}
	else
	{
		/* It's just the acknowledge */
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX);
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);

	osSemaphoreRelease(xSemaphore_InitDaughterHandle);
	/* It's a periodic frame */
	if(NO_MSG_ID != UART_RxBuffer[0])
	{
		/* Indicating that the component has to process the data */
		osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
	}
	else
	{
		/* Do Nothing, It's just the acknowledge */
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX);
}

/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	static uint32_t FrameErrorCounts = 0, OverrunErrorsCounts = 0;
	uint32_t uartError = HAL_UART_GetError(huart);
	uint32_t dummy = 0; /* Dummy variable to avoid compiler optimization*/

	switch (uartError)
	{
		case HAL_UART_ERROR_NONE:
			/* No errors */
			dummy++;
		break;
		case HAL_UART_ERROR_PE:
			/* Parity Error */
			dummy++;
		break;
		case HAL_UART_ERROR_NE:
			/* Noise Error */
			dummy++;
		break;
		case HAL_UART_ERROR_FE:
			/* Frame Error */
			FrameErrorCounts++;
		break;
		case HAL_UART_ERROR_ORE:
			/* Overrun error */
			OverrunErrorsCounts++;
		break;
		case HAL_UART_ERROR_DMA:
			/* DMA Transfer Error */
			dummy++;
		break;
		default:
			/* Unknown error */
		break;
	}
	/* Continue with the transmission and try to report the number of this errors */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES_RX);
}

uint32_t errorCounterI2C1 = 0;

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	if(I2C1 == hi2c->Instance)
	{
		osSemaphoreRelease(xSemaphore_SensorRxCpltHandle); /* VL53L0X Read action completed */
		errorCounterI2C1 = 0;
	}
	else if(I2C2 == hi2c->Instance)
		osSemaphoreRelease(xSemaphore_DMA_TransferCpltHandle); /* EEPROM DMA Transfer completed */
}

/** @brief  Slave Tx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(I2C1 == hi2c->Instance)
	{
		osSemaphoreRelease(xSemaphore_SensorRxCpltHandle); /* VL53L0X Write action completed */
		errorCounterI2C1 = 0;
	}
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	if(I2C1 == hi2c->Instance)
	{
		/* Error transmission for the VL53L0X */
		errorCounterI2C1++;
		if(COM_ERROR_COUNTS_TO_CANCEL_VL53L0X_TX < errorCounterI2C1)
		{
			/* Too many errors, canceling the transmission to the sensor */
			/* Requesting task reset when timer */
			osSemaphoreRelease(xSemaphore_SensorErrorHandle);
		}
	}
	else if(I2C2 == hi2c->Instance)
	{
		/* TODO: Handle Logic */
		/* TODO: Turn on event flag Avoid Send EEPROM Data*/
	}
}
