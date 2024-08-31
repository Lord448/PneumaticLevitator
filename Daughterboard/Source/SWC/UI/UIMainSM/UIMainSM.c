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

extern UG_WINDOW aboutWindow;
extern UG_WINDOW mainWindow;
extern UG_WINDOW menuWindow;
extern UG_WINDOW configsWindow;

extern struct MenuSelectorsGroup MenuSelectorsGroup;

/**
 * ---------------------------------------------------------
 * 					   SOFTWARE COMPONENT GLOBALS
 * ---------------------------------------------------------
 */
MenuStages menuStage = sMainLobby;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static UG_WINDOW* UIMainSM_SelectWindow(MenuStages menuStageID);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void UIMainSM_Init(void)
{
}

void UIMainSM_InfiniteLoop(void)
{
	Buttons btnPress = iNone;
	static bool isFirstMenuInit = true;
	osMessageQueueGet(xFIFO_ButtonsHandle, &btnPress, NULL, osNoTimeout);
	static bool debugInitMenu = false; /* TODO: Debug porpouses */

	if(iMenu == btnPress)
	{
		/* Menu Button pressed */
		//Menu_ShowInitImage();
		UIMainSM_ChangeMenu(sMenu);
		isFirstMenuInit = true;
		btnPress = iNone;
	}
	else
	{
		/* Do Nothing */
	}

	if(debugInitMenu) /* TODO: Debug porpouses */
	{
		//UIMainSM_ChangeMenu(sAbout);
		//debugInitMenu = false;
	}
	switch(menuStage)
	{
		case sMainLobby:
			MainMenu_MenuDynamics(btnPress, &isFirstMenuInit);
		break;
		case sMenu:
			Menu_MenuDynamics();
		break;
		case sConfigs:
			ConfigsMenu_Dynamics(btnPress, &isFirstMenuInit);
		break;
		case sAbout:
			AboutMenu_Dynamics(btnPress, &isFirstMenuInit);
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
	static MenuStages pastMenuStage = sMainLobby;
	UG_WINDOW *window = UIMainSM_SelectWindow(menuStageID);
	UG_WINDOW *pastWindow = UIMainSM_SelectWindow(pastMenuStage);

	HMI_DisableAllButtons();
	UG_WindowHide(pastWindow);
	menuStage = menuStageID;
	UG_WindowShow(window);
	UG_Update();
	pastMenuStage = menuStageID;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
static UG_WINDOW* UIMainSM_SelectWindow(MenuStages menuStageID)
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
			window = &configsWindow;
		break;
		case sAbout:
			window = &aboutWindow;
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
	return window;
}
