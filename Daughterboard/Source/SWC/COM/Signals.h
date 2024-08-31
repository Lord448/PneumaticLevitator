/**
 * @file      Signals.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      May 30, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#ifndef SWC_COM_SIGNALS_H_
#define SWC_COM_SIGNALS_H_

/**
 * ---------------------------------------------------------
 * 					               MACROS
 * ---------------------------------------------------------
 */
#define ACK_DATA 1<<7

#define SIGNAL(ID) (ACK_DATA | ID)

/**
 * ---------------------------------------------------------
 * 					               SIGNALS
 * ---------------------------------------------------------
 */

#define INIT_FRAME        0xA5

#define NO_MSG_ID         SIGNAL(0x00)
#define PERIODIC_ID       SIGNAL(0x01) /* Only sent by motherboard */
#define ENTER_DIAGS_ID    SIGNAL(0x02) /* Only sent by motherboard */
#define SET_POINT					SIGNAL(0x03)
#define REQUEST_CONST			SIGNAL(0x04) /* Only sent by daughterboard */
#define REQUEST_KP        SIGNAL(0x05) /* Only sent by daughterboard */
#define REQUEST_KI        SIGNAL(0x06) /* Only sent by daughterboard */
#define REQUEST_KD        SIGNAL(0x07) /* Only sent by daughterboard */
#define KP_VAL            SIGNAL(0x07)
#define KI_VAL            SIGNAL(0x08)
#define KD_VAL            SIGNAL(0x09)
#define TOGGLE_PID        SIGNAL(0x0A)

#define CPU_LOAD_DAUGHTER SIGNAL(0x7F) /* 7 bit limit */
/**
 * ---------------------------------------------------------
 * 					     TYPEDEFS (COMMAND ARGUMENTS)
 * ---------------------------------------------------------
 */

#endif /* SWC_COM_SIGNALS_H_ */
