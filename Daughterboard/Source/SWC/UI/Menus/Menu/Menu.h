/**
 * @file      Menu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     In this software component it's implemented a
 *            carrousel that allows the user to change between
 *            all the menus, with an extra that runs in this
 *            component, the LCD test
 *
 * @date      Aug 22, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_MENUS_MENU_MENU_H_
#define SWC_UI_MENUS_MENU_MENU_H_

#include "UIMainSM.h"
#include "ugui.h"
#include "UI.h"
#include "Bitmaps.h"
#include "HMI.h"
#include <stdio.h>
#include <string.h>

/**
 * ---------------------------------------------------------
 * 					           IMAGE SYMBOLS
 * ---------------------------------------------------------
 */
#define LEFT_ARROW_IMG_ID          IMG_ID_0
#define PRESSED_LEFT_ARROW_IMG_ID  IMG_ID_1
#define RIGHT_ARROW_IMG_ID         IMG_ID_2
#define PRESSED_RIGHT_ARROW_IMG_ID IMG_ID_3

#define MAIN_LOBBY_MENU_IMG_ID     IMG_ID_4
#define CONFIG_MENU_IMG_ID         IMG_ID_5
#define LCD_TEST_IMG_ID						 IMG_ID_6
#define ABOUT_MENU_IMG_ID          IMG_ID_7
#define PLOT_MENU_IMG_ID           IMG_ID_8
#define USB_CONFIG_MENU_IMG_ID     IMG_ID_9
#define PLANT_MENU_IMG_ID          IMG_ID_10

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

#define TB_MAIN_LOBBY_ID    TXB_ID_0
#define TB_CONFIG_ID        TXB_ID_1
#define TB_LCD_TEST_ID      TXB_ID_2
#define TB_ABOUT_ID         TXB_ID_3
#define TB_PLOT_ID          TXB_ID_4
#define TB_USB_CONFIG_ID    TXB_ID_5
#define TB_PLANT_ID         TXB_ID_6
#define TB_VER_PROMPT_ID    TXB_ID_7

/**
 * ---------------------------------------------------------
 * 					       TEXBOX POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define MENU_TEXBOX_Y_POS   160

#define TB_MAIN_LOBBY_X_POS 43
#define TB_CONFIG_X_POS     43
#define TB_LCD_TEST_X_POS   40
#define TB_ABOUT_X_POS      75
#define TB_PLOT_X_POS				80
#define TB_USB_CONFIG_X_POS 0
#define TB_PLANT_X_POS      30
#define TB_VER_PROMPT_X_POS 55
#define TB_VER_PROMPT_Y_POS MENU_TEXBOX_Y_POS + MENU_TEXTBOX_FONT_Y + 5

/**
 * ---------------------------------------------------------
 * 					       MENU GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void Menu_MenuDynamics(void);
void Menu_buildObjects(void);
void Menu_ShowInitImage(void);

#endif /* SWC_UI_MENUS_MENU_MENU_H_ */
