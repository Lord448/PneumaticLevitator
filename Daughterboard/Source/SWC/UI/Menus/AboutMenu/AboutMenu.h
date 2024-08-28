/**
 * @file      AboutMenu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     Implement the About Menu, menu that brings info
 *            about the hardware revision on both boards and
 *            software version, as well as the credits for
 *            each engineer that worked on the project
 *
 * @date      27 ago 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_MENUS_ABOUTMENU_ABOUTMENU_H_
#define SWC_UI_MENUS_ABOUTMENU_ABOUTMENU_H_

#include "UI.h"
#include "ugui.h"

/**
 * ---------------------------------------------------------
 * 					           IMAGE SYMBOLS
 * ---------------------------------------------------------
 */
#define UP_ARROW_IMG_ID            IMG_ID_0
#define PRESSED_UP_ARROW_IMG_ID    IMG_ID_1
#define DOWN_ARROW_IMG_ID          IMG_ID_2
#define PRESSED_DOWN_ARROW_IMG_ID  IMG_ID_3
/**
 * ---------------------------------------------------------
 * 					          IMAGE POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define UP_ARROW_POS_X             93
#define UP_ARROW_POS_Y             0
#define DOWN_ARROW_POS_X           UP_ARROW_POS_X
#define DOWN_ARROW_POS_Y           175
/**
 * ---------------------------------------------------------
 * 					          TEXTBOX SYMBOLS
 * ---------------------------------------------------------
 */
#define ABOUT_MENU_TEXTBOX_FONT    FONT_arial_20X23_CYRILLIC
#define ABOUT_MENU_TEXTBOX_FONT_X  20
#define ABOUT_MENU_TEXTBOX_FONT_Y  23

#define MENU_SMALL_TEXTBOX_FONT		 FONT_arial_16X18_CYRILLIC
#define MENU_SMALL_TEXTBOX_FONT_X  16
#define MENU_SMALL_TEXTBOX_FONT_Y  18

#define TB_HARD_REV_ID             TXB_ID_0
#define TB_HARD_REV_MOTHER_STR_ID  TXB_ID_1
#define TB_HARD_REV_DAUGHT_STR_ID  TXB_ID_2
#define TB_SOFT_VER_ID             TXB_ID_3
#define TB_SOFT_VER_STR_ID         TXB_ID_4
#define TB_HARD_ENG_ID             TXB_ID_5
#define TB_HARD_ENG_PEDRO_ID       TXB_ID_6
#define TB_HARD_ENG_PEDRO_MAIL_ID  TXB_ID_7
#define TB_HARD_ENG_DAVID_ID       TXB_ID_8
#define TB_HARD_ENG_DAVID_MAIL_ID  TXB_ID_9
#define TB_SOFT_ENG_ID             TXB_ID_10
#define TB_SOFT_ENG_PEDRO_ID       TXB_ID_11
#define TB_SOFT_ENG_PEDRO_MAIL_ID  TXB_ID_12
#define TB_SOFT_ENG_DAVID_ID       TXB_ID_13
#define TB_SOFT_ENG_DAVID_MAIL_ID  TXB_ID_14
#define TB_SOFT_ENG_ANDRES_ID      TXB_ID_15
#define TB_SOFT_ENG_ANDRES_MAIL_ID TXB_ID_16
#define TB_SOFT_ENG_HECTOR_ID      TXB_ID_17
#define TB_SOFT_ENG_HECTOR_MAIL_ID TXB_ID_18
#define TB_GITHUB_USERS						 TXB_ID_19
#define TB_PEDRO_GIT 							 20
#define TB_PEDRO_GIT_USER					 21
#define TB_DAVID_GIT							 22
#define TB_DAVID_GIT_USER          23
#define TB_ANDRES_GIT							 24
#define TB_ANDRES_GIT_USER         25
#define TB_HECTOR_GIT							 26
#define TB_HECTOR_GIT_USER         27
#define TB_SCHOOL_ID               28

/**
 * ---------------------------------------------------------
 * 					       TEXBOX POSITION SYMBOLS
 * ---------------------------------------------------------
 */
#define APPLY_OFFSET(_) _+5
#define SEQUENCED_TB(pre) APPLY_OFFSET(pre+ABOUT_MENU_TEXTBOX_FONT_Y)

#define TITLE_POS_Y								 43

/* Page 1 */
#define TB_HARD_REV_POS_Y          15
#define TB_HARD_REV_MOTHER_POS_Y   SEQUENCED_TB(TB_HARD_REV_POS_Y)
#define TB_HARD_REV_DAUGHT_POS_Y   SEQUENCED_TB(TB_HARD_REV_MOTHER_POS_Y)
#define TB_SOFT_VER_POS_Y          SEQUENCED_TB(TB_HARD_REV_DAUGHT_POS_Y) + 13
#define TB_SOFT_VER_STR_POS_Y      SEQUENCED_TB(TB_SOFT_VER_POS_Y)

