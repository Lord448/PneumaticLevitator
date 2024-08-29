/**
 * @file      COM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
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

extern osSemaphoreId_t xSemaphore_InitMotherHandle;
extern osSemaphoreId_t xSemaphore_UARTTxCpltHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;

extern osEventFlagsId_t xEventFinishedInitHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;
extern osEventFlagsId_t xEvent_UARTSendTypeHandle;

uint8_t COM_UARTRxBuffer[COM_UART_INIT_NUMBER_FRAMES] = {0};

#define firstFrame COM_UARTRxBuffer[0]

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static result_t sCOM_ParseFloatConstants(char *str, float *kp, float *ki, float *kd);
static result_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);
static void buildFloatFromUART(uint8_t *data, float *number, uint8_t startIndex);
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
	static ControlConst controlConst;
	char UARTRXLocalBuffer[COM_UART_INIT_NUMBER_FRAMES] = {0};
	uint16_t datalen;

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
				//buildFloatFromUART(COM_UARTRxBuffer, &kp, 1);
				//buildFloatFromUART(COM_UARTRxBuffer, &controlConst.ki, 5);
				//buildFloatFromUART(COM_UARTRxBuffer, &controlConst.kp, 9);
				osMessageQueuePut(xFIFO_ControlConstantsHandle, &controlConst, 0U, osNoTimeout);
			}
			else
			{
				/* Do nothing */
			}
		}
		else
		{
			/* Timeout, asumming errors on the Motherboard init */
			/* Reporting to LEDS SWC */
			osEventFlagsSet(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
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
				/* Copy the data of the frame */
				datalen = strlen((char *)COM_UARTRxBuffer);
				memset(UARTRXLocalBuffer, '\0', sizeof(UARTRXLocalBuffer));
				memcpy(UARTRXLocalBuffer, COM_UARTRxBuffer, datalen);
				memset(COM_UARTRxBuffer, 0, sizeof(COM_UARTRxBuffer));
				if(OK == sCOM_ParseFloatConstants((char *)UARTRXLocalBuffer, &controlConst.kp, &controlConst.ki, &controlConst.kd))
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
				/* Normal perdiocal frame */
				/* TODO handle here the distance and RPM */
				/* Decoding the message */
				distance = COM_UARTRxBuffer[1] | (COM_UARTRxBuffer[2]<<8);
				rpm = COM_UARTRxBuffer[3] | (COM_UARTRxBuffer[4]<<8);
				actionControl = COM_UARTRxBuffer[5];
				/* Sending to the FIFOs */
				/* TODO: Add logic for FIFO reseting when overflow */
				osMessageQueuePut(xFIFO_DistanceHandle, &distance, 0U, osNoTimeout);
				osMessageQueuePut(xFIFO_RPMHandle, &rpm, 0U, osNoTimeout);
				osMessageQueuePut(xFIFO_ActionControlHandle, &actionControl, 0U, osNoTimeout);
			break;
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

static result_t sCOM_ParseFloatConstants(char *str, float *kp, float *ki, float *kd)
{
	const char endOfFrameChar = '!';
	result_t retval = Error;
	uint16_t datalen = strlen(str);
	bool stringIsCorrect = false;
	bool kpErrorFlag = false, kiErrorFlag = false, kdErrorFlag = false;
	char kpBuf[16] = "";
	char kiBuf[16] = "";
	char kdBuf[16] = "";

	/* Search for end of frame char */
	for(int16_t i = datalen; i > 0; i--)
	{
		if(str[i] == endOfFrameChar)
		{
			/* String received correctly */
			stringIsCorrect = true;
			break;
		}
	}
	/* Processing the data */
	if(stringIsCorrect)
	{
		/* Can process the data */
		for(uint16_t i = 0; i < datalen; i++)
		{
			/* Searching through all the buffer */
			if(str[i] == 'P')
			{
				/* P constant found */
				i++;
				for(uint16_t j = i, bufIndex = 0; j < datalen; j++, bufIndex++)
				{
					/* Filling P constant */
					if(str[j] != 'I')
					{
						/* Can fill the buffer */
						kpBuf[bufIndex] = str[j];
					}
					else
					{
						/* End of P constant */
						i = j-1;
						break;
					}
				}
			}
			else if(str[i] == 'I')
			{
				/* I constant found */
				i++;
				for(uint16_t j = i, bufIndex = 0; j < datalen; j++, bufIndex++)
				{
					/* Filling I constant */
					if(str[j] != 'D')
					{
						/* Can fill the buffer */
						kiBuf[bufIndex] = str[j];
					}
					else
					{
						/* End of I constant */
						i = j-1;
						break;
					}
				}
			}
			else if(str[i] == 'D')
			{
				/* D constant found */
				i++;
				for(uint16_t j = i, bufIndex = 0; j < datalen; j++, bufIndex++)
				{
					/* Filling D constant */
					if(str[j] != '!')
					{
						/* Can fill the buffer */
						kdBuf[bufIndex] = str[j];
					}
					else
					{
						/* End of D constant */
						i = j-1;
						break;
					}
				}
			}
			else
			{
				/* Do Nothing */
			}
		}
		/* Parse the float numbers */
		if(EOF == sscanf(kpBuf, "%f", kp))
		{
			/* Error on the parse */
			*kp = (float)EOF;
			COM_SendMessage(REQUEST_KP, MSG_TYPE_ONDEMAND, PRIORITY_HIGH, 0);
		}
		if(EOF == sscanf(kiBuf, "%f", ki))
		{
			/* Error on the parse */
			*ki = (float)EOF;
			COM_SendMessage(REQUEST_KI, MSG_TYPE_ONDEMAND, PRIORITY_HIGH, 0);
		}
		if(EOF != sscanf(kdBuf, "%f", kd))
		{
			/* Error on the parse */
			*kd = (float)EOF;
			COM_SendMessage(REQUEST_KD, MSG_TYPE_ONDEMAND, PRIORITY_HIGH, 0);
		}
		if(kpErrorFlag && kiErrorFlag && kdErrorFlag)
		{
			/* Non of the constants were parsed correctly */
			retval = Error;
		}
		else
		{
			/* At least one of the constants were parsed correctly */
			retval = OK;
		}
	}
	else
	{
		/* String came with errors, request the init frame */
		COM_SendMessage(REQUEST_CONST, MSG_TYPE_ONDEMAND, PRIORITY_HIGH, 0);
		retval = Error;
	}
	return retval;
}
typedef union COM32Type
	{
		float result;
		uint32_t data32;
		uint8_t rawData[4];
	}COM32Type;
/**
 * @brief
 * @param
 * @retval none
 */
static void buildFloatFromUART(uint8_t *data, float *number, uint8_t startIndex)
{
	/* TODO Unscalable code, fix this with PDU handling */

	/*
	TODO: Old algorithm, it's uncomented due to memory copy issues for the union
	float res;
	COM32Type data32type;
	COM32Type data32type2;
	for(uint16_t i = 0; i < sizeof(COM32Type); i++, startIndex++)
	{
		/* Filling the float data
		data32type.rawData[i] = data[startIndex];
	}
	memcpy((uint8_t *)&res, data32type.rawData, sizeof(float));

	data32type2.result = data32type.result;

	res = (float)data32type.result;
	//res = (float) 100.32;
	*number = data32type.result;
	*/
	COM32Type data32type;
	int32_t integerData;
	int32_t fraccData;
	for(uint16_t i = 0; i < sizeof(COM32Type); i++, startIndex++)
	{
		/* Filling the float data */
		data32type.rawData[i] = data[startIndex];
	}
	/* Moving data to res */
	integerData = (uint32_t) data32type.result;
	fraccData = (uint32_t) (data32type.result-integerData ) * 1000000;
}

/**
 * @brief
 * @param
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
 * @brief
 * @param
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
 * 					        SOFT-TIMERS CALLBACKS
 * ---------------------------------------------------------
 */
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
	else if(flags&INIT_FRAME_MESSAGE_TYPE)
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
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
		osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
	}
	/* Continue expecting receiveing signals */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
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
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
}

