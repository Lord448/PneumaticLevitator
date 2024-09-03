/**
 * @file      ControlModeMenu.c
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

#include "ControlModeMenu.h"

extern osEventFlagsId_t xEvent_CurrentControlModeHandle;

extern bool MainMenu_firstChangeMode;

UG_WINDOW controlModesWindow;

char ManualModeBtnStr[] = "Modo Manual";
char PIDModeBtnStr[] = "PID";
char SlaveModeBtnStr[] = "Matlab (USB)";

const uint8_t borderOffset = 1;

typedef struct BtnCoordinates {
	uint8_t xs;
	uint8_t xe;
	uint8_t ys;
	uint8_t ye;
}BtnCoordinates;

/* Must be the same that Motherboard/ModeManager.h typedef */
typedef enum btnSelect {
	ManualBtnSel = BTN_MANUAL_MODE,
	PIDBtnSel = BTN_AUTO_PID_MODE,
	SlaveBtnSel = BTN_SLAVE_MODE,
	None = -1 /* Forcing the enum to be signed */
}BtnSelect;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sControlModeMenu_SetFlags(uint8_t id);
static void sControlModeMenu_ButtonSetCursor(uint8_t id);
static void sControlModeMenu_ButtonPressAnim(uint8_t id);
static void sControlModeMenu_SetSelectedText(uint8_t id);
static void sControlModeMenu_ButtonSetPress(bool state, uint8_t id);
static void sControlModeMenu_ButtonCreate(UG_BUTTON *btn, uint8_t id, char *str, uint16_t xs, uint16_t ys);
static void sControlModeMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
static BtnCoordinates sControlModeMenu_GetCoordinates(uint8_t id);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief
 * @param  btnPress : Button that has been pressed
 * @retval none
 */
void ControlModeMenu_Dynamics(Buttons btnPress, bool *isFirstInit)
{
	BtnCoordinates initFrameCoord;
	uint32_t flags = 0;
	static BtnSelect currentSel = ManualBtnSel;

	if(*isFirstInit)
	{
		/* First entry to the function */
		*isFirstInit = false;
		HMI_DisableButtons(iLeft | iRight);
		HMI_EnableButtons(iUp | iDown);
		initFrameCoord = sControlModeMenu_GetCoordinates(BTN_MANUAL_MODE_Y_POS);
		sControlModeMenu_DrawFrame(initFrameCoord);
		flags = osEventFlagsGet(xEvent_CurrentControlModeHandle);
		if(flags&MODE_MANUAL_FLAG)
		{
			/* Manual mode selected on the system */
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "Manual");
		}
		if(flags&MODE_PID_FLAG)
		{
			/* PID mode selected on the system */
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "PID");
		}
		if(flags&MODE_USB_FLAG)
		{
			/* USB mode selected on the system */
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "Matlab (USB)");
		}
		UG_Update();
	}
	else
	{
		/* Normal code flow */
		if(iNone != btnPress)
		{
			/* There is a press */
			if(iUp == btnPress)
			{
				/* Up Button pressed */
				currentSel--;
				if(currentSel < ManualBtnSel)
					currentSel = SlaveBtnSel;
			}
			else if(iDown == btnPress)
			{
				/* Down Button pressed */
				currentSel++;
				if(currentSel > SlaveBtnSel)
					currentSel = ManualBtnSel;
			}
			else if(iOk == btnPress || iEncoderSW == btnPress)
			{
				/* Select Button pressed */
				sControlModeMenu_SetSelectedText((uint8_t)currentSel);
				sControlModeMenu_ButtonPressAnim((uint8_t)currentSel);
				/* Notify to the system */
				sControlModeMenu_SetFlags((uint8_t)currentSel);
				/* Send to COM the message */
				COM_SendMessage(MODE_SELECTION, PRIORITY_HIGH, MSG_TYPE_ONDEMAND, currentSel);
			}
			else
			{
				/* Do Nothing */
			}
			sControlModeMenu_ButtonSetCursor((uint8_t)currentSel);
		}
		else
		{
			/* Do Nothing */
		}
	}
}

/**
 * @brief
 * @param  none
 * @retval none
 */
