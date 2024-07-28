/**
 * @file      MainMenu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jul 28, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_MENUS_MAINMENU_H_
#define SWC_UI_MENUS_MAINMENU_H_

#define charslen(_) (int32_t)(sizeof(_)-1)

/**
 * ---------------------------------------------------------
 * 					       PROGRESS BAR SYMBOLS
 * ---------------------------------------------------------
 */
#define PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE       36
#define PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT      36+80    /*TODO: Adjust*/
#define PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL 36+80+80 /*TODO: Adjust*/
#define PROGRESS_BAR_OFFSET 10
#define PROGRESS_BAR_Y_START_LOC 95 /*TODO: Adjust*/
#define PROGRESS_BAR_Y_END_LOC 190

/**
 * ---------------------------------------------------------
 * 					          TEXBOX SYMBOLS
 * ---------------------------------------------------------
 */
#define TEXTBOX_FONT           FONT_arial_12X15_CYRILLIC
#define TEXTBOX_FONT_X         12
#define TEXTBOX_FONT_Y         15

#define TB_DATE_ID             TXB_ID_0
#define TB_RPM_ID              TXB_ID_1
#define TB_RPM_STR_ID          TXB_ID_2
#define TB_DISTANCE_ID         TXB_ID_3
#define TB_DISTANCE_STR_ID     TXB_ID_4
#define TB_SET_POINT_ID        TXB_ID_5
#define TB_SET_POINT_STR_ID    TXB_ID_6
#define TB_CTRL_ACTION_ID      TXB_ID_7
#define TB_CTRL_ACT_STR_ID     TXB_ID_8
#define TB_ACTION_MODE_ID      TXB_ID_9
#define TB_KP_ID               TXB_ID_10
#define TB_KI_ID               TXB_ID_11
#define TB_KD_ID               TXB_ID_12
#define TB_ERROR_ID            TXB_ID_13

/* Date */
/* RPM */
#define TB_RPM_X_INIT          LCD_WIDTH-80
#define TB_RPM_Y						   76
#define TB_RPM_CHARS           charslen("RPM")
/* RPM STR*/
#define TB_RPM_STR_X_INIT      TB_RPM_X_INIT+(TB_RPM_CHARS*TEXTBOX_FONT_X)-2
#define TB_RPM_STR_Y				   TB_RPM_Y
#define TB_RPM_STR_CHARS       charslen("5000")
/* Distance */
#define TB_DISTANCE_INIT       5
#define TB_DISTANCE_Y				   PROGRESS_BAR_Y_END_LOC+5
#define TB_DISTANCE_CHARS      charslen("Distancia")
/* Distance STR */
#define TB_DISTANCE_STR_INIT   22
#define TB_DISTANCE_STR_Y		   TB_RPM_Y
#define TB_DISTANCE_STR_CHARS  charslen("500")
/* SetPoint */
#define TB_SET_POINT_INIT      TB_DISTANCE_INIT+85
#define TB_SET_POINT_Y			   PROGRESS_BAR_Y_END_LOC+5
#define TB_SET_POINT_CHARS     charslen("SetPoint")
/* SetPoint STR */
#define TB_SET_POINT_STR_INIT  TB_DISTANCE_STR_INIT+80
#define TB_SET_POINT_STR_Y		 TB_RPM_Y
#define TB_SET_POINT_STR_CHARS charslen("500")
/* Control Action */
#define TB_CTRL_ACTION_INIT    TB_SET_POINT_INIT+70
#define TB_CTRL_ACTION_Y		   PROGRESS_BAR_Y_END_LOC+5
#define TB_CTRL_ACTION_CHARS   charslen("Accion Ctrl")
/* Action Mode */
#define TB_ACTION_MODE_INIT    TB_DISTANCE_INIT+70
#define TB_ACTION_MODE_Y		   PROGRESS_BAR_Y_END_LOC+5
#define TB_ACTION_MODE_CHARS   charslen("SetPoint")
/* Kp */
#define TB_KP_INIT             TB_DISTANCE_INIT+90
#define TB_KP_Y		             PROGRESS_BAR_Y_END_LOC+5
#define TB_KP_CHARS            charslen("SetPoint")
/* Ki */
#define TB_KI_INIT             TB_DISTANCE_INIT+90
#define TB_KI_Y		             PROGRESS_BAR_Y_END_LOC+5
#define TB_KI_CHARS            charslen("SetPoint")
/* Kd */
#define TB_KD_INIT             TB_DISTANCE_INIT+90
#define TB_KD_Y		             PROGRESS_BAR_Y_END_LOC+5
#define TB_KD_CHARS            charslen("SetPoint")
/* Error */
#define TB_ERROR_INIT          TB_DISTANCE_INIT+90
#define TB_ERROR_Y		         PROGRESS_BAR_Y_END_LOC+5
#define TB_ERROR_CHARS         charslen("SetPoint")

#endif /* SWC_UI_MENUS_MAINMENU_H_ */
