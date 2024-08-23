/**
 * @file      UIMainSM.h
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

#ifndef SWC_UI_UIMAINSM_UIMAINSM_H_
#define SWC_UI_UIMAINSM_UIMAINSM_H_

#include "ugui.h"
#include "HMI.h"
#include "MainMenu.h"
#include "ugui.h"

/*Macro meant to be used with the uGUI BMP type*/
#define DrawCenterBitmap(bmp) UG_DrawBMP((LCD_WIDTH-bmp->width)/2, (LCD_HEIGHT-bmp->height)/2, bmp)

typedef enum MenuStages {
	sMainLobby = 0,
	sMenu,
	sConfiguration,
	sAbout,
	sPlot,
	sUSBConfig,
	sPlantAnalysis,
	dummy = -1, /* Dummy variable to make the enum signed */
}MenuStages;

typedef struct MenuState {
	MenuStages menuStageID;
	UG_WINDOW  *window;
}MenuState;

typedef struct UIWindow {
	UG_WINDOW window;
	UG_OBJECT *obj_buff;
	uint8_t obj_len;
}UIWindow;

extern MenuState msMainLobby;
extern MenuState msMenu;
extern MenuState msConfiguration;
extern MenuState msAbout;
extern MenuState msPlot;
extern MenuState msUSBConfig;
extern MenuState msPlantAnalysis;

void UIMainSM_Init(void);
void UIMainSM_InfiniteLoop(void);
void UIMainSM_ChangeMenu(MenuState menu);

#endif /* SWC_UI_UIMAINSM_UIMAINSM_H_ */