void ControlModeMenu_buildObjects(void)
{
	static UG_OBJECT  ObjWinBuf[8];
	/* TextButtons */
	static UG_BUTTON  btnManual;
	static UG_BUTTON  btnAutoPID;
	static UG_BUTTON  btnSlave;
	/* TextBoxes */
	static UG_TEXTBOX tbSelect;
	static UG_TEXTBOX tbCurrentSel;

	UG_WindowCreate(&controlModesWindow, ObjWinBuf, 8, NULL);
	/* Set window characteristics */
	UG_WindowSetTitleText(&controlModesWindow, "Modo de control");
  UG_WindowSetTitleTextFont(&controlModesWindow, FONT_6X8);
  UG_WindowSetTitleHeight(&controlModesWindow, 15);
  UG_WindowSetXStart(&controlModesWindow, 0);
  UG_WindowSetYStart(&controlModesWindow, 0);
  UG_WindowSetXEnd(&controlModesWindow, LCD_WIDTH-1);
  UG_WindowSetYEnd(&controlModesWindow, LCD_HEIGHT-1);
  UG_WindowSetBackColor(&controlModesWindow, C_WHITE);
  /* Building textbuttons */
  sControlModeMenu_ButtonCreate(&btnManual, BTN_MANUAL_MODE, ManualModeBtnStr, CONTROL_MENU_TEXTBOX_X_POS, BTN_MANUAL_MODE_Y_POS);
  sControlModeMenu_ButtonCreate(&btnAutoPID, BTN_AUTO_PID_MODE, PIDModeBtnStr, CONTROL_MENU_TEXTBOX_X_POS, BTN_PID_MODE_Y_POS);
  sControlModeMenu_ButtonCreate(&btnSlave, BTN_SLAVE_MODE, SlaveModeBtnStr, CONTROL_MENU_TEXTBOX_X_POS, BTN_SLAVE_MODE_Y_POS);
  /* Building textboxes */
  sControlModeMenu_CreateTextbox(&tbCurrentSel, TB_CURRENT_SELECT_ID, "Modo Actual", true, 0, TB_SELECT_Y_POS-CONTROL_MENU_TEXTBOX_FONT_Y-15);
  sControlModeMenu_CreateTextbox(&tbSelect, TB_SELECT_ID, "Manual", true, 0, TB_SELECT_Y_POS);
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief
 * @param  id : Id of the button
 * @retval none
 */
static void sControlModeMenu_SetFlags(uint8_t id)
{
	osEventFlagsClear(xEvent_CurrentControlModeHandle, MODE_MANUAL_FLAG | MODE_PID_FLAG | MODE_USB_FLAG);
	switch(id)
	{
		case BTN_MANUAL_MODE:
			osEventFlagsSet(xEvent_CurrentControlModeHandle, MODE_MANUAL_FLAG);
			MainMenu_setManualChecked(true);
			MainMenu_setPIDChecked(false);
			MainMenu_setUSBChecked(false);
		break;
		case BTN_AUTO_PID_MODE:
			osEventFlagsSet(xEvent_CurrentControlModeHandle, MODE_PID_FLAG);
			MainMenu_setManualChecked(false);
			MainMenu_setPIDChecked(true);
			MainMenu_setUSBChecked(false);
		break;
		case BTN_SLAVE_MODE:
			osEventFlagsSet(xEvent_CurrentControlModeHandle, MODE_USB_FLAG);
			MainMenu_setManualChecked(false);
			MainMenu_setPIDChecked(false);
			MainMenu_setUSBChecked(true);
		break;
		default:
		break;
	}
	MainMenu_firstChangeMode = true;
}

/**
 * @brief
 * @param  id : Id of the button
 * @retval none
 */
static void sControlModeMenu_ButtonSetCursor(uint8_t id)
{
	BtnCoordinates currentCoord = sControlModeMenu_GetCoordinates(id);
	static BtnCoordinates pastCoord = {
			.xs = 0,
			.ys = 0,
			.xe = 0,
			.ye = 0
	};

	sControlModeMenu_DrawFrame(currentCoord);
	sControlModeMenu_EraseFrame(pastCoord);
	pastCoord = currentCoord;
	UG_Update();
}

/**
 * @brief
 * @param  id : Id of the button
 * @retval none
 */
static void sControlModeMenu_ButtonPressAnim(uint8_t id)
{
	const TickType_t AnimDelay = 100; /* MS */
	sControlModeMenu_ButtonSetPress(true, id);
	osDelay(pdMS_TO_TICKS(AnimDelay));
	sControlModeMenu_ButtonSetPress(false, id);
}

/**
 * @brief
 * @param  id : Id of the button
 * @retval none
 */
static void sControlModeMenu_SetSelectedText(uint8_t id)
{
	switch(id)
	{
		case BTN_MANUAL_MODE:
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "Manual");
		break;
		case BTN_AUTO_PID_MODE:
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "PID");
		break;
		case BTN_SLAVE_MODE:
			UG_TextboxSetText(&controlModesWindow, TB_SELECT_ID, "Matlab (USB)");
		break;
		default:
		break;
	}
	UG_Update();
}

