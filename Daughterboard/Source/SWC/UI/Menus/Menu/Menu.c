/**
 * @file      Menu.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
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

#include "Menu.h"

extern osMessageQueueId_t xFIFO_ButtonsHandle;

UG_WINDOW menuWindow;

typedef struct MenuSelector {
	MenuStages menuStage;
	uint8_t idImage;
	uint8_t idTextbox;
}MenuSelector;

/* Menu Handlers */
MenuSelector mSelMainLobby = {
	.menuStage = sMainLobby,
	.idImage = MAIN_LOBBY_MENU_IMG_ID,
	.idTextbox = TB_MAIN_LOBBY_ID
};

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sMenu_MakeArrowPressAnim(Buttons side);
static void sMenu_ProcessButtonPress(Buttons btn);
static void sMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
static void sMenu_MakeSelectedAnim(MenuStages menu);
static void sMenu_HideAllTextboxes(void);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void Menu_MenuDynamics(void)
{
	enum states {
		Init,
		Periodic
	}static stateHandler = Init;
	Buttons btnPressed = None;

	osMessageQueueGet(xFIFO_ButtonsHandle, &btnPressed, NULL, osWaitForever);
	switch(stateHandler)
	{
		case Init:
			HMI_DisableButtons(iUp | iDown | iEncoder);
			stateHandler = Periodic;
		break;
		case Periodic:
			if(None != btnPressed)
			{
				/* There is a press */
				sMenu_MakeArrowPressAnim(btnPressed);
				sMenu_ProcessButtonPress(btnPressed);
				btnPressed = None;
			}
		break;
		default:

		break;
	}
}

