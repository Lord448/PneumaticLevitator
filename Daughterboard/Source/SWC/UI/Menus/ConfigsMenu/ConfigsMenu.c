/**
 * @file      ConfigsMenu.c
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

#include "ConfigsMenu.h"

UG_WINDOW configsWindow;

typedef enum LocalMenuStages {
	sChangeConst,
	sChangeMode
}LocalMenuStages;

typedef struct MenuSelector {
	LocalMenuStages menuStage;
	uint8_t idImage;
	uint8_t idTextbox;
	bool isOnDevelopment;
}MenuSelector;

/* Menu Handlers */
struct MenuSelectorsGroup {
	MenuSelector mChangeConstants;
	MenuSelector mChangeMode;
}static MenuSelectorsGroup = {
	.mChangeConstants = {
		.menuStage = sChangeConst,
		.idImage = 0, /* TODO Complete here */
		.idTextbox = TB_CHANGE_CONS_ID,
		.isOnDevelopment = false
	},
	.mChangeMode = {
		.menuStage = sChangeMode,
		.idImage = 0, /* TODO Complete here */
		.idTextbox = TB_CHANGE_MODE_ID,
		.isOnDevelopment = false
	}
};

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static bool sConfigsMenu_ProcessButtonPress(Buttons btnPress);
static void sConfigsMenu_MakeArrowPressAnim(Buttons btnPress);
static void sConfigsMenu_MakeSelectedAnim(MenuSelector *menu);
static void sConfigsMenu_ChangeMenu(LocalMenuStages menuStage);
static void sConfigsMenu_HideAllTextboxes(void);
static void sConfigsMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void ConfigsMenu_Dynamics(Buttons btnPress, bool *isFirstInit)
{
	enum states{
		Init,
		Periodic
	}static stateHandler = Init;

	if(*isFirstInit)
	{
		/* First Entry to the menu */
		stateHandler = Init;
		*isFirstInit = false;
	}
	else
	{
		/* Do Nothing */
	}

	switch(stateHandler)
	{
		case Init:
			HMI_EnableButtons(iMenu | iLeft | iRight | iOk | iEncoderSW);
			stateHandler = Periodic;
		break;
		case Periodic:
			if(iNone != btnPress)
			{
				sConfigsMenu_MakeArrowPressAnim(btnPress);
				if(true == sConfigsMenu_ProcessButtonPress(btnPress))
				{
					/* Registered an option selection */
					stateHandler = Init;
				}
			}
		break;
	}
}

void ConfigsMenu_buildObjects(void)
{
	static UG_OBJECT  ObjWinBuf[32];
	/* Images */
	static UG_IMAGE   LeftArrowImage;
	static UG_IMAGE   PressedLeftArrowImage;
	static UG_IMAGE   RightArrowImage;
	static UG_IMAGE   PressedRightArrowImage;
	//static UG_IMAGE   ConstantsImage;
	//static UG_IMAGE   ModesImage;
	/* Textboxes */
	static UG_TEXTBOX tbChangeConstants;
	static UG_TEXTBOX tbChangeMode;

	UG_WindowCreate(&configsWindow, ObjWinBuf, 32, NULL);
	/* Set window characteristics */
	UG_WindowSetTitleText(&configsWindow, "Configuraciones");
  UG_WindowSetTitleTextFont(&configsWindow, FONT_6X8);
  UG_WindowSetTitleHeight(&configsWindow, 15);
  UG_WindowSetXStart(&configsWindow, 0);
  UG_WindowSetYStart(&configsWindow, 0);
  UG_WindowSetXEnd(&configsWindow, LCD_WIDTH-1);
  UG_WindowSetYEnd(&configsWindow, LCD_HEIGHT-1);
  UG_WindowSetBackColor(&configsWindow, C_WHITE);
  /* Building the images */
  UI_CreateImage(&configsWindow, &LeftArrowImage, LEFT_ARROW_IMG_ID, &LeftArrow, true, LEFT_ARROW_X_POS, LEFT_ARROW_Y_POS);
  UI_CreateImage(&configsWindow, &PressedLeftArrowImage, PRESSED_LEFT_ARROW_IMG_ID, &PressedLeftArrow, false, LEFT_ARROW_X_POS, LEFT_ARROW_Y_POS);
  UI_CreateImage(&configsWindow, &RightArrowImage, RIGHT_ARROW_IMG_ID, &RightArrow, true, RIGHT_ARROW_X_POS, RIGHT_ARROW_Y_POS);
  UI_CreateImage(&configsWindow, &PressedRightArrowImage, PRESSED_RIGHT_ARROW_IMG_ID, &PressedRightArrow, false, RIGHT_ARROW_X_POS, RIGHT_ARROW_Y_POS);
  /* TODO: Add Images here */
  /* Building the textboxes */
  sConfigsMenu_CreateTextbox(&tbChangeConstants, TB_CHANGE_CONS_ID, "Cambiar constantes PID", true, TB_CHANG_CONS_X_POS, MENU_TEXTBOX_Y_POS);
  sConfigsMenu_CreateTextbox(&tbChangeMode, TB_CHANGE_MODE_ID, "Cambiar modo de control", false, TB_CHANG_MODE_X_POS, MENU_TEXTBOX_Y_POS);
}
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
static bool sConfigsMenu_ProcessButtonPress(Buttons btnPress)
{
	MenuSelector *firstMenu = &MenuSelectorsGroup.mChangeConstants;
	MenuSelector *lastMenu = &MenuSelectorsGroup.mChangeMode;
	static MenuSelector *currentMenu = (MenuSelector *)&MenuSelectorsGroup;

	if(iLeft == btnPress)
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
	}
	else if(iRight == btnPress)
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
	}
	else if(iOk == btnPress || iEncoderSW == btnPress)
	{
		/* Enter to the menu  */
		sConfigsMenu_MakeSelectedAnim(currentMenu);
		if(!currentMenu->isOnDevelopment)
		{
			/* We can enter to the menu */
			sConfigsMenu_ChangeMenu(currentMenu->menuStage);
			return true;
		}
		return false; /* Skip Image processing */
	}
	else
	{
		/* Do Nothing */
	}
	sConfigsMenu_HideAllTextboxes();
	UG_ImageShow(&configsWindow, currentMenu->idImage);
	UG_TextboxShow(&configsWindow, currentMenu->idTextbox);
	if(currentMenu->isOnDevelopment)
	{
		/* Need to show the development texbox */
		UG_TextboxShow(&configsWindow, TB_VER_PROMPT_ID);
	}
	else
	{
		/* Do Nothing */
	}
	UG_Update();
	return false;
}

