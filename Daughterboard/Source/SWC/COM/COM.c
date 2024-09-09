/**
 * @file      COM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component leads the communication with
 *            the Motherboard via UART and handles the reception of
 *            data, it could be init frame, periodical or on demand.
 *            In order to know the frame structure for each frame you
 *            can check the ODS file on the COM directory in the
 *            Motherboard project
 *
 * @date      Jul 3, 2024
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
extern TIM_HandleTypeDef htim3; /* Timer used for the UART transmission @ 10ms*/
extern TIM_HandleTypeDef htim4; /* Watchdog timer for UART @ 10ms (Count down)*/

extern osTimerId_t xTimer_UARTSendHandle;
extern osTimerId_t xTimer_WdgUARTHandle;

extern osMessageQueueId_t xFIFO_ControlConstantsHandle;
extern osMessageQueueId_t xFIFO_DistanceHandle;
extern osMessageQueueId_t xFIFO_RPMHandle;
extern osMessageQueueId_t xFIFO_COMHandle;
extern osMessageQueueId_t xFIFO_ActionControlHandle;
extern osMessageQueueId_t xFIFO_UISetPointHandle;

extern osSemaphoreId_t xSemaphore_InitMotherHandle;
extern osSemaphoreId_t xSemaphore_UARTTxCpltHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;

extern osEventFlagsId_t xEventFinishedInitHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;
extern osEventFlagsId_t xEvent_UARTSendTypeHandle;

uint8_t COM_UARTRxBuffer[COM_UART_INIT_NUMBER_FRAMES] = {0};
uint8_t UARTRXLocalBuffer[COM_UART_INIT_NUMBER_FRAMES] = {0};

