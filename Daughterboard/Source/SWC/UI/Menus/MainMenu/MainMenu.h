/**
 * @file      MainMenu.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     On this menu it's displayed by progress bar
 *            the distance of the ball, the action control,
 *            the RPM of the FAN, the set point, the current
 *            control mode and the gains of the PID control
 *            (Kp, Ki, Kd)
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

#include "UI.h"
#include "COM.h"
#include "cmsis_os2.h"
#include "Bitmaps.h"
#include <string.h>
#include <stdio.h>

/**
 * ---------------------------------------------------------
 * 					       PROGRESS BAR SYMBOLS
 * ---------------------------------------------------------
 */
#define PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE       28
#define PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT      PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE+68
#define PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT+62
#define PROGRESS_BAR_X_CENTRAL_LOC_RPM						PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL+50
#define PROGRESS_BAR_OFFSET 10
#define PROGRESS_BAR_Y_START_LOC 95
#define PROGRESS_BAR_Y_END_LOC 190

#define PROGRESS_BAR_DISTANCE_ID       PGB_ID_0
#define PROGRESS_BAR_SET_POINT_ID      PGB_ID_1
#define PROGRESS_BAR_ACTION_CONTROL_ID PGB_ID_2
#define PROGRESS_BAR_RPM_ID 					 PGB_ID_3

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
#define TB_MANUAL_ID           TXB_ID_10
#define TB_PID_ID              TXB_ID_11
#define TB_USB_ID              TXB_ID_12
#define TB_KP_ID               TXB_ID_13
#define TB_KI_ID               TXB_ID_14
#define TB_KD_ID               TXB_ID_15

/* RPM */
#define TB_RPM_X_INIT          LCD_WIDTH-45
#define TB_RPM_Y						   PROGRESS_BAR_Y_END_LOC+5
#define TB_RPM_CHARS           charslen("RPM")
/* RPM STR*/
#define TB_RPM_STR_X_INIT      LCD_WIDTH-50
#define TB_RPM_STR_Y				   76
#define TB_RPM_STR_CHARS       charslen("5000")
/* Distance */
#define TB_DISTANCE_INIT       0
#define TB_DISTANCE_Y				   PROGRESS_BAR_Y_END_LOC+5
#define TB_DISTANCE_CHARS      charslen("Distancia")
/* Distance STR */
#define TB_DISTANCE_STR_INIT   15
#define TB_DISTANCE_STR_Y		   TB_RPM_STR_Y
#define TB_DISTANCE_STR_CHARS  charslen("500")
/* SetPoint */
#define TB_SET_POINT_INIT      TB_DISTANCE_INIT+70
#define TB_SET_POINT_Y			   PROGRESS_BAR_Y_END_LOC+5
#define TB_SET_POINT_CHARS     charslen("SetPoint")
/* SetPoint STR */
#define TB_SET_POINT_STR_INIT  TB_DISTANCE_STR_INIT+68
#define TB_SET_POINT_STR_Y		 TB_RPM_STR_Y
#define TB_SET_POINT_STR_CHARS charslen("500")
/* Control Action */
#define TB_CTRL_ACTION_INIT    TB_SET_POINT_INIT+65
#define TB_CTRL_ACTION_Y		   PROGRESS_BAR_Y_END_LOC+5
#define TB_CTRL_ACTION_CHARS   charslen("Accion Ctrl")
/* Control Action STR */
#define TB_CTRL_ACT_STR_INIT   TB_SET_POINT_INIT+75
#define TB_CTRL_ACT_STR_Y		   TB_RPM_STR_Y
#define TB_CTRL_ACT_STR_CHARS  charslen("100")
/* Action Mode */
#define TB_ACTION_MODE_INIT    TB_SET_POINT_STR_INIT+10
#define TB_ACTION_MODE_Y		   0
#define TB_ACTION_MODE_CHARS   charslen("Control")
/*Manual*/
#define TB_MANUAL_INIT         TB_ACTION_MODE_INIT
#define TB_MANUAL_Y		         TB_ACTION_MODE_Y+TEXTBOX_FONT_Y+3
#define TB_MANUAL_CHARS        charslen("Manual")
/*PID*/
#define TB_PID_INIT            TB_ACTION_MODE_INIT
#define TB_PID_Y		           TB_MANUAL_Y+TEXTBOX_FONT_Y+1
#define TB_PID_CHARS           charslen("PID")
/*USB*/
#define TB_USB_INIT            TB_ACTION_MODE_INIT
#define TB_USB_Y		           TB_PID_Y+TEXTBOX_FONT_Y+1
#define TB_USB_CHARS           charslen("USB")
/* Kp */
#define TB_KP_INIT             0
#define TB_KP_Y		             5
#define TB_KP_CHARS            charslen("KP: 0.0000000")
/* Ki */
#define TB_KI_INIT             TB_KP_INIT
#define TB_KI_Y		             TB_KP_Y+TEXTBOX_FONT_Y+5
#define TB_KI_CHARS            charslen("KI: 0.0000000")
/* Kd */
#define TB_KD_INIT             TB_KP_INIT
#define TB_KD_Y		             TB_KI_Y+TEXTBOX_FONT_Y+5
#define TB_KD_CHARS            charslen("KD: 0.0000000")

/**
 * ---------------------------------------------------------
 * 					            CHECKBOX SYMBOLS
 * ---------------------------------------------------------
 */
#define CB_MANUAL_ID           CHB_ID_0
#define CB_PID_ID							 CHB_ID_1
#define CB_USB_ID              CHB_ID_2

/**
 * ---------------------------------------------------------
 * 					           GENERIC SYMBOLS
 * ---------------------------------------------------------
 */
#define CB_MANUAL_INIT

#define MAX_DISTANCE 520
#define MAX_RPM 5200

#define DEFAULT_SET_POINT 260

#define MainMenu_isMode(x) osEventFlagsGet(xEvent_CurrentControlModeHandle)&x

typedef struct ControlGain {
	float value;
	uint8_t constant;
}ControlGain;


/**
 * ---------------------------------------------------------
 * 					     MAIN_MENU GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void MainMenu_MenuDynamics(Buttons btnPressed, bool *isFirstMenuInit);
void MainMenu_buildObjects(void);

result_t MainMenu_setKP(float kp);
result_t MainMenu_setKI(float ki);
result_t MainMenu_setKD(float kd);
result_t MainMenu_setControlConstants(float kp, float ki, float kd);
result_t MainMenu_setDistance(int16_t distance);
result_t MainMenu_setSetPoint(uint16_t setPoint);
result_t MainMenu_setRPM(int16_t rpm);
result_t MainMenu_setActionControl(int8_t actionControl);
result_t MainMenu_setManualChecked(bool state);
result_t MainMenu_setPIDChecked(bool state);
result_t MainMenu_setUSBChecked(bool state);
result_t MainMenu_preCheck(uint16_t ID);

#endif /* SWC_UI_MENUS_MAINMENU_H_ */