void Menu_buildObjects(void)
{
	static UG_OBJECT ObjWinBuf[WINDOW_MAX_OBJECTS];
	/* Images */
	static UG_IMAGE   LeftArrowImage;
	static UG_IMAGE   PressedLeftArrowImage;
	static UG_IMAGE   RightArrowImage;
	static UG_IMAGE   PressedRightArrowImage;

	static UG_IMAGE   MainLobbyImage;
	static UG_IMAGE   ConfigImage;
	static UG_IMAGE   AboutImage;
	static UG_IMAGE   PlotImage;
	static UG_IMAGE   USBConfigImage;
	static UG_IMAGE   PlantAnalysisImage;
	/* Textboxes */
	static UG_TEXTBOX tbMainLobby;
	static UG_TEXTBOX tbConfig;
	static UG_TEXTBOX tbAbout;
	static UG_TEXTBOX tbPlot;
	static UG_TEXTBOX tbUSBConfig;
	static UG_TEXTBOX tbPlantAnalysis;
	static UG_TEXTBOX tbEarlyVersionPrompt;

	UG_WindowCreate(&menuWindow, ObjWinBuf, WINDOW_MAX_OBJECTS, NULL);
	/* Set window characteristics */
	UG_WindowSetTitleText(&menuWindow, "Menu");
  UG_WindowSetTitleTextFont(&menuWindow, FONT_6X8);
  UG_WindowSetTitleHeight(&menuWindow, 15);
  UG_WindowSetXStart(&menuWindow, 0);
  UG_WindowSetYStart(&menuWindow, 0);
  UG_WindowSetXEnd(&menuWindow, LCD_WIDTH-1);
  UG_WindowSetYEnd(&menuWindow, LCD_HEIGHT-1);
  UG_WindowSetBackColor(&menuWindow, C_WHITE);
  /* Building images */
  UI_CreateImage(&menuWindow, &LeftArrowImage, LEFT_ARROW_IMG_ID, &LeftArrow, true, LEFT_ARROW_X_POS, LEFT_ARROW_Y_POS);
  UI_CreateImage(&menuWindow, &PressedLeftArrowImage, PRESSED_LEFT_ARROW_IMG_ID, &PressedLeftArrow, false, LEFT_ARROW_X_POS, LEFT_ARROW_Y_POS);
  UI_CreateImage(&menuWindow, &RightArrowImage, RIGHT_ARROW_IMG_ID, &RightArrow, true, RIGHT_ARROW_X_POS, RIGHT_ARROW_Y_POS);
  UI_CreateImage(&menuWindow, &PressedRightArrowImage, PRESSED_RIGHT_ARROW_IMG_ID, &PressedRightArrow, false, RIGHT_ARROW_X_POS, RIGHT_ARROW_Y_POS);
  UI_CreateImage(&menuWindow, &MainLobbyImage, MAIN_LOBBY_MENU_IMG_ID, &MainLobby, true, ICON_X_POS, ICON_Y_POS);
  UI_CreateImage(&menuWindow, &ConfigImage, CONFIG_MENU_IMG_ID, &Config, false, ICON_X_POS, ICON_Y_POS);
  UI_CreateImage(&menuWindow, &AboutImage, ABOUT_MENU_IMG_ID, &About, false, ICON_X_POS, ICON_Y_POS);
  UI_CreateImage(&menuWindow, &PlotImage, PLOT_MENU_IMG_ID, &Plot, false, ICON_X_POS, ICON_Y_POS);
  UI_CreateImage(&menuWindow, &USBConfigImage, USB_CONFIG_MENU_IMG_ID, &USBConfig, false, ICON_X_POS, ICON_Y_POS);
  UI_CreateImage(&menuWindow, &PlantAnalysisImage, PLANT_MENU_IMG_ID, &PlantAnalysis, false, ICON_X_POS, ICON_Y_POS);
  /* Building Textboxes */
  sMenu_CreateTextbox(&tbMainLobby, TB_MAIN_LOBBY_ID, "Lobby Principal", true, TB_MAIN_LOBBY_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbConfig, TB_CONFIG_ID, "Configuraciones", false, TB_MAIN_LOBBY_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbAbout, TB_ABOUT_ID, "Acerca de", false, TB_ABOUT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbPlot, TB_PLOT_ID, "Graficar", false, TB_PLOT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbUSBConfig, TB_USB_CONFIG_ID, "Configuraciones del USB", false, TB_USB_CONFIG_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbPlantAnalysis, TB_PLANT_ID, "Analisis de Planta", false, TB_PLANT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbEarlyVersionPrompt, TB_VER_PROMPT_ID, "En desarrollo", false, TB_VER_PROMPT_X_POS, TB_VER_PROMPT_Y_POS);
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */

static void sMenu_MakeArrowPressAnim(Buttons side)
{
	const TickType_t AnimDelay = 100; /* MS */
	if(iLeft == side)
	{
		/* Left button has been pressed */
		UG_ImageShow(&menuWindow, PRESSED_LEFT_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&menuWindow, LEFT_ARROW_IMG_ID);
		UG_Update();
	}
	else if(iRight == side)
	{
		/* Right button has been pressed */
		UG_ImageShow(&menuWindow, PRESSED_RIGHT_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&menuWindow, RIGHT_ARROW_IMG_ID);
		UG_Update();
	}
	else
	{
		/* Do Nothing */
	}

}

static void sMenu_ProcessButtonPress(Buttons btn)
{
	const MenuStages firstMenu = sMainLobby;
	const MenuStages lastMenu = sPlantAnalysis;
	//static MenuSelector *currentMenu = &msM;
	static MenuStages currentMenuSel = sMainLobby;

	if(iLeft == btn)
	{
		/* Change image menu sel */
		currentMenuSel--;
		if(currentMenuSel < firstMenu)
			currentMenuSel = lastMenu;
		if(sMenu == currentMenuSel)
			currentMenuSel--;
	}
	else if(iRight == btn)
	{
		/* Change image menu sel */
		currentMenuSel++;
		if(currentMenuSel > lastMenu)
			currentMenuSel = firstMenu;
		if(sMenu == currentMenuSel)
			currentMenuSel++;
	}
	else if(iOk == btn || iEncoderSW == btn)
	{
		/* Enter to the menu  */
		sMenu_MakeSelectedAnim(currentMenuSel);

		return; /* Skip Image processing */
	}
	else
	{
		/* Do Nothing */
	}
	sMenu_HideAllTextboxes();
	switch(currentMenuSel)
	{
		case sMainLobby:
			UG_ImageShow(&menuWindow, MAIN_LOBBY_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_MAIN_LOBBY_ID);
			UG_Update();
		break;
		case sConfiguration:
			UG_ImageShow(&menuWindow, CONFIG_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_CONFIG_ID);
			UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID); /* TODO: Remove when finished */
			UG_Update();
		break;
		case sAbout:
			UG_ImageShow(&menuWindow, ABOUT_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_ABOUT_ID);
			UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID); /* TODO: Remove when finished */
			UG_Update();
		break;
		case sPlot:
			UG_ImageShow(&menuWindow, PLOT_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_PLOT_ID);
			UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID); /* TODO: Remove when finished */
			UG_Update();
		break;
		case sUSBConfig:
			UG_ImageShow(&menuWindow, USB_CONFIG_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_USB_CONFIG_ID);
			UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID); /* TODO: Remove when finished */
			UG_Update();
		break;
		case sPlantAnalysis:
			UG_ImageShow(&menuWindow, PLANT_MENU_IMG_ID);
			UG_TextboxShow(&menuWindow, TB_PLANT_ID);
			UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID); /* TODO: Remove when finished */
			UG_Update();
		break;
		case sMenu:
		default:
		break;
	}
}

