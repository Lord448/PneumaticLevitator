/**
 * @file      COM.h
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

void vTaskCOM(void *argument);

PDU_t COM_CreatePDU(uint8_t MessageID, uint32_t Data);

uint16_t COM_SendMessage(uint8_t MessageID, uint32_t Data);

#endif /* SWC_COM_COM_H_ */
