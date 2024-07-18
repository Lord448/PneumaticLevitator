/**
 * @file      DiagAppl.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jun 2, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

/*
 	 	 	 	 	        GENERAL DIAGNOSTICS FRAME
 	 	 	 	 	 	 	 	 	     (BASED ON  UDS)
  |------------------|----------------|---------------|
  |	       PCI       |       SID      |       RPD     |
  | Protocol Control |  Service Data  |  Request Data |
  |    Information   |   Identifier   |   Parameters  |
  |      (0xAA)      |                |               |
	|------------------|----------------|---------------|
*/

#include "DiagAppl.h"

extern osMessageQueueId_t xFIFO_DiagsLongHandle;
extern osMessageQueueId_t xFIFO_DiagShortHandle;
extern osEventFlagsId_t xEventDiagnosticsHandle;

static result_t sendNegativeResponse(void);

static result_t ProcessDID32(uint8_t SID, uint32_t data);
static result_t ProcessDID64(uint8_t SID, uint64_t data);
static result_t ProcessDIDCompound(uint8_t SID, uint8_t *buffer);

static result_t READ_DATA_BY_ID_DID(uint8_t DID, uint32_t buffer);

void DiagAppl_Init(void)
{

}

void vTaskDiagAppl(void *argument)
{
	uint32_t DiagFlags = 0;

	DiagAppl_Init();
	for(;;)
	{
		/* Wait for a FIFO diagnostic request */
		DiagFlags = osEventFlagsWait(xEventDiagnosticsHandle, osFlagsWaitAny, 0U, osWaitForever);
		/*Processing Flags*/
		while(0 != DiagFlags)
		{
			if(DiagFlags&PCI_SINGLE_STREAM_32BIT)
			{
				/*It's PCI_SINGLE_STREAM_32BIT*/
				PDU_t diagPDU;

				osMessageQueueGet(xFIFO_DiagShortHandle, &diagPDU, 0U, QUEUE_GET_TIMEOUT);
				ProcessDID32(diagPDU.SID_D, diagPDU.fields.payload);
				if(0 == osMessageQueueGetCount(xFIFO_DiagShortHandle))
				{
					/*Clearing flags*/
					osEventFlagsClear(xEventDiagnosticsHandle, DiagFlags);
				}
				else
				{
					/* Do Nothing, process the rest of the FIFO*/
				}
			}
			else if(DiagFlags&PCI_SINGLE_STREAM_64BIT)
			{
				/*It's PCI_SINGLE_STREAM_32BIT*/
				DiagPDU_t diagPDU;

				osMessageQueueGet(xFIFO_DiagsLongHandle, &diagPDU, 0U, QUEUE_GET_TIMEOUT);
				ProcessDID64(diagPDU.fields.SID, diagPDU.fields.payload);
				if(0 == osMessageQueueGetCount(xFIFO_DiagsLongHandle))
				{
					/*Clearing flags*/
					osEventFlagsClear(xEventDiagnosticsHandle, DiagFlags);
				}
				else
				{
					/* Do Nothing, process the rest of the FIFO*/
				}
			}
			else if(DiagFlags&PCI_COMPOUND_STREAM_64BIT)
			{
				/*It's PCI_SINGLE_STREAM_32BIT*/

				uint32_t Flag;

				/*Waiting for the end flag*/
				Flag = osEventFlagsWait(xEventDiagnosticsHandle, PCI_COMPOUND_END_STREAM, 0U, osWaitForever);
				/*Allocate memory pool with the data*/
				/*Send to pointer of the buffer to the process function*/
				ProcessDIDCompound(0, NULL);
				/*Free the memory pool*/
				if(Flag&PCI_COMPOUND_END_STREAM)
				{
					/*It's the selected flag*/
					osEventFlagsClear(xEventDiagnosticsHandle, DiagFlags&PCI_COMPOUND_STREAM_64BIT);
				}
				else
				{
					/*Not finished yet*/
				}
			}
		}
	}
}

static result_t sendNegativeResponse(void)
{
	return OK;
}

/*
 *-----------------------------------------
 *              32 BIT DIDs
 *-----------------------------------------
 */
static result_t ProcessDID32(uint8_t SID, uint32_t data)
{
	result_t result = OK;
	uint8_t ByteData;

	switch(SID)
	{
		case SERVICE_SESSION_CONTROL_REQUEST:
		break;
		case SERVICE_ECU_RESET_REQUEST:
		break;
		case SERVICE_COMMUNICATION_CONTROL_REQUEST:
		break;
		case SERVICE_READ_DATA_BY_ID_REQUEST:
			/*Data Acquire*/
			ByteData = (uint8_t)data;
			if(OK != READ_DATA_BY_ID_DID(ByteData, data))
			{
				/*Send negative response*/
				sendNegativeResponse();
			}
			else
			{
				/*Do Nothing, Internally send the data via USB*/
			}

		break;
		case SERVICE_READ_MEMORY_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_CLEAR_DTCS_REQUEST:
		break;
		case SERVICE_READ_DTC_INFORMATION_REQUEST:
		break;
		default:
			/*Do nothing*/
		break;
	}
	return result;
}

static result_t READ_DATA_BY_ID_DID(uint8_t DID, uint32_t buffer)
{
	result_t result = OK;
	switch(DID)
	{
		case READ_MEMORY_DUMP:
		break;
		case READ_PID_CONSTANTS:
		break;
		case READ_P_CONSTANT:
		break;
		case READ_I_CONSTANT:
		break;
		case READ_D_CONSTANT:
		break;
		default:
		break;
	}
	return result;
}
/*
 *-----------------------------------------
 *              64 BIT DIDs
 *-----------------------------------------
 */
static result_t ProcessDID64(uint8_t SID, uint64_t data)
{
	result_t result = OK;
	switch(SID)
	{
		case SERVICE_SESSION_CONTROL_REQUEST:
		break;
		case SERVICE_ECU_RESET_REQUEST:
		break;
		case SERVICE_COMMUNICATION_CONTROL_REQUEST:
		break;
		case SERVICE_READ_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_READ_MEMORY_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_CLEAR_DTCS_REQUEST:
		break;
		case SERVICE_READ_DTC_INFORMATION_REQUEST:
		break;
		default:
			/*Do nothing*/
		break;
	}
	return result;
}

static result_t ProcessDIDCompound(uint8_t SID, uint8_t *buffer)
{
	result_t result = OK;
	switch(SID)
	{
		case SERVICE_SESSION_CONTROL_REQUEST:
		break;
		case SERVICE_ECU_RESET_REQUEST:
		break;
		case SERVICE_COMMUNICATION_CONTROL_REQUEST:
		break;
		case SERVICE_READ_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_READ_MEMORY_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_CLEAR_DTCS_REQUEST:
		break;
		case SERVICE_READ_DTC_INFORMATION_REQUEST:
		break;
		default:
			/*Do nothing*/
		break;
	}
	return result;
}

/*
 TODO Erase when finished
 	switch(SID)
	{
		case SERVICE_SESSION_CONTROL_REQUEST:
		break;
		case SERVICE_ECU_RESET_REQUEST:
		break;
		case SERVICE_COMMUNICATION_CONTROL_REQUEST:
		break;
		case SERVICE_READ_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_READ_MEMORY_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ID_REQUEST:
		break;
		case SERVICE_WRITE_DATA_BY_ADDRESS_REQUEST:
		break;
		case SERVICE_CLEAR_DTCS_REQUEST:
		break;
		case SERVICE_READ_DTC_INFORMATION_REQUEST:
		break;
	}
 */












