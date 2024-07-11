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

extern osMessageQueueId_t xFIFO_COMHandle;

void COM_Init(void)
{

}

void vTaskCOM(void *argument)
{
	COM_Init();
	for(;;)
	{

	}
}

PDU_t COM_CreatePDU(uint8_t MessageID, uint32_t Data)
{
	PDU_t result;
	result.rawData = Data;
	result.chunks[4] = MessageID;
	return result;
}

/*Approach for the COM message transmit, need to be analyzed and tested*/
/**
 * @brief  Send the message to the buffer
 * @return
 */
uint16_t COM_SendMessage(uint8_t MessageID, uint32_t Data)
{
	PDU_t pdu;
	pdu.rawData = Data;
	pdu.chunks[4] = MessageID;
	osMessageQueuePut(xFIFO_COMHandle, &pdu, 0U, 0U); /*Put the PDU on the FIFO*/

	/*TODO*/
	/*See how charged is the FIFO*/
	/*Implement logic (DID's or DTC's) if FIFO overflow*/
	/*(Possible) Implement logic for increase FIFO buffer*/

	return osMessageQueueGetCount(xFIFO_COMHandle); /*Number of messages charged on the FIFO*/
}