static void sMenu_MakeSelectedAnim(MenuStages menu)
{
	const TickType_t AnimDelay = 45; /* x2 MS */
	const uint16_t AnimTimes = 3;
	uint8_t id;

	/* Choosing the ID */
	switch(menu)
	{
		case sMainLobby:
			id = MAIN_LOBBY_MENU_IMG_ID;
		break;
		case sConfiguration:
			id =  CONFIG_MENU_IMG_ID;
		break;
		case sAbout:
			id = ABOUT_MENU_IMG_ID;
		break;
		case sPlot:
			id = PLOT_MENU_IMG_ID;
		break;
		case sUSBConfig:
			id = USB_CONFIG_MENU_IMG_ID;
		break;
		case sPlantAnalysis:
			id = PLANT_MENU_IMG_ID;
		break;
		case sMenu:
		default:
		break;
	}

	/* Making the animation */
	for(uint16_t i = 0; i < AnimTimes; i++)
	{
		UG_FillFrame(ICON_X_POS, ICON_Y_POS, ICON_X_POS+120, ICON_Y_POS+120, C_WHITE);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&menuWindow, id);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
	}
}

static void sMenu_HideAllTextboxes(void)
{
	UG_TextboxHide(&menuWindow, TB_MAIN_LOBBY_ID);
	UG_TextboxHide(&menuWindow, TB_CONFIG_ID);
	UG_TextboxHide(&menuWindow, TB_ABOUT_ID);
	UG_TextboxHide(&menuWindow, TB_PLOT_ID);
	UG_TextboxHide(&menuWindow, TB_USB_CONFIG_ID);
	UG_TextboxHide(&menuWindow, TB_PLANT_ID);
	UG_TextboxHide(&menuWindow, TB_VER_PROMPT_ID);
	UG_Update();
}

static void sMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys)
{
	uint16_t xe = xs + ((MENU_TEXTBOX_FONT_X-10) * strlen(str));
	uint16_t ye = ys + MENU_TEXTBOX_FONT_Y;

	UI_TextboxCreate(&menuWindow, txb, id, xs, ys, xe, ye);
	UG_TextboxSetFont(&menuWindow, id, MENU_TEXTBOX_FONT);
	UG_TextboxSetText(&menuWindow, id, str);
	if(!show)
	{
		UG_TextboxHide(&menuWindow, id);
	}
}
