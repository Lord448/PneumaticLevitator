/**
 * @file      DIDList.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jun 2, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#ifndef SWC_DIAGAPPL_DIDLIST_H_
#define SWC_DIAGAPPL_DIDLIST_H_

/*
 *-----------------------------------------
 *          SESSION CONTROL DIDs
 *-----------------------------------------
 */
#define START_DIAGNOSTIC_SESSION 0x01
#define END_DIAGNOSTIC_SESSION   0x02
/*
 *-----------------------------------------
 *            ECU RESET DIDs
 *-----------------------------------------
 */
#define RESET_MOTHERBOARD   0x01
#define RESET_DUAGHTERBOARD 0X02
/*
 *-----------------------------------------
 *        COMMUNICATION CONTROL DIDs
 *-----------------------------------------
 */
#define BLOCK_COMMUNICATION_WITH_DAUGHTERBOARD 0x01
#define START_COMMUNICATION_WITH_DAUGHTERBOARD 0x02
/*
 *-----------------------------------------
 *          READ DATA BY ID DIDs
 *-----------------------------------------
 */
#define READ_MEMORY_DUMP   0x01
#define READ_PID_CONSTANTS 0x02
#define READ_P_CONSTANT    0x03
#define READ_I_CONSTANT    0x04
#define READ_D_CONSTANT    0x05

/*
 *-----------------------------------------
 *        READ DATA BY ADDRESS DIDs
 *-----------------------------------------
 */

#endif /* SWC_DIAGAPPL_DIDLIST_H_ */