#define firstFrame COM_UARTRxBuffer[0]

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static result_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);
static float buildFloatFromUART(uint8_t *data, uint8_t startIndex);
static void reSyncCom(bool *syncComInProcess);
static void sendCPULoad(char *statsBuffer);
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
	osStatus_t status;
	char statsBuffer[1024] = {0};
	bool syncComInProcess = false;
	int16_t distance, rpm, actionControl;
	int16_t setPoint;
	ControlConst controlConst;

	do {
		/* Waiting to receive the init frame of Motherboard */
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
		status = osSemaphoreAcquire(xSemaphore_InitMotherHandle, pdMS_TO_TICKS(COM_SECONDS_TO_WAIT_MOTHER_INIT*1000)); /* Wait for Mother to init*/
		if(osOK == status)
		{
			/* All Ok to go */
			uint8_t initFrame = COM_UART_INIT_FRAME_VALUE;
			/* Send the init frame */
			osEventFlagsSet(xEvent_UARTSendTypeHandle, INIT_FRAME_MESSAGE_TYPE);
			HAL_UART_Transmit_DMA(&huart1, &initFrame, sizeof(uint8_t));
			/* Set the RX UART bit (wait for IDLE bus state to process the ISR)*/
			osSemaphoreAcquire(xSemaphore_UARTTxCpltHandle, osWaitForever);
			/* Start hard-timer for UART send */
			HAL_TIM_Base_Start_IT(&htim3);
			/* Start hard-watchdog timer for UART receive */
			TIM3 -> CNT = htim4.Init.Period;
			HAL_TIM_Base_Start_IT(&htim4);
			/* Getting the constants data */
			if(COM_UART_INIT_FRAME_VALUE == firstFrame)
			{
				/* The first frame was sent */
				controlConst.kp = buildFloatFromUART(COM_UARTRxBuffer, 1);
				controlConst.ki = buildFloatFromUART(COM_UARTRxBuffer, 5);
				controlConst.kp = buildFloatFromUART(COM_UARTRxBuffer, 9);
				osMessageQueuePut(xFIFO_ControlConstantsHandle, &controlConst, 0U, osNoTimeout);
			}
			else
			{
				/* Didn't received the constants */
			}
		}
		else
		{
			/* Timeout, asumming errors on the Motherboard init */
			/* Reporting to LEDS SWC */
			/* osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);*/ /* Disabled feature */
			/* Poll the task and wait for a response of the mother */
			osDelay(pdMS_TO_TICKS(500));
		}
	}while(osOK != status);

	for(;;)
	{
		osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
		osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
		/* An UART message has been received */
		switch(firstFrame)
		{
			case COM_UART_INIT_FRAME_VALUE:
				/* An initial frame has been received when it's not supposed to */
				controlConst.kp = buildFloatFromUART(UARTRXLocalBuffer, 1);
				controlConst.ki = buildFloatFromUART(UARTRXLocalBuffer, 5);
				controlConst.kd = buildFloatFromUART(UARTRXLocalBuffer, 9);
				osMessageQueuePut(xFIFO_ControlConstantsHandle, &controlConst, 0U, osNoTimeout);
				syncComInProcess = true;
				osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
				while(syncComInProcess)
					reSyncCom(&syncComInProcess);
			break;
			case CPU_LOAD_DAUGHTER:
				/* Motherboard requesting single CPU Load measure */
				sendCPULoad(statsBuffer);
			break;
			case ENTER_DIAGS_ID:
				/* Enter on diagnostics session */
			break;
			case PERIODIC_ID:
				/* Normal periodical frame */
				/* Decoding the message */
				distance = COM_UARTRxBuffer[1] | (COM_UARTRxBuffer[2]<<8);
				rpm = COM_UARTRxBuffer[3] | (COM_UARTRxBuffer[4]<<8);
				actionControl = COM_UARTRxBuffer[5];
				/* Sending to the FIFOs */
				osMessageQueuePut(xFIFO_DistanceHandle, &distance, 0U, osNoTimeout);
				osMessageQueuePut(xFIFO_RPMHandle, &rpm, 0U, osNoTimeout);
				osMessageQueuePut(xFIFO_ActionControlHandle, &actionControl, 0U, osNoTimeout);
			break;
			case SET_POINT:
				/* Changing set point */
				setPoint = COM_UARTRxBuffer[1] | (COM_UARTRxBuffer[2]<<8);
				osMessageQueuePut(xFIFO_UISetPointHandle, &setPoint, 0U, osNoTimeout);
			default:
				/* Unknown message, Do Nothing */
			break;
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
 * @return		result of the operation
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
			osMessageQueuePut(xFIFO_COMHandle, &pdu, 0U, osNoTimeout);
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
 * @brief  This function build the float number
 *         provided by the motherboard via UART
 * @param  *data : data buffer (amount of 32 bits)
 * @param  startIndex : Index of the buffer where the number
 *                      starts
 * @retval The floating point number received by the UART port
 */
static float buildFloatFromUART(uint8_t *data, uint8_t startIndex)
{
	union COM32Type
	{
		float result;
		uint8_t rawData[4];
	}COM32Type;

	for(uint16_t i = 0; i < sizeof(COM32Type); i++, startIndex++)
	{
		/* Filling the float data */
		COM32Type.rawData[i] = data[startIndex];
	}
	return COM32Type.result;
}

/**
 * @brief  This function try to resyncroinze the communication
 *         with the Motherboard, responding the init frame so
 *         the motherboard can start to send the periodic frames
 * @note   this function it's called when the UART bus detects that
 *         the motherboard send the initial frame
 * @param  *syncComInProcess : notifies to the external scope if the
 *                             MCUs are syncronized
 * @retval none
 */
static void reSyncCom(bool *syncComInProcess)
{
	enum ReSyncStates {
		StopTimer,
		SendInitFrame,
		RestoreTimer
	}static stateHandler = StopTimer;
	const uint8_t initFrame = COM_UART_INIT_FRAME_VALUE;

	switch(stateHandler)
	{
		case StopTimer:
			/* First state of the machine */
			HAL_TIM_Base_Stop_IT(&htim3);
			stateHandler = SendInitFrame;
		break;
		case SendInitFrame:
			/* Send init frame until the Motherboard gets the info*/
			osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
			if(COM_UART_INIT_FRAME_VALUE != firstFrame)
			{
				/* The Motherboard started to send another type of frames */
				stateHandler = RestoreTimer;
			}
			else
			{
				/* The Motherboard still sends init frames */
				HAL_UART_Transmit_DMA(&huart1, &initFrame, sizeof(uint8_t));
			}
		break;
		case RestoreTimer:
			*syncComInProcess = false; /* Break the upper while */
			osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
			HAL_TIM_Base_Start_IT(&htim3);
		break;
	}
}

/**
 * @brief  Send the CPU load statistics provided by the freeRTOS kernel
 * @param  *statsBuffer : The buffer where the data it's allocated
 * @retval none
 */
static void sendCPULoad(char *statsBuffer)
{
	/* Enter on small diagnostics session */
	HAL_TIM_Base_Stop_IT(&htim3);
	/* The Motherboard has requested a CPU load measure */
	memset(statsBuffer, '\0', strlen(statsBuffer));
	vTaskGetRunTimeStats(statsBuffer);
	strcat(statsBuffer, "\nend\n"); /* Data specific for CPULoad script */
	/* Send all the data */
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)statsBuffer, strlen(statsBuffer));
	/* Wait to end transmission */
	osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
	/* End small diagnostics session */
	HAL_TIM_Base_Start_IT(&htim3);
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
	const uint8_t noMSGData = NO_MSG_ID;
	static uint8_t bufferTX[COM_UART_PERIODIC_NUMBER_FRAMES_TX] = {0};
	volatile uint8_t *bytePointer;
	PDU_t dataTX;

	if(TIM3 == htim -> Instance)
	{
		/* Time to send a UART transmission */
		if(0 == osMessageQueueGetCount(xFIFO_COMHandle))
		{
			/* The FIFO is empty */
			HAL_UART_Transmit_DMA(&huart1, &noMSGData, sizeof(uint8_t));
		}
		else
		{
			/* The FIFO has data */
			osStatus_t status = osMessageQueueGet(xFIFO_COMHandle, &dataTX, NULL, osNoTimeout);
			if(osOK == status)
			{
				/* Building the buffer for transmission */
				bytePointer = (uint8_t *)&dataTX.fields.payload;
				bufferTX[0] = ACK_DATA | dataTX.fields.messageID;
				for(uint16_t i = 1; i < sizeof(uint32_t)+1; i++, bytePointer++)
				{
					bufferTX[i] = *bytePointer;
				}
				/* Buffer ready for transmission */
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)bufferTX, COM_UART_PERIODIC_NUMBER_FRAMES_TX);
			}
			else
			{
				/* Errors on the FIFO, sending no message */
				HAL_UART_Transmit_DMA(&huart1, &noMSGData, sizeof(uint8_t));
			}
		}
	}
	else if(TIM4 == htim -> Instance)
	{
		/* Watch dog timer for UART */
		/* Reporting to LEDS SWC */
		osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
	}
}
/**
 * ---------------------------------------------------------
 * 					       ISR COMPONENT CALLBACKS
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
	uint32_t flags = 0;

	flags = osEventFlagsGet(xEvent_UARTSendTypeHandle);
	if(flags&CPU_LOAD_MESSAGE_TYPE)
	{
		/* A CPU load measure has been sent */
		osEventFlagsClear(xEvent_UARTSendTypeHandle, CPU_LOAD_MESSAGE_TYPE);
		osSemaphoreRelease(xSemaphore_UARTTxCpltHandle);
	}
	else if(flags&INIT_FRAME_MESSAGE_TYPE)/* Flag for the special meesage type */
	{
		/* A first frame has been sent */
		osEventFlagsClear(xEvent_UARTSendTypeHandle, INIT_FRAME_MESSAGE_TYPE);
		osSemaphoreRelease(xSemaphore_UARTTxCpltHandle);
	}
	else
	{
		/* Do Nothing, Normal periodical send */
	}
}

