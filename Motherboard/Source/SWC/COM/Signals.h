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

#include "COM.h"


#define ACK_DATA 1<<7

#define SIGNAL(ID) (ACK_DATA | ID)

/*Declare here all the signals*/
#define NO_MSG_ID         SIGNAL(0x00)
#define PERIODIC_ID       SIGNAL(0x01)
#define ENTER_DIAGS_ID    SIGNAL(0x02)

#define CPU_LOAD_DAUGHTER SIGNAL(0x7F) /* 7 bit limit */

#endif /* SWC_COM_SIGNALS_H_ */
