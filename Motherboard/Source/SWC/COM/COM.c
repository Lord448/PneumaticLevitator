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

