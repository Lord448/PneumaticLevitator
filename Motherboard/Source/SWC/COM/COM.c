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

extern UART_HandleTypeDef huart1;

extern osThreadId_t TaskSensorHandle;

extern osThreadAttr_t TaskSensor_attributes;

extern osMessageQueueId_t xFIFO_COMHandle;
extern osMessageQueueId_t xFIFO_COMDistanceHandle;
extern osMessageQueueId_t xFIFO_COMRPMHandle;

extern osTimerId_t xTimer_UARTSendHandle;
extern osTimerId_t xTimer_WdgUARTHandle;
extern osTimerId_t xTimer_RestartSensorTaskHandle;

extern osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorTxCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorRxCpltHandle;
extern osSemaphoreId_t xSemaphore_InitDaughterHandle;
extern osSemaphoreId_t xSemaphore_SensorErrorHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;

extern osEventFlagsId_t xEvent_USBHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;

extern char CDC_ResBuffer[64];

extern void vTaskSensor(void *argument);

uint8_t UART_RxBuffer[COM_UART_PERIODIC_NUMBER_FRAMES] = {0};

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sendInitBuffer(void);
static void processUSBData(void);
static void processUARTData(uint8_t *buffer);
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
	uint8_t buffer[6] = {0};
	uint8_t ack = 0; /*Dummy memory region since the data it's handled on the ISR*/
	osStatus_t status;
	uint8_t errorCounter = 0;

	osThreadId_t SubTaskUSB;
	uint32_t SubTaskUSBStackBuffer[128];
	osThreadId_t SubTaskUART;


	HAL_UART_Receive_IT(&huart1, &ack, sizeof(uint8_t));
	do {
		/*
		 * @note If the Daughterboard fails, during SECONDS_TO_WAIT_DAUGHTER_INIT*10
		 * seconds the system wont be able to communicate via USB since the
		 * task is polled here (USB Data can be received, but the system
		 * wont response after that time)
		 *
		 */
		/* Send initial buffer */
		sendInitBuffer();
		/* Wait for the UART to receive the init message of Daughter */
		status = osSemaphoreAcquire(xSemaphore_InitDaughterHandle, pdMS_TO_TICKS(COM_SECONDS_TO_WAIT_DAUGHTER_INIT*1000)); /* Wait for daughter to init*/
		if(osOK == status)
		{
			/* All Ok to go */
			/* Set the RX UART bit (wait for IDLE bus state to process the ISR)*/
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, buffer, sizeof(buffer));
			/* Free the memory reserved by the UART Semaphore */
			osSemaphoreDelete(xSemaphore_InitDaughterHandle);
			/* Start soft-timer for UART send */
			osTimerStart(xTimer_UARTSendHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));
		}
		else
		{
			/* Timeout, asumming errors on the Daughterboard init */
			/* Reporting to LEDS SWC */
			osSemaphoreRelease(xSemaphore_InitDaughterHandle);
			osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
			//osDelay(pdMS_TO_TICKS(500));
			errorCounter++;
			/* TODO: Trigger DTC Not GPU Comm */
		}
	}while(osOK != status && errorCounter < 10);

	for(;;)
	{
		/* TODO: Check possible creation of subthreads for USB and UART communication */
		/* Process USB info */
		processUSBData();
		/* Process UART info */
		processUARTData(buffer);
		/* Searching if there are errors on the sensor VL53L0X TODO: Move this to ModeManager*/
		if(osOK == osSemaphoreAcquire(xSemaphore_SensorErrorHandle, 0U))
		{
			/* Errors on the VL53L0X communication */
			osThreadTerminate(TaskSensorHandle);
			osTimerStart(xTimer_RestartSensorTaskHandle, pdMS_TO_TICKS(COM_TIME_TO_RESET_SENSOR_TASK));
		}
	}
}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/* PDU Constructor */
/*
 * @brief 		Set values to a blank PDU
 * @return		0 for successful value assignment
 */
uint8_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) {
    /* Checking that the data is inside of bounderies */
    if (MSG_TYPE_MAX  <= type ||  PRIORITY_MAX <= priority)
    {
        return 1; /* Values out of range */
    }

    pdu->fields.messageID = messageID;
    pdu->fields.messageType = type;
    pdu->fields.priority = priority;
    pdu->fields.payload = payload;

    return 0;
}

/* Approach for the COM message sender */
/*
 * @brief		Send the message to the buffer
 * @return		Number of messages in the queue, -1 for full queue
 */
