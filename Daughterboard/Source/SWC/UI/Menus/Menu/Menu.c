/**
 * @file      Menu.c
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

#include "Menu.h"

extern osMessageQueueId_t xFIFO_ButtonsHandle;

/**
 * ---------------------------------------------------------
 * 					   SOFTWARE COMPONENT GLOBALS
 * ---------------------------------------------------------
 */

UG_WINDOW menuWindow;

typedef struct MenuSelector {
	MenuStages menuStage;
	uint8_t idImage;
	uint8_t idTextbox;
	bool isOnDevelopment;
}MenuSelector;

/* Menu Handlers */
/**
 * @brief Grouping all the menu structures on a mega structure
 *        in order to force the linker to save all the structures
 *        under contiguous RAM Addresses and perform safe pointer
 *        arithmetics through the whole group
 */
struct MenuSelectorsGroup {
	MenuSelector mSelMainLobby;
	MenuSelector mSelConfigs;
	MenuSelector mSelAbout;
	MenuSelector mSelLCDTest;
	MenuSelector mSelPlot;
	MenuSelector mSelUSBConfigs;
	MenuSelector mSelPlantAnalysis;
}static MenuSelectorsGroup = {
		.mSelMainLobby = {
			.menuStage = sMainLobby,
			.idImage = MAIN_LOBBY_MENU_IMG_ID,
			.idTextbox = TB_MAIN_LOBBY_ID,
			.isOnDevelopment = false
		},
		.mSelConfigs = {
			.menuStage = sConfigs,
			.idImage = CONFIG_MENU_IMG_ID,
			.idTextbox = TB_CONFIG_ID,
			.isOnDevelopment = false
		},
		.mSelAbout = {
			.menuStage = sAbout,
			.idImage = ABOUT_MENU_IMG_ID,
			.idTextbox = TB_ABOUT_ID,
			.isOnDevelopment = false
		},
		.mSelLCDTest= {
			.menuStage = sLCDTest,
			.idImage = LCD_TEST_IMG_ID,
			.idTextbox = TB_LCD_TEST_ID,
			.isOnDevelopment = false
		},
		.mSelPlot = {
			.menuStage = sPlot,
			.idImage = PLOT_MENU_IMG_ID,
			.idTextbox = TB_PLOT_ID,
			.isOnDevelopment = true /* TODO: Remove when finished */
		},
		.mSelUSBConfigs = {
			.menuStage = sUSBConfig,
			.idImage = USB_CONFIG_MENU_IMG_ID,
			.idTextbox = TB_USB_CONFIG_ID,
			.isOnDevelopment = true /* TODO: Remove when finished */
		},
		.mSelPlantAnalysis = {
			.menuStage = sPlantAnalysis,
			.idImage = PLANT_MENU_IMG_ID,
			.idTextbox = TB_PLANT_ID,
			.isOnDevelopment = true /* TODO: Remove when finished */
		}
};

static MenuSelector *currentMenu = &MenuSelectorsGroup.mSelMainLobby;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static bool sMenu_ProcessButtonPress(Buttons btn);
static void sMenu_MakeArrowPressAnim(Buttons side);
static void sMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
static void sMenu_MakeSelectedAnim(MenuSelector *menu);
static void sMenu_HideAllTextboxes(void);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  This function makes all the animations and
 *         processes the button pressions
 * @param  none
 * @retval none
 */
void Menu_MenuDynamics(void)
{
	enum states {
		Init,
		Periodic
	}static stateHandler = Init;
	Buttons btnPressed = iNone;

	switch(stateHandler)
	{
		case Init:
			HMI_EnableButtons(iLeft | iRight | iEncoderSW | iOk | iMenu);
			UG_ImageShow(&menuWindow, currentMenu->idImage);
			UG_Update();
			stateHandler = Periodic;
		break;
		case Periodic:
			osMessageQueueGet(xFIFO_ButtonsHandle, &btnPressed, NULL, osWaitForever);
			if(iNone != btnPressed)
			{
				/* There is a press */
				sMenu_MakeArrowPressAnim(btnPressed);
				if(true == sMenu_ProcessButtonPress(btnPressed))
				{
					/* Registered a menu selection */
					stateHandler = Init;
				}
				btnPressed = iNone;
			}
		break;
		default:
			/* Do Nothing */
		break;
	}
}

/**
 * @brief  Build and declare all the graphical
 *         resources needed by this component
 * @param  none
 * @retval none
 */
