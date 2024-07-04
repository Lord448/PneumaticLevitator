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

void vTaskCOM(void *argument)
{
	COM_Init();
	for(;;)
	{

	}
}

/* PDU Constructor Function */
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

/*Approach for the COM message transmit, need to be analyzed and tested*/
/**
 * @brief  Send the message to the buffer
 * @return
 */
/*uint16_t COM_SendMessage(uint8_t MessageID, uint32_t Data)
{
	PDU_t pdu;
	pdu.rawData = Data;
	pdu.chunks[4] = MessageID;
	osMessageQueuePut(xFIFO_COMHandle, &pdu, 0U, 0U);
*/

	/*TODO*/
	/*See how charged is the FIFO*/
	/*Implement logic (DID's or DTC's) if FIFO overflow*/
	/*(Possible) Implement logic for increase FIFO buffer*/

//	return osMessageQueueGetCount(xFIFO_COMHandle); /*Number of messages charged on the FIFO*/
//}

