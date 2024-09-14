/**
 * @file      COM.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This software component leads the all the
 *            communication ports on the Motherboard, UART
 *            for the communication with the Daughterboard,
 *            USB for the communication with Matlab or any PC
 *            program (Arduino monitor can be used to).
 *
 *            This component implements two subthreads in order to
 *            make a fast process and response for the USB messages
 *            as well as the on demand UART messages, and save CPU
 *            load charge by polling this tasks until there's a
 *            message to process.
 *
 *            This component implements soft-timers with the help
 *            of the kernel predefined objects for the periodic
 *            frame sending
 *
 *            For the usage of the Arduino Monitor, please check
 *            the user manual in the section USB communication.
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
#include "Signals.h"
#include "PID.h"
#include "NVM.h"
#include "PID.h"
#include "NVMVariables.h"
#include "FreeRTOS.h"
#include "ModeManager.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

/**
 * ---------------------------------------------------------
 * 					         COM GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define COM_MAX_QUEUE_MESSAGES 16 /* Used for the send message logic */

#define COM_UART_INIT_NUMBER_FRAMES        13    /* Number of frames of the initial messages (for transmission)*/
#define COM_UART_PERIODIC_NUMBER_FRAMES_TX 6     /* Number of frames of the periodic messages (for transmission)*/
#define COM_UART_PERIODIC_NUMBER_FRAMES_RX 5     /* Number of frames of the periodic messages (for reception)*/
#define COM_UART_INIT_FRAME_VALUE          0xA5  /* Number that the MCU will send to identify the initial frame */
#define COM_UART_PERIOD_FOR_DATA_TX        10    /*MS*/ /* Time of the send of the periodical frames */

#define COM_MSG_ERROR_CODE                 -1    /* Error code for the RPM & distance Frames (could not get the value) */
#define COM_FIFO_EMPTY_COUNTS_FOR_ERROR    10    /* Number of counts to assume errors on the FIFO get for distance & RPM */

#define COM_SECONDS_TO_WAIT_DAUGHTER_INIT  1     /* Seconds to wait for mark the GPU Comm as an error */

#define COM_ERROR_COUNTS_TO_CANCEL_VL53L0X_TX 20 /* Count of sequenced errors to cancel the comm with VL53L0X */
#define COM_TIME_TO_RESET_SENSOR_TASK 1000       /*MS*/ /* Time to wait to reset the sensor task */

//#define COM_SEND_DISTANCE_NORMALIZED           /* If defined the distance will be sent on a range from 0 to 100 */

/* Wait for arguments of the commands */
#define COM_WaitForArgs() osEventFlagsClear(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX); \
													osEventFlagsWait(xEvent_USBHandle, CDC_FLAG_MESSAGE_RX, osFlagsWaitAny, osWaitForever);

#define if_msg_is(x) if(strcmp(CDC_ResBuffer, x) == 0)

/**
 * ---------------------------------------------------------
 * 					       SUPPORTED USB MESSAGES
 * ---------------------------------------------------------
 */
#define COM_CPU_LOAD_USB_MSG       "CPU"        /* Make a CPU load measure - No args*/
#define COM_CONSTANT_USB_MSG       "CONST"      /* Change the constants of the PID controller - Args: KP, KI, KD*/
#define COM_GET_CONSTANTS_USB_MSG  "CONST_GET"  /* Request the value of the PID constants - No Args */
#define COM_CONTROL_ACTION_USB_MSG "ACT"        /* Desired action control (only valid when mode is on slave) - Args: ControlAction 0:100*/
#define COM_SET_POINT_USB_MSG      "SP"         /* Communicate the set point - Args Setpoint */
#define COM_SET_FIXED_FREQ_DIST  	 "FIXED_D_F"  /* Set a fixed frequency for distance report - Args: Frequency on HZ */
#define COM_SET_FIXED_PERIOD_DIST	 "FIXED_D_P"  /* Set a fixed period for distance report - Args: period on ms */
#define COM_SET_FREE_FREQ_DIST   	 "FREE_D_F"   /* Set free sampling frequency on distance sensor - No Args*/
#define COM_SET_NVM_DEFAULT        "NVM_DEF"    /* Set the default values of the EEPROM variables - No Args */
#define COM_SET_NVM_DUMP           "NVM_DUMP"   /* Make a memory dump of the EEPROM data - No Args*/
#define COM_SET_NVM_DUMP_ADDR      "NVM_ADDR"   /* Make a memory dump of with selected addresses - Args: InitAddr, LastAddr */

#define COM_DISTANCE_SEND_FORMAT   "DIST: %d\n" /* Format for the distance send to the usb */
#define COM_CONFIRM_SLAVE_MODE     "READY\n"    /* It's sent the system starts slave mode */
/**
 * ---------------------------------------------------------
 * 					          COM GENERAL TYPES
 * ---------------------------------------------------------
 */
typedef union Data16 {
	int16_t data;
	uint8_t rawData[sizeof(int16_t)];
}COMData16;

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
        uint8_t messageID;          /* 8 bits for message ID */ /* Limit to 7 bits */
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

/**
 * ---------------------------------------------------------
 * 					        COM GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void vTaskCOM(void *argument);
void vSubTaskUSB(void *argument);
void vSubTaskUART(void *argument);

result_t COM_SendMessage (uint8_t messageID, MessageType type, PriorityType priority, uint32_t payload);

void vTimer_UARTSendCallback(void *argument);
void vTimer_WdgUARTCallback(void *argument);
void vTimer_RestartSensorTaskCallback(void *argument);

#endif /* SWC_COM_COM_H_ */
