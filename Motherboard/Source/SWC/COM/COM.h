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

void COM_Init(void);

void vTaskCOM_Runnable(void);
uint8_t COM_CreatePDU (PDU_t *pdu, uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload) ;
//uint16_t COM_SendMessage(uint8_t MessageID, uint32_t Data);

#endif /* SWC_COM_COM_H_ */
