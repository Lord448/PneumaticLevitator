/**
 * @file      EcuM.h
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

#ifndef SWC_ECUM_ECUM_H_
#define SWC_ECUM_ECUM_H_

#include "NVM/NVM.h"

void EcuM_Init(void);

void vTaskEcuM(void *argument);

#endif /* SWC_ECUM_ECUM_H_ */
