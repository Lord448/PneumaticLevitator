/*
 * COM.c
 *
 *  Created on: 29 may 2024
 *      Author: lord448
 */

#include "COM.h"

extern osMessageQueueId_t xFIFO_COMHandle;

void COM_Init(void)
{

}

void vTaskCOM_Runnable(void)
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

