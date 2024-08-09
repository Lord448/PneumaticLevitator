/**
 * @file      COM.c
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

#include "COM.h"

extern UART_HandleTypeDef huart1;

extern osMessageQueueId_t xFIFO_COMHandle;
extern osMessageQueueId_t xFIFO_COMDistanceHandle;
extern osMessageQueueId_t xFIFO_COMRPMHandle;

extern osTimerId_t xTimer_UARTSendHandle;
extern osTimerId_t xTimer_AckHandle;

extern osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorTxCpltHandle;
extern osSemaphoreId_t xSemaphore_SensorRxCpltHandle;
extern osSemaphoreId_t xSemaphore_AckHandle;
extern osSemaphoreId_t xSemaphore_InitDaughterHandle;

extern osEventFlagsId_t xEvent_USBHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;

extern char ResBuffer[64];

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sendInitBuffer(void);

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
	uint32_t usbFlags = 0;
	char statsBuffer[512] = {0};
	uint8_t buffer[6] = {0};
	uint8_t ack = 0;

	/* Send initial buffer */
	sendInitBuffer();
	HAL_UART_Receive_IT(&huart1, &ack, sizeof(uint8_t));
	osSemaphoreAcquire(xSemaphore_InitDaughterHandle, pdMS_TO_TICKS(SECONDS_TO_WAIT_DAUGHTER_INIT*1000)); /* Wait for daughter to init*/
	/* Eliminate the semaphore */
	osSemaphoreDelete(xSemaphore_InitDaughterHandle);
	/* Start soft-timer for UART send */
	osTimerStart(xTimer_UARTSendHandle, pdMS_TO_TICKS(10)); /* 10 Millisecond period */
	/* Set the RX UART bit (wait for IDLE bus state to process the ISR)*/
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, buffer, sizeof(buffer));
	for(;;)
	{
		/* Process USB info */
		usbFlags = osEventFlagsWait(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX, osFlagsWaitAny, osWaitForever);

		if(usbFlags&CDC_FLAG_MESSAGE_RX)
		{
			/* An USB message arrived */
			if(strcmp(ResBuffer, "CPU") == 0) /* TODO: Instrumented code */
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
		/* Process UART info */
		if(buffer[0] != 0)
		{
			/* An UART message has been received */

			buffer[0] = 0;
		}
	}
}

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
		if (MAX_QUEUE_MESSAGES > currentMessages) { 
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
 * @brief  Send through the UART Bus the "INIT TX BUS"
 * @param  none
 * @retval none
 */
static void sendInitBuffer(void)
{
	uint8_t buffer[16] = {0};
	NVMType32 Kp, Ki, Kd;
	uint32_t i = 1;

	NVM_Read(KP_PID_BASE_ADDR, &Kp);
	NVM_Read(KI_PID_BASE_ADDR, &Ki);
	NVM_Read(KD_PID_BASE_ADDR, &Kd);

	/* Building the buffer transmit */
	buffer[0] = UART_INIT_FRAME_VALUE;
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
	HAL_UART_Transmit_DMA(&huart1, buffer, 14);
}


/**
 * ---------------------------------------------------------
 * 					        SOFT-TIMERS CALLBACKS
 * ---------------------------------------------------------
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
	if(FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterDist)
	{
		/* Send error code */
		buffer[1] = MSG_ERROR_CODE;
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
	if(FIFO_EMPTY_COUNTS_FOR_ERROR < emptyFIFOCounterRPM)
	{
		/* Send error code */
		buffer[3] = MSG_ERROR_CODE;
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
	HAL_UART_Transmit_DMA(&huart1, buffer, UART_PERIODIC_NUMBER_FRAMES);
}

void vTimer_Ack_Callback(void *argument)
{
	/* Watchdog soft-timer */
	/* More than the allowed times */
	/* TODO: Trigger DTC Not GPU Comm */
	/* Reporting to LEDS */
	osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
	osTimerStop(xTimer_UARTSendHandle);
}


/**
 * ---------------------------------------------------------
 * 					    SOFTWARE COMPONENT CALLBACKS
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
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t firstInit = 1;
	uint8_t firstFrame = huart->pRxBuffPtr[0];

	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_GPU);
	osTimerStart(xTimer_AckHandle, pdMS_TO_TICKS(11)); /* Giving 1 millisecond of tolerance */
	if(firstFrame == UART_INIT_FRAME_VALUE)
	{
		/* Sending an init frame */
		if(firstInit)
		{
			/* The system were waiting for this init */
			osSemaphoreRelease(xSemaphore_InitDaughterHandle);
			firstInit = 0;
		}
		else
		{
			/* Watchdog timer trigger on daughterboard */
			/* TODO: Handle here */
		}
	}
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	if(I2C1 == hi2c->Instance)
		osSemaphoreRelease(xSemaphore_SensorRxCpltHandle); /* VL53L0X Read action completed */
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
		osSemaphoreRelease(xSemaphore_SensorRxCpltHandle); /* VL53L0X Write action completed */
}
