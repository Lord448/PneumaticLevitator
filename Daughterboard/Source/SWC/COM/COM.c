/**
 * @file      COM.c
 * @author    TODO
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

#include "COM.h"

extern osEventFlagsId_t xEventFinishedInitHandle;

/**
* @brief Function implementing the TaskCOM thread.
* @param argument: Not used
* @retval None
*/
void vTaskCOM(void *argument)
{
	osEventFlagsWait(xEventFinishedInitHandle, FINISH_INIT_ID, 0U, osWaitForever);

	for(;;)
	{
		osDelay(1);
	}
}
