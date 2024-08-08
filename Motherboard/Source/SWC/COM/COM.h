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
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>

/* TODO TEMPORAL INCLUDES */
#include "FreeRTOS.h"
#include "task.h"
/* TODO TEMPORAL INCLUDES */

#define MAX_QUEUE_MESSAGES	16

/* Definition of message types and priority (ensuring 4 bits) */

typedef enum {
    MSG_TYPE_PERIODIC = 0,
    MSG_TYPE_ONDEMAND,
    MSG_TYPE_CONTROL,
    MSG_TYPE_GATEWAY,
    MSG_TYPE_MAX = 15  /* Limit to 4 bits */
} MessageType;

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH,
    PRIORITY_CRITICAL,
    PRIORITY_MAX = 15  /* Limit to 4 bits */
} PriorityType;

/* 48-bit PDU union */
typedef union {
    struct {
        uint8_t messageID;          /* 8 bits for message ID */
        uint8_t messageType : 4;    /* 4 bits for message type */
        uint8_t priority : 4;       /* 4 bits for priority */
        uint32_t payload;           /* 32 bits for the message content */
    } fields;

    uint8_t rawData[6];             /* Access as byte array */
} PDU_t;

/* 80-bit Diagnostics PDU Union */
typedef union DiagPDU{
	struct PDUFields{
		uint8_t PDU_ID;    /* 8 bits for the PDU control information */
		uint8_t SID;       /* 8 bits for the service data identifier */
		uint64_t payload;  /* 64 bits for the message content */
	}fields;

	uint8_t rawData[10]; /* Access as byte array */
} DiagPDU_t;

void vTaskCOM(void *argument);
uint8_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);
int16_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);

#endif /* SWC_COM_COM_H_ */
