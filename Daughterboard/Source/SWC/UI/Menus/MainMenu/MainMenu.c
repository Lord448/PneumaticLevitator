/**
 * @file      MainMenu.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      28 jul 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#include "MainMenu.h"

extern osMessageQueueId_t xFIFO_ControlConstantsHandle;
extern osMessageQueueId_t xFIFO_DistanceHandle;
extern osMessageQueueId_t xFIFO_RPMHandle;
extern osMessageQueueId_t xFIFO_ButtonsHandle;
extern osMessageQueueId_t xFIFO_EncoderDataHandle;
extern osMessageQueueId_t xFIFO_ActionControlHandle;
extern osMessageQueueId_t xFIFO_ControlGainsHandle;

UG_WINDOW mainWindow;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sMainMenu_ProcessButtonPress(Buttons btnPressed, int16_t *setPoint);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void MainMenu_MenuDynamics(Buttons btnPressed, bool *isFirstMenuInit)
{
	static int16_t setPoint = DEFAULT_SET_POINT;
	int16_t Distance, rpm;
	int8_t actionControl;
	EncoderDir encoderDir;
	ControlConst controlConst;
	ControlGain Gain;

	if(*isFirstMenuInit)
	{
		/* First init of the menu */
		HMI_EnableAllButtons();
		MainMenu_setSetPoint(setPoint);
		*isFirstMenuInit = false;
	}
	/* Printing the bars */
	if(osMessageQueueGet(xFIFO_DistanceHandle, &Distance, NULL, osNoTimeout) == osOK)
		MainMenu_setDistance(Distance);
	if(osMessageQueueGet(xFIFO_RPMHandle, &rpm, NULL, osNoTimeout) == osOK)
		MainMenu_setRPM(rpm);
	if(osMessageQueueGet(xFIFO_ActionControlHandle, &actionControl, NULL, osNoTimeout) == osOK)
		MainMenu_setActionControl(actionControl);
	/*Constants process*/
	if(osMessageQueueGet(xFIFO_ControlConstantsHandle, &controlConst, NULL, osNoTimeout) == osOK)
		MainMenu_setControlConstants(controlConst.kp, controlConst.ki, controlConst.kd);
	if(osMessageQueueGet(xFIFO_ControlGainsHandle, &Gain, NULL, osNoTimeout) == osOK)
	{
		switch(Gain.constant)
		{
			case KP_VAL:
				MainMenu_setKP(Gain.value);
			break;
			case KI_VAL:
				MainMenu_setKI(Gain.value);
			break;
			case KD_VAL:
				MainMenu_setKD(Gain.value);
			break;
			default:
			break;
		}
	}
	if(osMessageQueueGet(xFIFO_EncoderDataHandle, &encoderDir, NULL, 1) == osOK)
	{
		/* The encoder has been moved */
		switch(encoderDir)
		{
			case Plus:
				setPoint+=15;
				if(MAX_DISTANCE < setPoint)
					setPoint = MAX_DISTANCE;
			break;
			case Minus:
				setPoint-=15;
				if(0 > setPoint)
					setPoint = 0;
			break;
		}
		MainMenu_setSetPoint(setPoint);
	}
	sMainMenu_ProcessButtonPress(btnPressed, &setPoint);
}

