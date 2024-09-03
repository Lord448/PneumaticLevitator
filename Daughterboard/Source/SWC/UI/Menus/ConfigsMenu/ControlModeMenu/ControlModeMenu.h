/**
 * @file      ControlModeMenu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Sep 2, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_MENUS_CONFIGSMENU_CONTROLMODEMENU_CONTROLMODEMENU_H_
#define SWC_UI_MENUS_CONFIGSMENU_CONTROLMODEMENU_CONTROLMODEMENU_H_

#include "UI.h"
#include "ugui.h"

#define sControlModeMenu_DrawFrame(x) UG_DrawFrame(x.xs, x.ys, x.xe, x.ye, C_ROYAL_BLUE)
#define sControlModeMenu_EraseFrame(x) UG_DrawFrame(x.xs, x.ys, x.xe, x.ye, C_WHITE)

/**
 * ---------------------------------------------------------
 * 					           TEXT BUTTON SYMBOLS
 * ---------------------------------------------------------
 */
#define CONTROL_MENU_TEXTBOX_FONT   FONT_arial_20X23_CYRILLIC
#define CONTROL_MENU_TEXTBOX_FONT_X 20
#define CONTROL_MENU_TEXTBOX_FONT_Y 23

#define BTN_MANUAL_MODE             BTN_ID_0
#define BTN_AUTO_PID_MODE						BTN_ID_1
#define BTN_SLAVE_MODE							BTN_ID_2
/**
 * ---------------------------------------------------------
 * 					      TEXT BUTTON POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define CONTROL_MENU_TEXTBOX_X_POS  20

#define BTN_MANUAL_MODE_Y_POS       40-27
#define BTN_PID_MODE_Y_POS          85-27
#define BTN_SLAVE_MODE_Y_POS        130-27
/**
 * ---------------------------------------------------------
 * 					           GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define TB_CURRENT_SELECT_ID        TXB_ID_0
#define TB_SELECT_ID                TXB_ID_1

#define TB_SELECT_Y_POS							185

#define MODE_MANUAL_FLAG            (uint32_t) 1U << 0
#define MODE_PID_FLAG               (uint32_t) 1U << 1
#define MODE_USB_FLAG               (uint32_t) 1U << 2

void ControlModeMenu_Dynamics(Buttons btnPress, bool *isFirstInit);
void ControlModeMenu_buildObjects(void);

#endif /* SWC_UI_MENUS_CONFIGSMENU_CONTROLMODEMENU_CONTROLMODEMENU_H_ */
