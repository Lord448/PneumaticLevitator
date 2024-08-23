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

MenuState currentMenuState;

MenuState msMainLobby;
MenuState msMenu;
MenuState msConfiguration;
MenuState msAbout;
MenuState msPlot;
MenuState msUSBConfig;
MenuState msPlantAnalysis;


void UIMainSM_Init(void)
{
	/* Initalizing the structures */
	msMainLobby.menuStageID = sMainLobby;
	msMainLobby.window = &mainWindow;
	msMenu.menuStageID = sMenu;
	msMenu.window = &menuWindow;

	currentMenuState = msMainLobby;
}

void UIMainSM_InfiniteLoop(void)
{
	Buttons btnPress;
	osMessageQueueGet(xFIFO_ButtonsHandle, &btnPress, NULL, osNoTimeout);
	if(iMenu == btnPress)
	{
		/* Menu Button pressed */
		UIMainSM_ChangeMenu(msMenu);
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
		UIMainSM_ChangeMenu(msMenu);
		firstInit = false;
	}
	/* TODO: Debug proposes */
	switch(currentMenuState.menuStageID)
	{
		case sMainLobby:
			MainMenu_MenuDynamics();
		break;
		case sMenu:
			Menu_MenuDynamics();
		break;
		case sConfiguration:
		break;
		case sAbout:
		break;
		case sPlot:
		break;
		case sUSBConfig:
		break;
		case sPlantAnalysis:
		break;
	}
}

void UIMainSM_ChangeMenu(MenuState menu)
{
	UG_WindowHide(currentMenuState.window);
	currentMenuState = menu;
	UG_WindowShow(menu.window);
	UG_Update();
}