void MainMenu_buildObjects(void)
{
	static UG_OBJECT ObjWinBuf[WINDOW_MAX_OBJECTS];
	static UG_IMAGE AnimatedPCFANImage;
	/*Progress bar*/
	static UG_PROGRESS pgbDistance;
	static UG_PROGRESS pgbSetPoint;
	static UG_PROGRESS pgbActionControl;
	static UG_PROGRESS pgbRPM;
	/* TextBox */
	static UG_TEXTBOX  tbRPM;
	static UG_TEXTBOX  tbRPMStr;
	static UG_TEXTBOX  tbDistance;
	static UG_TEXTBOX  tbDistanceStr;
	static UG_TEXTBOX  tbSetPoint;
	static UG_TEXTBOX  tbSetPointStr;
	static UG_TEXTBOX  tbCtrlAction;
	static UG_TEXTBOX  tbCtrlActionStr;
	static UG_TEXTBOX  tbActionMode;
	static UG_TEXTBOX  tbManual;
	static UG_TEXTBOX  tbPID;
	static UG_TEXTBOX  tbUSB;
	static UG_TEXTBOX  tbKP;
	static UG_TEXTBOX  tbKI;
	static UG_TEXTBOX  tbKD;
	/* Checkbox */
	static UG_CHECKBOX cbManual;
	static UG_CHECKBOX cbPID;
	static UG_CHECKBOX cbUSB;

	UG_WindowCreate(&mainWindow, ObjWinBuf, WINDOW_MAX_OBJECTS, NULL);
	/* Set window characteristics */
	UG_WindowSetTitleText(&mainWindow, "Lobby Principal");
  UG_WindowSetTitleTextFont(&mainWindow, FONT_6X8);
  UG_WindowSetTitleHeight(&mainWindow, 15);
  UG_WindowSetXStart(&mainWindow, 0);
  UG_WindowSetYStart(&mainWindow, 0);
  UG_WindowSetXEnd(&mainWindow, LCD_WIDTH-1);
  UG_WindowSetYEnd(&mainWindow, LCD_HEIGHT-1);
  UG_WindowSetBackColor(&mainWindow, C_WHITE);

  /* Create an load the images */
  /* Animated FAN */
  UI_ImageCreate(&mainWindow, &AnimatedPCFANImage, IMG_ID_0, LCD_WIDTH-80, 3);
  UG_ImageSetBMP(&mainWindow, IMG_ID_0, &AnimatedPCFan);

  /* Setting progress bar */
  /* Distance of the ball */
  UG_ProgressCreate(&mainWindow, &pgbDistance, PROGRESS_BAR_DISTANCE_ID,
  		PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PROGRESS_BAR_DISTANCE_ID, C_ROYAL_BLUE);
  /* Set point */
  UG_ProgressCreate(&mainWindow, &pgbSetPoint, PROGRESS_BAR_SET_POINT_ID,
  		PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PROGRESS_BAR_SET_POINT_ID, C_ROYAL_BLUE);
  /* Action Control */
  UG_ProgressCreate(&mainWindow, &pgbActionControl, PROGRESS_BAR_ACTION_CONTROL_ID,
  		PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PROGRESS_BAR_ACTION_CONTROL_ID, C_ROYAL_BLUE);
  /* RPM */
  UG_ProgressCreate(&mainWindow, &pgbRPM, PROGRESS_BAR_RPM_ID,
  		PROGRESS_BAR_X_CENTRAL_LOC_RPM-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_RPM+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PROGRESS_BAR_RPM_ID, C_ROYAL_BLUE);

  /* Setting text boxes */
  /* RPM*/
  UI_TextboxCreate(&mainWindow, &tbRPM, TB_RPM_ID,
  		TB_RPM_X_INIT, TB_RPM_Y,
			TB_RPM_X_INIT+(TB_RPM_CHARS*TEXTBOX_FONT_X)-6, TB_RPM_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_RPM_ID, "RPM");
  /* RPM STR*/
  UI_TextboxCreate(&mainWindow, &tbRPMStr, TB_RPM_STR_ID,
  		TB_RPM_STR_X_INIT, TB_RPM_STR_Y,
			TB_RPM_STR_X_INIT+(TB_RPM_STR_CHARS*TEXTBOX_FONT_X)-10, TB_RPM_STR_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_RPM_STR_ID, "5200");
  /* Distance */
  UI_TextboxCreate(&mainWindow, &tbDistance, TB_DISTANCE_ID,
  		TB_DISTANCE_INIT, TB_DISTANCE_Y,
			(TB_DISTANCE_CHARS*TEXTBOX_FONT_X)-50, TB_DISTANCE_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_DISTANCE_ID, "Distancia");
  /* Distance STR */
  UI_TextboxCreate(&mainWindow, &tbDistanceStr, TB_DISTANCE_STR_ID,
  		TB_DISTANCE_STR_INIT, TB_DISTANCE_STR_Y,
			TB_DISTANCE_STR_INIT+(TB_DISTANCE_STR_CHARS*TEXTBOX_FONT_X)-10, TB_DISTANCE_STR_Y+TEXTBOX_FONT_Y);
	UG_TextboxSetText(&mainWindow, TB_DISTANCE_STR_ID, "0");
  /* SetPoint */
  UI_TextboxCreate(&mainWindow, &tbSetPoint, TB_SET_POINT_ID,
  		TB_SET_POINT_INIT, TB_SET_POINT_Y,
			TB_SET_POINT_INIT+(TB_SET_POINT_CHARS*TEXTBOX_FONT_X)-45, TB_SET_POINT_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_SET_POINT_ID, "SetPoint");
  /* SetPoint STR */
  UI_TextboxCreate(&mainWindow, &tbSetPointStr, TB_SET_POINT_STR_ID,
  		TB_SET_POINT_STR_INIT, TB_SET_POINT_STR_Y,
			TB_SET_POINT_STR_INIT+(TB_SET_POINT_STR_CHARS*TEXTBOX_FONT_X)-10, TB_SET_POINT_STR_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_SET_POINT_STR_ID, "500");
  /* Control Action */
  UI_TextboxCreate(&mainWindow, &tbCtrlAction, TB_CTRL_ACTION_ID,
  		TB_CTRL_ACTION_INIT, TB_CTRL_ACTION_Y,
			TB_CTRL_ACTION_INIT+(TB_ACTION_MODE_CHARS*TEXTBOX_FONT_X)-35, TB_CTRL_ACTION_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_CTRL_ACTION_ID, "Act Ctrl");
  /* Control Action STR */
  UI_TextboxCreate(&mainWindow, &tbCtrlActionStr, TB_CTRL_ACT_STR_ID,
  		TB_CTRL_ACT_STR_INIT, TB_CTRL_ACT_STR_Y,
			TB_CTRL_ACT_STR_INIT+(TB_CTRL_ACT_STR_CHARS*TEXTBOX_FONT_X)-10, TB_CTRL_ACT_STR_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_CTRL_ACT_STR_ID, "100");
  /* Action Mode */
  UI_TextboxCreate(&mainWindow, &tbActionMode, TB_ACTION_MODE_ID,
  		TB_ACTION_MODE_INIT, TB_ACTION_MODE_Y,
			TB_ACTION_MODE_INIT+(TB_ACTION_MODE_CHARS*TEXTBOX_FONT_X)-30, TB_ACTION_MODE_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_ACTION_MODE_ID, "Control");
  /* Manual */
  UI_TextboxCreate(&mainWindow, &tbManual, TB_MANUAL_ID,
  		TB_MANUAL_INIT, TB_MANUAL_Y,
			TB_MANUAL_INIT+(TB_MANUAL_CHARS*TEXTBOX_FONT_X)-30, TB_MANUAL_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_MANUAL_ID, "Manual");
  /* PID */
  UI_TextboxCreate(&mainWindow, &tbPID, TB_PID_ID,
  		TB_PID_INIT, TB_PID_Y,
			TB_PID_INIT+(TB_PID_CHARS*TEXTBOX_FONT_X)-10, TB_PID_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_PID_ID, "PID");
  /* USB */
  UI_TextboxCreate(&mainWindow, &tbUSB, TB_USB_ID,
  		TB_USB_INIT, TB_USB_Y,
			TB_USB_INIT+(TB_USB_CHARS*TEXTBOX_FONT_X)-10, TB_USB_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_USB_ID, "USB");
  /* Kp */
  UI_TextboxCreate(&mainWindow, &tbKP, TB_KP_ID,
  		TB_KP_INIT, TB_KP_Y,
			TB_KP_INIT+(TB_KP_CHARS*TEXTBOX_FONT_X)-60, TB_KP_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_KP_ID, "KP: 0.00000");
  /* Ki */
  UI_TextboxCreate(&mainWindow, &tbKI, TB_KI_ID,
  		TB_KI_INIT, TB_KI_Y,
			TB_KI_INIT+(TB_KP_CHARS*TEXTBOX_FONT_X)-60, TB_KI_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_KI_ID, "KI: 0.00000");
  /* Kd */
  UI_TextboxCreate(&mainWindow, &tbKD, TB_KD_ID,
  		TB_KD_INIT, TB_KD_Y,
			TB_KD_INIT+(TB_KD_CHARS*TEXTBOX_FONT_X)-60, TB_KD_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_KD_ID, "KD: 0.00000");

  /* CheckBoxes */
  /* Manual */
  UI_CheckboxCreate(&mainWindow, &cbManual, CB_MANUAL_ID,
  		TB_MANUAL_INIT+(TB_MANUAL_CHARS*TEXTBOX_FONT_X)-25, TB_MANUAL_Y,
			TB_MANUAL_INIT+(TB_MANUAL_CHARS*TEXTBOX_FONT_X)-25, TB_MANUAL_Y);
  UG_CheckboxSetChecked(&mainWindow, CB_MANUAL_ID, 0);
  MainMenu_preCheck(CB_MANUAL_ID);
  /* PID */
  UI_CheckboxCreate(&mainWindow, &cbPID, CB_PID_ID,
  		TB_PID_INIT+(TB_PID_CHARS*TEXTBOX_FONT_X)-5, TB_PID_Y,
			TB_PID_INIT+(TB_PID_CHARS*TEXTBOX_FONT_X), TB_PID_Y);
  UG_CheckboxSetChecked(&mainWindow, CB_PID_ID, 1);
  /* USB */
  UI_CheckboxCreate(&mainWindow, &cbUSB, CB_USB_ID,
  		TB_USB_INIT+(TB_USB_CHARS*TEXTBOX_FONT_X)-5, TB_USB_Y,
			TB_USB_INIT+(TB_USB_CHARS*TEXTBOX_FONT_X)-5, TB_USB_Y);
  UG_CheckboxSetChecked(&mainWindow, CB_USB_ID, 0);
}

