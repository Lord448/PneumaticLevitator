/**
 * @file      UIMainSM.c
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
#include "UIMainSM.h"

extern osMessageQueueId_t xFIFO_ButtonsHandle;

extern UG_WINDOW mainWindow;
extern UG_WINDOW menuWindow;

MenuStages menuStage = sMainLobby; /*Global variable*/

void UIMainSM_Init(void)
{
}

void UIMainSM_InfiniteLoop(void)
{
	Buttons btnPress;
	osMessageQueueGet(xFIFO_ButtonsHandle, &btnPress, NULL, osNoTimeout);
	if(iMenu == btnPress)
	{
		/* Menu Button pressed */
		UIMainSM_ChangeMenu(sMenu);
		btnPress = None;
	}
	else
	{
		/* Do Nothing */
	}
	/* TODO: Debug proposes */
	static bool firstInit = true;
	if(firstInit)
	{
		UIMainSM_ChangeMenu(sMenu);
		firstInit = false;
	}
	/* TODO: Debug proposes */
	switch(menuStage)
	{
		case sMainLobby:
			MainMenu_MenuDynamics();
		break;
		case sMenu:
			Menu_MenuDynamics();
		break;
		case sConfigs:
		break;
		case sAbout:
		break;
		case sPlot:
		break;
		case sUSBConfig:
		break;
		case sPlantAnalysis:
		break;
		case dummy:
		default:
		break;
	}
}

void UIMainSM_ChangeMenu(MenuStages menuStageID)
{
	UG_WINDOW *window;
	switch(menuStageID)
	{
		case sMainLobby:
			window = &mainWindow;
		break;
		case sMenu:
			window = &menuWindow;
		break;
		case sConfigs:

		break;
		case sAbout:
		break;
		case sPlot:
		break;
		case sUSBConfig:
		break;
		case sPlantAnalysis:
		break;
		case dummy:
		default:
		break;
	}
	UG_WindowHide(window);
	menuStage = menuStageID;
	UG_WindowShow(window);
	UG_Update();
}