int16_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) {
	PDU_t pdu;
	
	if (0 == COM_CreatePDU(&pdu, messageID, type, priority, payload)) { /* Check sucessful set */
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
/**
 * @brief  Function that process the incoming data from
 *         the USB, this data it's transmitted with a global
 *         buffer called "CDC_ResBuffer"
 * @param  none
 * @retval none
 */
static void processUSBData(void)
{
	char statsBuffer[512] = {0};
	uint32_t usbFlags = 0;

	usbFlags = osEventFlagsWait(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX, osFlagsWaitAny, pdMS_TO_TICKS(5));

	if(usbFlags&CDC_FLAG_MESSAGE_RX)
	{
		/* An USB message arrived */
		if(strcmp(CDC_ResBuffer, "CPU") == 0) /* TODO: Instrumented code */
		{
			/* TODO: When COM finished, implement this code on DiagAppl */
			memset(statsBuffer, '\0', strlen(statsBuffer));
			vTaskGetRunTimeStats(statsBuffer);
			strcat(statsBuffer, "\nend\n"); /* Data specific for CPULoad script */
			if(USBD_OK == CDC_getReady())
			{
				/* The USB it's not busy */
				CDC_Transmit_FS((uint8_t *)statsBuffer, strlen(statsBuffer));
			}
		}
	}
}

/**
 * @brief
 * @param  none
 * @retval none
 */
static void processUARTData(uint8_t *buffer)
{
	osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, pdMS_TO_TICKS(5));
	/* TODO: Handle here the communication */
}

/**
 * @brief  Send through the UART Bus the "INIT TX BUS"
 * @param  none
 * @retval none
 */
static void sendInitBuffer(void)
{
	uint8_t buffer[12] = {0}; /* Fixed size following the doc */
	NVMType32 Kp, Ki, Kd;
	uint32_t i = 1;

	NVM_Read(KP_PID_BASE_ADDR, &Kp);
	NVM_Read(KI_PID_BASE_ADDR, &Ki);
	NVM_Read(KD_PID_BASE_ADDR, &Kd);

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
	HAL_UART_Transmit_DMA(&huart1, buffer, sizeof(buffer));
	osSemaphoreAcquire(xSemaphore_InitDaughterHandle, osWaitForever);
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
	static uint16_t emptyFIFOCounterDist = 0;
	static uint16_t emptyFIFOCounterRPM = 0;
	uint8_t *bytePointer;
	uint8_t buffer[6] = {0};
	uint8_t acknowledge = 1;
	uint8_t confirmReset = 0;
	uint32_t fatalErrorFlags = 0;
	osStatus_t status;

	fatalErrorFlags = osEventFlagsWait(xEvent_FatalErrorHandle, FATAL_ERROR_GPU, osFlagsWaitAny, osNoTimeout);

	if(fatalErrorFlags&FATAL_ERROR_GPU)
		return; /* Cancel the send */

	buffer[0] = acknowledge | confirmReset << 1;
	/* Getting the distance data */
	status = osMessageQueueGet(xFIFO_COMDistanceHandle, &distance, NULL, osNoTimeout);
	if(osOK != status)
	{
		emptyFIFOCounterDist++;
	}
	else
	{
		emptyFIFOCounterDist = 0;
	}

	/* Send Distance data */
	if(COM_FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterDist)
	{
		/* Send error code */
		buffer[1] = COM_MSG_ERROR_CODE;
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


	/* Getting the RPM data */
	status = osMessageQueueGet(xFIFO_COMRPMHandle, &RPM, NULL, osNoTimeout);
	if(osOK != status)
	{
		emptyFIFOCounterRPM++;
	}
	else
	{
		emptyFIFOCounterRPM = 0;
	}
	/* Send RPM data */
	if(COM_FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterRPM)
	{
		/* Send error code */
		buffer[3] = COM_MSG_ERROR_CODE;
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
	HAL_UART_Transmit_DMA(&huart1, buffer, COM_UART_PERIODIC_NUMBER_FRAMES);
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
	TaskSensorHandle = osThreadNew(vTaskSensor, NULL, &TaskSensor_attributes);
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
	static bool firstFrame = true;

	if(USART1 == huart -> Instance)
	{
		/* Communication with the daughterboard */
		if(firstFrame)
		{
			/* The first frame (init frame) has been sent */
			osSemaphoreRelease(xSemaphore_InitDaughterHandle);
			firstFrame = false;
		}
		else
		{
			/* Do Nothing */
		}
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
	static bool firstInit = true;
	uint8_t firstFrame = huart->pRxBuffPtr[0];

	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
	/* Reseting the Watch dog timer for UART */
	osTimerStart(xTimer_WdgUARTHandle, pdMS_TO_TICKS(COM_UART_PERIOD_FOR_DATA_TX));

	if(firstFrame == COM_UART_INIT_FRAME_VALUE)
	{
		/* It's an init frame */
		if(firstInit)
		{
			/* The system were waiting for this init */
			osSemaphoreRelease(xSemaphore_InitDaughterHandle);
			firstInit = false;
		}
		else
		{
			/* Watchdog timer were triggered on daughterboard */
			/* TODO: Handle here */
		}
	}
	else
	{
		/* It's a periodic frame */
		if(NO_MSG_ID != firstFrame)
		{
			/* Indicating that the component has to process the data */
			osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
		}
		else
		{
			/* Do Nothing, It's just the acknowledge */
		}
		HAL_UARTEx_ReceiveToIdle_IT(huart, UART_RxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES);
	}
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