result_t MainMenu_setKP(float kp)
{
	result_t result = OK;
	char Buffer[32] = "";

	sprintf(Buffer, "KP: %1.5f", kp);
	result = UG_RESULT_OK == UG_TextboxSetText(&mainWindow, TB_KP_ID, Buffer) ? OK : Error;
	UG_Update();
	return result;
}

result_t MainMenu_setKI(float ki)
{
	result_t result = OK;
	char Buffer[32] = "";

	sprintf(Buffer, "KI: %1.5f", ki);
	result = UG_RESULT_OK == UG_TextboxSetText(&mainWindow, TB_KI_ID, Buffer) ? OK : Error;
	UG_Update();
	return result;
}

result_t MainMenu_setKD(float kd)
{
	result_t result = OK;
	char Buffer[32] = "";

	sprintf(Buffer, "KD: %1.5f", kd);
	result = UG_RESULT_OK == UG_TextboxSetText(&mainWindow, TB_KD_ID, Buffer) ? OK : Error;
	UG_Update();
	return result;
}

result_t MainMenu_setControlConstants(float kp, float ki, float kd)
{
	if(OK != MainMenu_setKP(kp))
		return Error;
	if(OK != MainMenu_setKI(ki))
		return Error;
	if(OK != MainMenu_setKD(kd))
		return Error;
	return OK;
}

