/**
 * @file      OSHandler.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Aug 5, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_OSHANDLER_OSHANDLER_H_
#define SWC_OSHANDLER_OSHANDLER_H_

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "main.h"

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
uint16_t usCheckHeapUsage(void);
uint16_t usCheckStackUsage(xTaskHandle xTask, uint32_t *pwStackBuffer);
result_t xResetTask(xTaskHandle xTask);

#endif /* SWC_OSHANDLER_OSHANDLER_H_ */
