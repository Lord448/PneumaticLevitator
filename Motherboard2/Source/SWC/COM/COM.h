/*
 * COM.h
 *
 *  Created on: 29 may 2024
 *      Author: lord448
 */

#ifndef SWC_COM_COM_H_
#define SWC_COM_COM_H_

#include "main.h"
#include "cmsis_os2.h"

/*Last Chunk is the message ID*/
typedef union PDU_t{
	uint32_t rawData;
	uint8_t chunks[5];
}PDU_t;

void COM_Init(void);

void vTaskCOM_Runnable(void);

PDU_t COM_CreatePDU(uint8_t MessageID, uint32_t Data);

void COM_SetMessage(uint8_t MessageID, uint32_t Data);

#endif /* SWC_COM_COM_H_ */
