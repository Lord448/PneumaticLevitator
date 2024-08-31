/**
 * @file      COM.c
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

#include "COM.h"

#define UARTfirstFrame UART_RxBuffer[0]

extern UART_HandleTypeDef huart1;

extern osThreadId_t TaskSensorHandle;

extern osThreadAttr_t TaskSensor_attributes;

extern osMessageQueueId_t xFIFO_COMHandle;
extern osMessageQueueId_t xFIFO_COMDistanceHandle;
extern osMessageQueueId_t xFIFO_COMRPMHandle;
extern osMessageQueueId_t xFIFO_COMActionControlHandle;
extern osMessageQueueId_t xFIFO_PIDSetPointHandle;

extern osTimerId_t xTimer_UARTSendHandle;
extern osTimerId_t xTimer_WdgUARTHandle;
extern osTimerId_t xTimer_RestartSensorTaskHandle;

extern osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorTxCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorRxCpltHandle;
extern osSemaphoreId_t xSemaphore_InitDaughterHandle;
extern osSemaphoreId_t xSemaphore_SensorErrorHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;
extern osSemaphoreId_t xSemaphore_UARTTxCpltHandle;

extern osEventFlagsId_t xEvent_USBHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;

extern char CDC_ResBuffer[64];

extern void vTaskSensor(void *argument);

uint8_t UART_RxBuffer[COM_UART_PERIODIC_NUMBER_FRAMES_RX] = {0};
bool firstInit = true;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static result_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);
static void sendInitBuffer(void);
static void syncComm(void);
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
	uint32_t flags = 0;
	syncComm();
	for(;;)
	{
		flags = osEventFlagsGet(xEvent_FatalErrorHandle);
		if(flags&FATAL_ERROR_GPU)
		{
			/* Handling the fatal error */
			syncComm();
		}
		/* Searching if there are errors on the sensor VL53L0X TODO: Move this to ModeManager*/
		if(osOK == osSemaphoreAcquire(xSemaphore_SensorErrorHandle, 100))
		{
			/* Errors on the VL53L0X communication */
			osThreadTerminate(TaskSensorHandle);
			osTimerStart(xTimer_RestartSensorTaskHandle, pdMS_TO_TICKS(COM_TIME_TO_RESET_SENSOR_TASK));
		}
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
	uint32_t usbFlags = 0;

	for(;;)
	{
		usbFlags = osEventFlagsWait(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX, osFlagsWaitAny, osWaitForever);

		if(usbFlags&CDC_FLAG_MESSAGE_RX)
		{
			/* An USB message arrived */
			if(strcmp(CDC_ResBuffer, "CPU") == 0) /* TODO: Instrumented code */
			{
				/* TODO: If decided, implement this code on DiagAppl */
				result_t result = Error;
				memset(statsBuffer, '\0', strlen(statsBuffer));
				vTaskGetRunTimeStats(statsBuffer);
				strcat(statsBuffer, "\nend\n"); /* Data specific for CPULoad script */
				do{
					if(USBD_OK == CDC_getReady())
					{
						/* The USB it's not busy */
						result = CDC_Transmit_FS((uint8_t *)statsBuffer, strlen(statsBuffer));
					}
					else
					{
						/* Wait for the USB to send */
					}
				}while(result != OK);
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
	int16_t setPoint = 0;
	for(;;)
	{
		osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
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
 * @return	Number of messages in the queue, -1 for full queue
 */
int16_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) {
	PDU_t pdu;

	if (OK == COM_CreatePDU(&pdu, messageID, type, priority, payload)) { /* Check sucessful set */
		uint16_t currentMessages = osMessageQueueGetCount(xFIFO_COMHandle);
		/* Checking if the queue is not full */
		if (COM_MAX_QUEUE_MESSAGES > currentMessages) {
			/* Putting the message in the queue and returning queue current messages */
			osMessageQueuePut(xFIFO_COMHandle, &pdu, 0U, 0U);
			return currentMessages;
		}

		return -1;
	}

	return 0;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/* PDU Constructor */
/*
 * @brief 		Set values to a blank PDU
 * @return		0 for successful value assignment
 */
static result_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) {
    /* Checking that the data is inside of bounderies */
    if (MSG_TYPE_MAX  <= type ||  PRIORITY_MAX <= priority)
    {
        return Error; /* Values out of range */
    }
    pdu->fields.messageID = messageID;
    pdu->fields.messageType = type;
    pdu->fields.priority = priority;
    pdu->fields.payload = payload;

    return OK;
}
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
		HAL_UART_Receive_DMA(&huart1, UART_RxBuffer, sizeof(uint8_t)); /* Jump the ISR when first arrived to receive the data */
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

	if(OK != NVM_Read(KP_PID_BASE_ADDR, &Kp))
	{
		/* Could not read the NVM */
		Kp.dataFloat = KP_DEFAULT;
	}
	if(OK != NVM_Read(KI_PID_BASE_ADDR, &Ki))
	{
		/* Could not read the NVM */
		Ki.dataFloat = KI_DEFAULT;
	}
	if(OK != NVM_Read(KD_PID_BASE_ADDR, &Kd))
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
 * ---------------------------------------------------------
 * 					        SOFT-TIMERS CALLBACKS
 * ---------------------------------------------------------
 */
/**
 * @brief
 * @param  *argument none
 * @retval none
 */
void vTimer_UARTSendCallback(void *argument)
{
	static int16_t distance = 0;
	static int16_t RPM = 0;
	static int16_t actionControl = 0;
	static uint16_t emptyFIFOCounterDist = 0;
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
		status = osMessageQueueGet(xFIFO_COMDistanceHandle, &distance, NULL, osNoTimeout);
		if(osOK != status)
		{
			/* The FIFO is empty */
			emptyFIFOCounterDist++;
		}
		else
		{
			/* All Good */
			emptyFIFOCounterDist = 0;
		}
		/* Checking value of distance */
		if(distance < 0)
		{
			/* Negative value on distance */
			distance = 0;
		}
		else
		{
			/* Do Nothing */
		}

		/* Load Distance data */
		if(COM_FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterDist)
		{
			/* Send error code */
			buffer[1] = COM_MSG_ERROR_CODE;
			buffer[2] = COM_MSG_ERROR_CODE;
			/* TODO Trigger DTC Not Receiving Distance FIFO */
		}
		else
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
	/* TODO: ON DEMAND MSG PROCESS */
	{

	}
	osSemaphoreAcquire(xSemaphore_UARTTxCpltHandle, osWaitForever);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buffer, COM_UART_PERIODIC_NUMBER_FRAMES_TX);
	bytePointer = (uint8_t *)&RPM; /* TODO: Stubbed code */
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
	//syncComm();
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
		//syncComm();
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
		/* Do Nothing, It's just the acknowledge */
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
