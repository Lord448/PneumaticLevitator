/**
 * @file      COM.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jul 3, 2024
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
#include "cmsis_os.h"
#include "Signals.h"
#include "MainMenu.h"
#include "stm32f4xx_hal_uart.h"
#include <stdio.h>
#include <string.h>

/* Used for the send message logic */
#define COM_MAX_QUEUE_MESSAGES 16

/* Number of frames of the initial messages (for reception)*/
#define COM_UART_INIT_NUMBER_FRAMES 49
/* Number of frames of the periodic messages (for reception)*/
#define COM_UART_PERIODIC_NUMBER_FRAMES_RX 6
/* Number of frames of the periodic messages (for transmission) */
#define COM_UART_PERIODIC_NUMBER_FRAMES_TX 5
/* Number that the MCU will send to identify the initial frame */
#define COM_UART_INIT_FRAME_VALUE 0xA5
/* Time of the send of the periodical frames */
#define COM_UART_PERIOD_FOR_DATA_TX 10 /*MS*/

/* Error code for the RPM & distance Frames (could not get the value)*/
#define COM_MSG_ERROR_CODE (int16_t) -1
/* Number of counts to assume errors on the FIFO get for distance & RPM */
#define COM_FIFO_EMPTY_COUNTS_FOR_ERROR 10

/* Seconds to wait for mark the Mother Comm as an error*/
#define COM_SECONDS_TO_WAIT_MOTHER_INIT 1

/* Count of sequenced errors to cancel the comm with VL53L0X */
#define COM_ERROR_COUNTS_TO_CANCEL_VL53L0X_TX 20
/* Time to wait to reset the sensor task */
#define COM_TIME_TO_RESET_SENSOR_TASK 1000 /*MS*/

/* Flag for the special meesage type */
#define CPU_LOAD_MESSAGE_TYPE (uint32_t)   1U << 0
/* Flag for the init frame */
#define INIT_FRAME_MESSAGE_TYPE (uint32_t) 1U << 1

#define KP_INDEX 1
#define KI_INDEX 2
#define KD_INDEX 3

typedef struct ControlConstants {
	float kp;
	float ki;
	float kd;
}ControlConst;

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

int16_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);

void vTaskCOM(void *argument);
void vTimer_UARTSendCallback(void *argument);

#endif /* SWC_COM_COM_H_ */
