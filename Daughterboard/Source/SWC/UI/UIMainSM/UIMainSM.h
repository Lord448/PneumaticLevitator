/**
 * @file      UIMainSM.h
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

#ifndef SWC_UI_UIMAINSM_UIMAINSM_H_
#define SWC_UI_UIMAINSM_UIMAINSM_H_

#include "ugui.h"
#include "HMI.h"
#include "MainMenu.h"
#include "AboutMenu.h"
#include "ConfigsMenu.h"
#include "ugui.h"
/**
 * ---------------------------------------------------------
 * 					     UI_MAIN_SM GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
/*Macro meant to be used with the uGUI BMP type*/
#define DrawCenterBitmap(bmp) UG_DrawBMP((LCD_WIDTH-bmp->width)/2, (LCD_HEIGHT-bmp->height)/2, bmp)
/**
 * ---------------------------------------------------------
 * 					      UI_MAIN_SM GENERAL TYPES
 * ---------------------------------------------------------
 */
typedef enum MenuStages {
	sMainLobby,
	sMenu,
	sConfigs,
	sAbout,
	sLCDTest,
	sPlot,
	sUSBConfig,
	sPlantAnalysis,
	dummy = -1, /* Dummy variable to make the enum signed */
}MenuStages;
/**
 * ---------------------------------------------------------
 * 					     UI_MAIN_SM GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void UIMainSM_InfiniteLoop(void);
void UIMainSM_ChangeMenu(MenuStages menuStageID);

#endif /* SWC_UI_UIMAINSM_UIMAINSM_H_ */
