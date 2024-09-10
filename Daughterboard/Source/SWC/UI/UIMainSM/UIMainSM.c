/**
 * @file      UIMainSM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This component handles the menu UI states and the
 *            the change of the menu, it also calls the dynamics
 *            (animation and button processing) of each menu
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
/**
 * @brief  Main function that process the UI
 *         state machine and defines the UI
 *         behavior when entering a menu
 * @param  none
 * @retval none
 */
void UIMainSM_InfiniteLoop(void)
{
	Buttons btnPress = iNone;
	static bool isFirstMenuInit = true;
	osMessageQueueGet(xFIFO_ButtonsHandle, &btnPress, NULL, osNoTimeout);

	if(iMenu == btnPress)
	{
		/* Menu Button pressed */
		UIMainSM_ChangeMenu(sMenu);
		isFirstMenuInit = true;
		btnPress = iNone;
	}
	else
	{
		/* Do Nothing */
	}

	/**
	 * @note Since most of the dynamics functions have the same signature
	 *       it is possible (with no much development cost) to create a
	 *       generic menu structure with the pointer of the window, the
	 *       stage id, and two function pointers, the BuildObjects and
	 *       the dynamic objects, so the second function pointer can be
	 *       called here; assuming those variables are members of a typedef
	 *       structure, we can declare a structure pointer that can point
	 *       to the current menu displayed, this strategy will need the
	 *       declaration of a global structure that contains all the menus
	 *       (A similar scheme it's implemented on ConfigsMenu, with the
	 *       "MenuSelector" and "MenuSelectorGroup"). This strategy would
	 *       save a lot of "switch case" statements among the UI code
	 */

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

/**
 * @brief  Changes the menu displayed on the screen
 * @param  menuStageID : ID of the new menu
 * @retval none
 */
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
/**
 * @brief  This functions returns the window with a pointer
 *         given menu ID
 * @param  menuStageID : Id of the menu
 * @retval UG_WINDOW* : Pointer to the respective window
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
