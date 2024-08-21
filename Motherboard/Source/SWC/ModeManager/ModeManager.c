/**
 * @file      ModeManager.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
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

#include "ModeManager.h"

extern osThreadId_t TaskModeManagerHandle;
extern osThreadId_t TaskPIDHandle;

extern osEventFlagsId_t xEvent_ControlModesHandle;

static void suspendPIDTask(void);
static void resumePIDTask(void);

/**
 * @brief
 * @param
 * @retval
 */
void vTaskModeManager(void *argument)
{
	ControlModes currentMode = AutoPID;
	uint32_t eventFlags = 0;

	for(;;)
	{
		/* Search for new request modes */
		eventFlags = osEventFlagsWait(xEvent_ControlModesHandle, ALL_CONTROL_MODE_FLAGS, osFlagsWaitAny, osWaitForever);
		if(eventFlags&DEV_MODE_FLAG)
		{
			/* Dev mode is enabled */
			/* TODO Implement here */
		}
		/* A new change of mode is requested */
		switch(currentMode)
		{
			case AutoPID:
				resumePIDTask();
			break;
			case Slave:
				suspendPIDTask();
			break;
			case Manual:
				suspendPIDTask();
			break;
			default:
			break;
		}
		/* Clear the flags */
		osEventFlagsClear(xEvent_ControlModesHandle, eventFlags);
	}
}

static void suspendPIDTask(void)
{
	osThreadSuspend(TaskPIDHandle);
	//TODO Think about the reset of the PID of the EEPROM variables access, it's more efficient to read the variables once
}

static void resumePIDTask(void)
{
	osThreadResume(TaskPIDHandle);
}