void Menu_buildObjects(void)
{
	static UG_OBJECT  ObjWinBuf[32];
	/* Images */
	static UG_IMAGE   LeftArrowImage;
	static UG_IMAGE   PressedLeftArrowImage;
	static UG_IMAGE   RightArrowImage;
	static UG_IMAGE   PressedRightArrowImage;

	static UG_IMAGE   MainLobbyImage;
	static UG_IMAGE   ConfigImage;
	static UG_IMAGE   AboutImage;
	static UG_IMAGE   LCDTestImage;
	/* Commented in order to save FLASH */
	//static UG_IMAGE   PlotImage;
	//static UG_IMAGE   USBConfigImage;
	//static UG_IMAGE   PlantAnalysisImage;
	/* Textboxes */
	static UG_TEXTBOX tbMainLobby;
	static UG_TEXTBOX tbConfig;
	static UG_TEXTBOX tbLCDTest;
	static UG_TEXTBOX tbAbout;
	static UG_TEXTBOX tbPlot;
	static UG_TEXTBOX tbUSBConfig;
	static UG_TEXTBOX tbPlantAnalysis;
	static UG_TEXTBOX tbEarlyVersionPrompt;

	UG_WindowCreate(&menuWindow, ObjWinBuf, 32, NULL);
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
  UI_CreateImage(&menuWindow, &LCDTestImage, LCD_TEST_IMG_ID, &LCDTest, false, ICON_X_POS, ICON_Y_POS);
  /* Commented in order to save FLASH */
  //UI_CreateImage(&menuWindow, &PlotImage, PLOT_MENU_IMG_ID, &Plot, false, ICON_X_POS, ICON_Y_POS);
  //UI_CreateImage(&menuWindow, &USBConfigImage, USB_CONFIG_MENU_IMG_ID, &USBConfig, false, ICON_X_POS, ICON_Y_POS);
  //UI_CreateImage(&menuWindow, &PlantAnalysisImage, PLANT_MENU_IMG_ID, &PlantAnalysis, false, ICON_X_POS, ICON_Y_POS);
  /* Building Textboxes */
  sMenu_CreateTextbox(&tbMainLobby, TB_MAIN_LOBBY_ID, "Lobby Principal", true, TB_MAIN_LOBBY_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbConfig, TB_CONFIG_ID, "Configuraciones", false, TB_MAIN_LOBBY_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbAbout, TB_ABOUT_ID, "Acerca de", false, TB_ABOUT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbPlot, TB_PLOT_ID, "Graficar", false, TB_PLOT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbUSBConfig, TB_USB_CONFIG_ID, "Configuraciones del USB", false, TB_USB_CONFIG_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbPlantAnalysis, TB_PLANT_ID, "Analisis de Planta", false, TB_PLANT_X_POS, MENU_TEXBOX_Y_POS);
  sMenu_CreateTextbox(&tbEarlyVersionPrompt, TB_VER_PROMPT_ID, "En desarrollo", false, TB_VER_PROMPT_X_POS, TB_VER_PROMPT_Y_POS);

	UI_TextboxCreate(&menuWindow, &tbLCDTest, TB_LCD_TEST_ID,
			TB_LCD_TEST_X_POS, MENU_TEXBOX_Y_POS,
			TB_LCD_TEST_X_POS+((MENU_TEXTBOX_FONT_X-8) * strlen("Prueba de LCD")), MENU_TEXBOX_Y_POS+MENU_TEXTBOX_FONT_Y);
	UG_TextboxSetFont(&menuWindow, TB_LCD_TEST_ID, MENU_TEXTBOX_FONT);
	UG_TextboxSetText(&menuWindow, TB_LCD_TEST_ID, "Prueba de LCD");
	UG_TextboxHide(&menuWindow, TB_LCD_TEST_ID);
}