result_t MainMenu_setDistance(int16_t distance)
{
	UG_U8 progress = 0;
	char Buffer[8] = "";
	/* COM Error Check */
	if(COM_MSG_ERROR_CODE == distance)
		distance = 0; /* Error Handling */
	/* Changing the Textbox */
	sprintf(Buffer, "%d", distance);
	if(UG_RESULT_OK != UG_TextboxSetText(&mainWindow, TB_DISTANCE_STR_ID, Buffer))
		return Error;
	/* Making percentage */
	progress = distance * 100 / MAX_DISTANCE;
	/* Changing the Progress bar */
	if(UG_RESULT_OK != UG_ProgressSetProgress(&mainWindow, PROGRESS_BAR_DISTANCE_ID, progress))
		return Error;
	UG_Update();
	return OK;
}

result_t MainMenu_setSetPoint(uint16_t setPoint)
{
	UG_U8 progress = 0;
	char Buffer[8] = "";

	sprintf(Buffer, "%d", setPoint);
	if(UG_RESULT_OK != UG_TextboxSetText(&mainWindow, TB_SET_POINT_STR_ID, Buffer))
		return Error;
	/* Making percentage */
	progress = setPoint * 100 / MAX_DISTANCE;
	/* Changing the Progress bar */
	if(UG_RESULT_OK != UG_ProgressSetProgress(&mainWindow, PROGRESS_BAR_SET_POINT_ID, progress))
		return Error;
	/* Send to COM */
	COM_SendMessage(SET_POINT, MSG_TYPE_ONDEMAND, PRIORITY_HIGH, setPoint);
	UG_Update();
	return OK;
}