/**
 * @brief
 * @param  state : true if pressed
 * @param  id : Id of the button
 * @retval none
 */
static void sControlModeMenu_ButtonSetPress(bool state, uint8_t id)
{
	if(state)
	{
		/* Button press */
		UG_ButtonSetStyle(&controlModesWindow, id, BTN_STYLE_2D);
	}
	else
	{
		/* Button release */
		UG_ButtonSetStyle(&controlModesWindow, id, BTN_STYLE_3D);
	}
	UG_Update();
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @param
 * @param
 * @retval none
 */
static void sControlModeMenu_ButtonCreate(UG_BUTTON *btn, uint8_t id, char *str, uint16_t xs, uint16_t ys)
{
	uint16_t xe = xs+(strlen(str)*(CONTROL_MENU_TEXTBOX_FONT_X-5));
	uint16_t ye = ys+CONTROL_MENU_TEXTBOX_FONT_Y+10;

	UG_ButtonCreate(&controlModesWindow, btn, id, xs, ys, xe, ye);
	UG_ButtonSetFont(&controlModesWindow, id, CONTROL_MENU_TEXTBOX_FONT);
	UG_ButtonSetForeColor(&controlModesWindow, id, C_BLACK);
	UG_ButtonSetBackColor(&controlModesWindow, id, C_WHITE_94);
	UG_ButtonSetStyle(&controlModesWindow, id, BTN_STYLE_3D);
	UG_ButtonSetText(&controlModesWindow, id, str);
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 * @retval none
 */
static void sControlModeMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys)
{
	//uint16_t xe = xs + ((CONTROL_MENU_TEXTBOX_FONT_X-10) * strlen(str));
	uint16_t xe = LCD_WIDTH-10;
	uint16_t ye = ys + CONTROL_MENU_TEXTBOX_FONT_Y;

	UI_TextboxCreate(&controlModesWindow, txb, id, xs, ys, xe, ye);
	UG_TextboxSetFont(&controlModesWindow, id, MENU_TEXTBOX_FONT);
	UG_TextboxSetText(&controlModesWindow, id, str);
	if(!show)
	{
		UG_TextboxHide(&controlModesWindow, id);
	}
}

/**
 * @brief
 * @param  id : ID of the button
 * @retval A structure with the coordinates
 *         of the button
 */
static BtnCoordinates sControlModeMenu_GetCoordinates(uint8_t id)
{

	BtnCoordinates retval;

	retval.xs = CONTROL_MENU_TEXTBOX_X_POS+1;
	switch(id)
	{
		case BTN_MANUAL_MODE:
			retval.ys = BTN_MANUAL_MODE_Y_POS+borderOffset+17;
			retval.xe = retval.xs+(strlen(ManualModeBtnStr)*(CONTROL_MENU_TEXTBOX_FONT_X-5))+borderOffset+3;
			retval.ye = retval.ys+CONTROL_MENU_TEXTBOX_FONT_Y+10+borderOffset+1;
		break;
		case BTN_AUTO_PID_MODE:
			retval.ys = BTN_PID_MODE_Y_POS+borderOffset+17;
			retval.xe = retval.xs+(strlen(PIDModeBtnStr)*(CONTROL_MENU_TEXTBOX_FONT_X-5))+borderOffset+3;
			retval.ye = retval.ys+CONTROL_MENU_TEXTBOX_FONT_Y+10+borderOffset+1;
		break;
		case BTN_SLAVE_MODE:
			retval.ys = BTN_SLAVE_MODE_Y_POS+borderOffset+17;
			retval.xe = retval.xs+(strlen(SlaveModeBtnStr)*(CONTROL_MENU_TEXTBOX_FONT_X-5))+borderOffset+3;
			retval.ye = retval.ys+CONTROL_MENU_TEXTBOX_FONT_Y+10+borderOffset+1;
		break;
		default:
			/* Do Nothing */
		break;
	}
	return retval;
}