static void sConfigsMenu_ChangeMenu(LocalMenuStages menuStage)
{

}

static void sConfigsMenu_MakeArrowPressAnim(Buttons btnPress)
{
	const TickType_t AnimDelay = 100; /* MS */
	if(iLeft == btnPress)
	{
		/* Left button has been pressed */
		UG_ImageShow(&configsWindow, PRESSED_LEFT_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&configsWindow, LEFT_ARROW_IMG_ID);
		UG_Update();
	}
	else if(iRight == btnPress)
	{
		/* Right button has been pressed */
		UG_ImageShow(&configsWindow, PRESSED_RIGHT_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&configsWindow, RIGHT_ARROW_IMG_ID);
		UG_Update();
	}
	else
	{
		/* Do Nothing */
	}
}

static void sConfigsMenu_MakeSelectedAnim(MenuSelector *menu)
{
	const TickType_t AnimDelay = 45; /* x2 MS */
	const uint16_t AnimTimes = 3;

	/* Making the animation */
	for(uint16_t i = 0; i < AnimTimes; i++)
	{
		UG_FillFrame(ICON_X_POS, ICON_Y_POS, ICON_X_POS+120, ICON_Y_POS+120, C_WHITE);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&configsWindow, menu->idImage);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
	}
}

static void sConfigsMenu_HideAllTextboxes(void)
{
	MenuSelector *menu = (MenuSelector *)&MenuSelectorsGroup; /* Pointing to the top of the structure */
	for(uint16_t i = 0; i < sizeof(MenuSelectorsGroup)/sizeof(MenuSelector); i++, menu++)
	{
		/* Hiding all the textboxes */
		UG_TextboxHide(&configsWindow, menu->idTextbox);
	}
	if(menu->isOnDevelopment)
	{
		/* Need to hide the prompt textbox */
		UG_TextboxHide(&configsWindow, TB_VER_PROMPT_ID);
	}
	UG_Update();
}

static void sConfigsMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys)
{
	uint16_t xe = xs + ((MENU_TEXTBOX_FONT_X-10) * strlen(str));
	uint16_t ye = ys + MENU_TEXTBOX_FONT_Y;

	UI_TextboxCreate(&configsWindow, txb, id, xs, ys, xe, ye);
	UG_TextboxSetFont(&configsWindow, id, MENU_TEXTBOX_FONT);
	UG_TextboxSetText(&configsWindow, id, str);
	if(!show)
	{
		UG_TextboxHide(&configsWindow, id);
	}
}