void Menu_ShowInitImage(void)
{
	UG_ImageShow(&menuWindow, MAIN_LOBBY_MENU_IMG_ID);
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  This function implements a carrousel for the
 *         menu selection
 * @param  btn : Button that have been pressed
 * @retval bool : the result of the pression
 *             true  -> you need to change menu
 *						 false -> keep in the same menu
 */
static bool sMenu_ProcessButtonPress(Buttons btn)
{
	MenuSelector *firstMenu = &MenuSelectorsGroup.mSelMainLobby;
	MenuSelector *lastMenu = &MenuSelectorsGroup.mSelPlantAnalysis;

	if(iLeft == btn)
	{
		/* Change image menu sel */
		if(currentMenu->menuStage == firstMenu->menuStage)
		{
			/* We are on the low bound of the carousel */
			currentMenu = lastMenu;
		}
		else
		{
			/* We are inside of the bounds */
			currentMenu--;
		}
		/* TODO: Disabling beta menus */
		if(sPlantAnalysis == currentMenu->menuStage)
		{
			/* Skip */
			currentMenu = &MenuSelectorsGroup.mSelLCDTest;
		}
		else
		{
			/* Do Nothing */
		}
	}
	else if(iRight == btn)
	{
		/* Change image menu sel */
		if(currentMenu->menuStage == lastMenu->menuStage)
		{
			/* We are on the top bound of the carousel */
			currentMenu = firstMenu;
		}
		else
		{
			/* We are inside of the bounds */
			currentMenu++;
		}
		/* TODO: Disabling beta menus */
		if(sPlot == currentMenu->menuStage)
		{
			/* Skip */
			currentMenu = &MenuSelectorsGroup.mSelMainLobby;
		}
		else
		{
			/* Do Nothing */
		}
	}
	else if(iOk == btn || iEncoderSW == btn)
	{
		/* Enter to the menu  */
		sMenu_MakeSelectedAnim(currentMenu);
		if(!currentMenu->isOnDevelopment)
		{
			/* We can enter to the menu */
			if(sLCDTest == currentMenu->menuStage)
			{
				/* Only run the test */
				HMI_DisableAllButtons();
				LCD_Test();
				HMI_EnableButtons(iLeft | iRight | iEncoderSW | iOk | iMenu);
				UG_WindowShow(&menuWindow);
				UG_Update();
			}
			else
			{
				/* Enter to the menu */
				UIMainSM_ChangeMenu(currentMenu->menuStage);
				return true;
			}
		}
		return false; /* Skip Image processing */
	}
	else
	{
		/* Do Nothing */
	}
	sMenu_HideAllTextboxes();
	UG_ImageShow(&menuWindow, currentMenu->idImage);
	UG_TextboxShow(&menuWindow, currentMenu->idTextbox);
	if(currentMenu->isOnDevelopment)
	{
		/* Need to show the development texbox */
		UG_TextboxShow(&menuWindow, TB_VER_PROMPT_ID);
	}
	else
	{
		/* Do Nothing */
	}
	UG_Update();
	return false;
}

/**
 * @brief  This function makes the animation of an
 *         arrow press, it's only valid with right and
 *         left pressions
 * @param  btnPress : btn that have been pressed
 * @retval none
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

/**
 * @brief  Makes the blink animation on the image
 * @param  *menu : menu structure
 * @retval none
 */
static void sMenu_MakeSelectedAnim(MenuSelector *menu)
{
	const TickType_t AnimDelay = 45; /* x2 MS */
	const uint16_t AnimTimes = 3;

	/* Making the animation */
	for(uint16_t i = 0; i < AnimTimes; i++)
	{
		UG_FillFrame(ICON_X_POS, ICON_Y_POS, ICON_X_POS+120, ICON_Y_POS+120, C_WHITE);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&menuWindow, menu->idImage);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
	}
}

/**
 * @brief  Hide all the textboxes of this menu
 * @param  none
 * @retval none
 */
static void sMenu_HideAllTextboxes(void)
{
	MenuSelector *menu = (MenuSelector *)&MenuSelectorsGroup; /* Pointing to the top of the structure */
	for(uint16_t i = 0; i < sizeof(MenuSelectorsGroup)/sizeof(MenuSelector); i++, menu++)
	{
		/* Hiding all the textboxes */
		UG_TextboxHide(&menuWindow, menu->idTextbox);
	}
	if(menu->isOnDevelopment)
	{
		/* Need to hide the prompt textbox */
		UG_TextboxHide(&menuWindow, TB_VER_PROMPT_ID);
	}
	UG_Update();
}

/**
 * @brief  Create a custom textbox for this menu
 * @param  *txb : Textbox object
 * @param  id   : id of the textbox
 * @param  *str : String of the textbox
 * @param  show : if true, it shows the textbox on the first init
 * @param  xs   : Start position in x of the textbox
 * @param  ys   : Start position in y of the textbox
 * @retval none
 */
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
