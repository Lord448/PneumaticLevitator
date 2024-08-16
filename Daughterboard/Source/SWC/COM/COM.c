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

extern osMessageQueueId_t xFIFO_UARTDataTXHandle;

extern osSemaphoreId_t xSemaphore_InitMotherHandle;
extern osSemaphoreId_t xSemaphore_UARTRxCpltHandle;

extern osEventFlagsId_t xEventFinishedInitHandle;
extern osEventFlagsId_t xEvent_FatalErrorHandle;
extern osEventFlagsId_t xEvent_UARTSendTypeHandle;

uint8_t COM_UARTRxBuffer[COM_UART_INIT_NUMBER_FRAMES] = {0};

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
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
	char statsBuffer[512] = {0};

	/* Waiting to receive the init frame of Motherboard */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, COM_UARTRxBuffer, COM_UART_INIT_NUMBER_FRAMES);
	do {
		status = osSemaphoreAcquire(xSemaphore_InitMotherHandle, pdMS_TO_TICKS(COM_SECONDS_TO_WAIT_MOTHER_INIT*1000)); /* Wait for Mother to init*/
		if(osOK == status)
		{
			/* All Ok to go */
			uint8_t initFrame = COM_UART_INIT_FRAME_VALUE;
			/* Send the init frame */
			HAL_UART_Transmit_DMA(&huart1, &initFrame, sizeof(uint8_t));
			/* Set the RX UART bit (wait for IDLE bus state to process the ISR)*/
			HAL_UARTEx_ReceiveToIdle_IT(&huart1, COM_UARTRxBuffer, sizeof(COM_UARTRxBuffer));
			/* Free the memory reserved by the UART Semaphore */
			osSemaphoreDelete(xSemaphore_InitMotherHandle);
			/* Start hard-timer for UART send */
			HAL_TIM_Base_Start_IT(&htim3);
			/* Start hard-watchdog timer for UART receive */
			TIM3 -> CNT = htim4.Init.Period;
			HAL_TIM_Base_Start_IT(&htim4);
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
		if(CPU_LOAD_DAUGHTER == COM_UARTRxBuffer[0])
		{
			/* Enter on diagnostics session */
			HAL_TIM_Base_Stop_IT(&htim3);
			/* The Motherboard has requested a CPU load measure */
			memset(statsBuffer, '\0', strlen(statsBuffer));
			vTaskGetRunTimeStats(statsBuffer);
			strcat(statsBuffer, "\nend\n"); /* Data specific for CPULoad script */
			/* Send all the data */
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)statsBuffer, strlen(statsBuffer));
			/* Wait to end transmission */
			osSemaphoreAcquire(xSemaphore_UARTRxCpltHandle, osWaitForever);
			/* End diagnostics session */
			HAL_TIM_Base_Start_IT(&htim3);
		}
		else
		{
			/* Unknown message, Do Nothing */
		}
	}
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
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
	uint8_t bufferTX[COM_UART_PERIODIC_NUMBER_FRAMES] = {0};
	PDU_t dataTX;

	if(TIM3 == htim -> Instance)
	{
		/* Time to send a UART transmission */
		if(0 != osMessageQueueGetCount(xFIFO_UARTDataTXHandle))
		{
			/* The FIFO is empty */
			HAL_UART_Transmit_DMA(&huart1, &noMSGData, sizeof(uint8_t));
		}
		else
		{
			/* The FIFO has data */
			osStatus_t status = osMessageQueueGet(xFIFO_UARTDataTXHandle, &dataTX, NULL, osNoTimeout);
			if(osOK == status)
			{
				/* Building the buffer for transmission */
				bufferTX[0] = ACK_DATA | dataTX.fields.messageID;
				for(uint16_t i = 1; i < sizeof(uint32_t)+1; i++)
				{
					bufferTX[i] = dataTX.rawData[i+1];
				}
				/* Buffer ready for transmission */
				HAL_UART_Transmit_DMA(&huart1, bufferTX, COM_UART_PERIODIC_NUMBER_FRAMES);
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
		osSemaphoreRelease(xSemaphore_UARTRxCpltHandle);
	}
	else
	{
		/* Do Nothing, Normal periodical send */
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

	osEventFlagsClear(xEvent_FatalErrorHandle, FATAL_ERROR_MOTHER_COMM);
	/* Reseting the Watch dog timer for UART */
	TIM3 -> CNT = htim4.Init.Period;

	if(firstFrame == COM_UART_INIT_FRAME_VALUE)
	{
		/* It's an init frame */
		if(firstInit)
		{
			/* The system were waiting for this init */
			osSemaphoreRelease(xSemaphore_InitMotherHandle);
			firstInit = false;
		}
		else
		{
			/* Watchdog timer were triggered on Motherboard */
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
	}
	HAL_UARTEx_ReceiveToIdle_IT(huart, COM_UARTRxBuffer, COM_UART_PERIODIC_NUMBER_FRAMES);
}

