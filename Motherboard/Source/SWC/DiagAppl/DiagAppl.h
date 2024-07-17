/**
 * @file      DiagAppl.h
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

#ifndef SWC_DIAGAPPL_DIAGAPPL_H_
#define SWC_DIAGAPPL_DIAGAPPL_H_

#include "DIDList.h"
#include "DTCList.h"

void DiagAppl_Init(void);

void vTaskDiagAppl(void *argument);

#endif /* SWC_DIAGAPPL_DIAGAPPL_H_ */
