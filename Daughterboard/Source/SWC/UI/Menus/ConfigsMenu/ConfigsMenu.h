/**
 * @file      ConfigsMenu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      28 ago 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_MENUS_CONFIGSMENU_CONFIGSMENU_H_
#define SWC_UI_MENUS_CONFIGSMENU_CONFIGSMENU_H_

#include "UI.h"
#include "ugui.h"

/**
 * ---------------------------------------------------------
 * 					           IMAGE SYMBOLS
 * ---------------------------------------------------------
 */
#define LEFT_ARROW_IMG_ID          IMG_ID_0
#define PRESSED_LEFT_ARROW_IMG_ID  IMG_ID_1
#define RIGHT_ARROW_IMG_ID         IMG_ID_2
#define PRESSED_RIGHT_ARROW_IMG_ID IMG_ID_3
/**
 * ---------------------------------------------------------
 * 					          IMAGE POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define LEFT_ARROW_X_POS  10
#define LEFT_ARROW_Y_POS  80
#define RIGHT_ARROW_X_POS 185
#define RIGHT_ARROW_Y_POS LEFT_ARROW_Y_POS
#define ICON_X_POS 68
#define ICON_Y_POS 50
/**
 * ---------------------------------------------------------
 * 					           TEXBOX SYMBOLS
 * ---------------------------------------------------------
 */
#define MENU_TEXTBOX_FONT   FONT_arial_20X23_CYRILLIC
#define MENU_TEXTBOX_FONT_X 20
#define MENU_TEXTBOX_FONT_Y 23

#define TB_CHANGE_CONS_ID   TXB_ID_0
#define TB_CHANGE_MODE_ID   TXB_ID_1
/**
 * ---------------------------------------------------------
 * 					       TEXBOX POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define MENU_TEXTBOX_Y_POS  160

#define TB_MAIN_LOBBY_X_POS 43
#define TB_CHANG_CONS_X_POS 2
#define TB_CHANG_MODE_X_POS 0
#define TB_VER_PROMPT_X_POS 55
#define TB_VER_PROMPT_Y_POS MENU_TEXBOX_Y_POS + MENU_TEXTBOX_FONT_Y + 5

void ConfigsMenu_Dynamics(Buttons btnPress, bool *isFirstInit);
void ConfigsMenu_buildObjects(void);

#endif /* SWC_UI_MENUS_CONFIGSMENU_CONFIGSMENU_H_ */