result_t MainMenu_setRPM(int16_t rpm)
{
	UG_U8 progress = 0;
	char Buffer[8] = "";

	if(COM_MSG_ERROR_CODE == rpm)
		rpm = 0;/* Error Handling */ /* TODO: Consider put "Error" */
	sprintf(Buffer, "%d", rpm);
	if(UG_RESULT_OK != UG_TextboxSetText(&mainWindow, TB_RPM_STR_ID, Buffer))
		return Error;
	/* Making percentage */
	progress = rpm * 100 / MAX_RPM;
	/* Changing the Progress bar */
	if(UG_RESULT_OK != UG_ProgressSetProgress(&mainWindow, PROGRESS_BAR_RPM_ID, progress))
		return Error;
	UG_Update();
	return OK;
}

result_t MainMenu_setActionControl(int8_t actionControl)
{
	char Buffer[8] = "";

	if(COM_MSG_ERROR_CODE == actionControl)
		actionControl = 0;/* Error Handling */ /* TODO: Consider put "Error" */
	sprintf(Buffer, "%d", actionControl);
	if(UG_RESULT_OK != UG_TextboxSetText(&mainWindow, TB_CTRL_ACT_STR_ID, Buffer))
		return Error;
	/* Changing the Progress bar */
	if(UG_RESULT_OK != UG_ProgressSetProgress(&mainWindow, PROGRESS_BAR_ACTION_CONTROL_ID, actionControl))
		return Error;
	UG_Update();
	return OK;
}

result_t MainMenu_preCheck(uint16_t ID)
{
	/* TODO Implement the function */
	UG_CheckboxSetAlternateForeColor(&mainWindow, ID, C_RED);
	UG_CheckboxSetForeColor(&mainWindow, ID, C_RED);
	return OK;
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
static void sMainMenu_ProcessButtonPress(Buttons btnPressed, int16_t *setPoint)
{
	switch(btnPressed)
	{
		case iOk:
		case iEncoderSW:
		break;
		case iUp:
			*setPoint = *setPoint + 1;
			if(MAX_DISTANCE < *setPoint)
				*setPoint = MAX_DISTANCE;
			MainMenu_setSetPoint(*setPoint);
		break;
		case iDown:
			*setPoint = *setPoint - 1;
			if(0 > *setPoint)
				*setPoint = 0;
			MainMenu_setSetPoint(*setPoint);
		break;
		case iLeft:
		break;
		case iRight:
		break;
		case iMenu:
		case iNone:
		case iEncoder:
		case Reset:
		default:
		break;
	}
}