/**
  * @brief  Rx Half Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	const uint8_t initFrame = COM_UART_INIT_FRAME_VALUE;
	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
	if(COM_UART_INIT_FRAME_VALUE == firstFrame)
	{
		/* Send init frame */
		HAL_UART_Transmit_DMA(&huart1, &initFrame, sizeof(uint8_t));
	}
	else
	{
		/* Do Nothing */
	}
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	static bool firstBoot = true;
	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
	/* Reseting the Watch dog timer for UART */
	TIM3 -> CNT = htim4.Init.Period;
	if(firstBoot)
	{
		/* It's the first frame received */
		osSemaphoreRelease(xSemaphore_InitMotherHandle);
		firstBoot = false;
	}
	else
	{
		/* Needs to be handled on COM */
		if(COM_UART_INIT_FRAME_VALUE == firstFrame)
		{
			/* @note
			 * Can't wait to the thread to process the buffer, copying the
			 * buffer on another in order to avoid data overrun or overwrite
			 * on the buffer
			 */
			memset(UARTRXLocalBuffer, '\0', sizeof(UARTRXLocalBuffer));
			memcpy(UARTRXLocalBuffer, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
		}
		osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
	}
	/* Continue expecting receiveing signals */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
}

/**
  * @brief  UART error callbacks.
  * @note   This function were implemented only for debug porpouses
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
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
}