/* Page 2 */
#define TB_HARD_ENG_POS_Y					 TITLE_POS_Y
#define TB_HARD_ENG_PEDRO_POS_Y    SEQUENCED_TB(TB_HARD_ENG_POS_Y) + 1
#define TB_HARDENG_PEDROMAIL_POS_Y SEQUENCED_TB(TB_HARD_ENG_PEDRO_POS_Y)
#define TB_HARD_ENG_DAVID_POS_Y    SEQUENCED_TB(TB_HARDENG_PEDROMAIL_POS_Y)-1
#define TB_HARDENG_DAVIDMAIL_POS_Y SEQUENCED_TB(TB_HARD_ENG_DAVID_POS_Y)

/* Page 3 */
#define TB_SOFT_ARCH_POS_Y         TITLE_POS_Y
#define TB_SOFT_ARCH_PEDRO_POS_Y   SEQUENCED_TB(TB_SOFT_ARCH_POS_Y)+20
#define TB_ARCH_PEDROMAIL_POS_Y    SEQUENCED_TB(TB_SOFT_ARCH_PEDRO_POS_Y)

/* Page 4 */
#define TB_SOFT_ENG_POS_Y					 TITLE_POS_Y
#define TB_SOFT_ENG_PEDRO_POS_Y    SEQUENCED_TB(TB_SOFT_ENG_POS_Y)+1
#define TB_SOFTENG_PEDROMAIL_POS_Y SEQUENCED_TB(TB_SOFT_ENG_PEDRO_POS_Y)
#define TB_SOFT_ENG_ANDRES_POS_Y   SEQUENCED_TB(TB_SOFTENG_PEDROMAIL_POS_Y)-1
#define TB_SOFENG_ANDRESMAIL_POS_Y SEQUENCED_TB(TB_SOFT_ENG_ANDRES_POS_Y)

/* Page 5 */
#define TB_SOFT_ENG_DAVID_POS_Y    TB_SOFT_ENG_PEDRO_POS_Y
#define TB_SOFTENF_DAVIDMAIL_POS_Y TB_SOFTENG_PEDROMAIL_POS_Y
#define TB_SOFT_ENG_HECTOR_POS_Y   TB_SOFT_ENG_ANDRES_POS_Y
#define TB_SOFENG_HECTORMAIL_POS_Y TB_SOFENG_ANDRESMAIL_POS_Y

/* Page 6 */
#define TB_GITHUB_USERS_POS_Y      TITLE_POS_Y
#define TB_GIT_PEDRO               TB_SOFT_ENG_PEDRO_POS_Y
#define TB_GIT_PEDRO_USER_POS_Y    TB_SOFTENG_PEDROMAIL_POS_Y
#define TB_GIT_DAVID_POS_Y         TB_SOFT_ENG_ANDRES_POS_Y
#define TB_GIT_DAVID_USER_POS_Y    TB_SOFENG_ANDRESMAIL_POS_Y

/* Page 7 */
#define TB_GIT_ANDRES_POS_Y        TB_GIT_PEDRO
#define TB_GIT_ANDRES_USER_POS_Y   TB_GIT_PEDRO_USER_POS_Y
#define TB_GIT_HECTOR_POS_Y        TB_GIT_DAVID_POS_Y
#define TB_GIT_HECTOR_USER_POS_Y   TB_GIT_DAVID_USER_POS_Y
/**
 * ---------------------------------------------------------
 * 					           GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define PEDRO_NAME  "Pedro Rojo"
#define PEDRO_MAIL  "pedroeroca@outlook.com"
#define PEDRO_GIT   "Lord448"
#define DAVID_NAME  "David Ramirez"
#define DAVID_MAIL  "dardito23@gmail.com"
#define DAVID_GIT   "noescine"
#define ANDRES_NAME "Andres Aguinaga"
#define ANDRES_MAIL "sample@gmail.com"
#define ANDRES_GIT  "AndresAL1"
#define HECTOR_NAME "Hector Rojo"
#define HECTOR_MAIL "hectoraroca@outlook.com"
#define HECTOR_GIT  "Mantace96"

#define SOFTWARE_VERSION "Version 0.0.1"

void AboutMenu_Dynamics(Buttons btnPress, bool *isFirstMenuInit);
void AboutMenu_buildObjects(void);

#endif /* SWC_UI_MENUS_ABOUTMENU_ABOUTMENU_H_ */

