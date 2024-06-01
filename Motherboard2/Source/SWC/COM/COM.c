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
