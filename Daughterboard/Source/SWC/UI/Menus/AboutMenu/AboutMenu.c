/**
 * @file      AboutMenu.c
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

#include "AboutMenu.h"

typedef enum pages {
	None = -1, /* Added in order to make the enum signed */
	Page1,
	Page2,
	Page3,
	Page4,
	Page5,
	Page6,
	Page7
}Pages;

typedef struct AboutMenu_Textbox {
	UG_TEXTBOX textbox;
	Pages page;
	uint8_t id;
}AboutMenu_Textbox;

static struct AboutMenuTextboxes {
	/* Page 1 */
	AboutMenu_Textbox tbHardRev;
	AboutMenu_Textbox tbHardMotherRevStr;
	AboutMenu_Textbox tbHardDaughterRevStr;
	AboutMenu_Textbox tbSoftVer;
	AboutMenu_Textbox tbSoftVerStr;
	/* Page 2 */
	AboutMenu_Textbox tbHardEngineers;
	AboutMenu_Textbox tbHardEngineerPedro;
	AboutMenu_Textbox tbHardEngineerPedroMail;
	AboutMenu_Textbox tbHardEngineerDavid;
	AboutMenu_Textbox tbHardEngineerDavidMail;
	/* Page 3 */
	AboutMenu_Textbox tbSoftArchitect;
	AboutMenu_Textbox tbSoftPedro;
	AboutMenu_Textbox tbSoftPedroMail;
	/* Page 4 */
	AboutMenu_Textbox tbSoftEngineers;
	AboutMenu_Textbox tbSoftEngineerPedro;
	AboutMenu_Textbox tbSoftEngineerPedroMail;
	AboutMenu_Textbox tbSoftEngineerAndres;
	AboutMenu_Textbox tbSoftEngineerAndresMail;
	/* Page 5 */
	AboutMenu_Textbox tbSoftEngineers5;
	AboutMenu_Textbox tbSoftEngineerDavid;
	AboutMenu_Textbox tbSoftEngineerDavidMail;
	AboutMenu_Textbox tbSoftEngineerHector;
	AboutMenu_Textbox tbSoftEngineerHectorMail;
	/* Page 6 */
	AboutMenu_Textbox tbGitHubUsers;
	AboutMenu_Textbox tbGitPedro;
	AboutMenu_Textbox tbGitPedroUser;
	AboutMenu_Textbox tbGitDavid;
	AboutMenu_Textbox tbGitDavidUser;
	/* Page 7 */
	AboutMenu_Textbox tbGitHubUsers7;
	AboutMenu_Textbox tbGitAndres;
	AboutMenu_Textbox tbGitAndresUser;
	AboutMenu_Textbox tbGitHector;
	AboutMenu_Textbox tbGitHectorUser;

}AboutMenuTextboxes = {
	/* Page 1 */
	.tbHardRev = {
		.page = Page1,
		.id = TB_HARD_REV_ID
	},
	.tbHardMotherRevStr = {
		.page = Page1,
		.id = TB_HARD_REV_MOTHER_STR_ID
	},
	.tbHardDaughterRevStr = {
		.page = Page1,
		.id = TB_HARD_REV_DAUGHT_STR_ID
	},
	.tbSoftVer = {
		.page = Page1,
		.id = TB_SOFT_VER_ID
	},
	.tbSoftVerStr = {
		.page = Page1,
		.id = TB_SOFT_VER_STR_ID
	},
	/* Page 2 */
	.tbHardEngineers = {
		.page = Page2,
		.id = TB_HARD_ENG_ID
	},
	.tbHardEngineerPedro = {
		.page = Page2,
		.id = TB_HARD_ENG_PEDRO_ID
	},
	.tbHardEngineerPedroMail = {
		.page = Page2,
		.id = TB_HARD_ENG_PEDRO_MAIL_ID
	},
	.tbHardEngineerDavid = {
		.page = Page2,
		.id = TB_HARD_ENG_DAVID_ID
	},
	.tbHardEngineerDavidMail = {
		.page = Page2,
		.id = TB_HARD_ENG_DAVID_MAIL_ID
	},
	/* Page 3 */
	.tbSoftArchitect = {
		.page = Page3,
		//.id = TB_SOFT_ARCH_ID
	},
	.tbSoftPedro = {
		.page = Page3,
		//.id = TB_SOFT_ARCH_PEDRO_ID
	},
	.tbSoftPedroMail = {
		.page = Page3,
		//.id = TB_SOFT_ARCH_PEDRO_MAIL_ID
	},
	/* Page 4 */
	.tbSoftEngineers = {
    .page = Page4,
    .id = TB_SOFT_ENG_ID
	},
	.tbSoftEngineerPedro = {
    .page = Page4,
    .id = TB_SOFT_ENG_PEDRO_ID
	},
	.tbSoftEngineerPedroMail = {
    .page = Page4,
    .id = TB_SOFT_ENG_PEDRO_MAIL_ID
	},
	.tbSoftEngineerAndres = {
    .page = Page4,
    .id = TB_SOFT_ENG_ANDRES_ID
	},
	.tbSoftEngineerAndresMail = {
    .page = Page4,
    .id = TB_SOFT_ENG_ANDRES_MAIL_ID
	},
	/* Page 5 */
	.tbSoftEngineers5 = {
    .page = Page5,
    .id = TB_SOFT_ENG_ID
	},
	.tbSoftEngineerDavid = {
    .page = Page5,
    .id = TB_SOFT_ENG_DAVID_ID
	},
	.tbSoftEngineerDavidMail = {
    .page = Page5,
    .id = TB_SOFT_ENG_DAVID_MAIL_ID
	},
	.tbSoftEngineerHector = {
    .page = Page5,
    .id = TB_SOFT_ENG_HECTOR_ID
	},
	.tbSoftEngineerHectorMail = {
    .page = Page5,
    .id = TB_SOFT_ENG_HECTOR_MAIL_ID
	},
	/* Page 6 */
	.tbGitHubUsers = {
    .page = Page6,
    .id = TB_GITHUB_USERS
	},
	.tbGitPedro = {
    .page = Page6,
    .id = TB_PEDRO_GIT
	},
	.tbGitPedroUser = {
    .page = Page6,
    .id = TB_PEDRO_GIT_USER
	},
	.tbGitDavid = {
    .page = Page6,
    .id = TB_DAVID_GIT
	},
	.tbGitDavidUser = {
    .page = Page6,
    .id = TB_DAVID_GIT_USER
	},
	/* Page 7 */
	.tbGitHubUsers7 = {
    .page = Page7,
    .id = TB_GITHUB_USERS
	},
	.tbGitAndres = {
    .page = Page7,
    .id = TB_ANDRES_GIT
	},
	.tbGitAndresUser = {
    .page = Page7,
    .id = TB_ANDRES_GIT_USER
	},
	.tbGitHector = {
    .page = Page7,
    .id = TB_DAVID_GIT
	},
	.tbGitHectorUser = {
    .page = Page7,
    .id = TB_DAVID_GIT_USER
	},
};

UG_WINDOW aboutWindow;

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */
static void sAboutMenu_ProcessButtonPress(Buttons btnPress);
static void sAboutMenu_MakeArrowPressAnim(Buttons btnPress);
static void sAboutMenu_ShowPage(Pages page);
static void sAboutMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
static void sAboutMenu_CreateSmallTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys);
/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  This function makes all the animations and
 *         processes the button pressions gived by the
 *         main state machine, the reception of the button
 *         pressed is received by a Queue on the Main state
 *         machine
 * @param  The button that has been pressed
 * @retval none
 */
void AboutMenu_Dynamics(Buttons btnPress, bool *isFirstMenuInit)
{
	enum states {
		Init,
		Periodic
	}static stateHandler = Init;

	if(*isFirstMenuInit)
	{
		/* First entry to the menu */
		stateHandler = Init;
		*isFirstMenuInit = false;
	}
	else
	{
		/* Do Nothing */
	}
	switch(stateHandler)
	{
		case Init:
			HMI_EnableButtons(iUp | iDown | iMenu);
			stateHandler = Periodic;
		break;
		case Periodic:
			if(iUp == btnPress || iDown == btnPress)
			{
				/* Process the pression */
				sAboutMenu_ProcessButtonPress(btnPress);
			}
			else
			{
				/* Do Nothing */
			}
		break;
	}
}

/**
 * @brief  Build and declare all the graphical
 *         resources needed by this component
 * @param  none
 * @retval none
 */
void AboutMenu_buildObjects(void)
{
	static UG_OBJECT ObjWinBuf[32];
	/* Images */
	static UG_IMAGE  UpArrowImage;
	static UG_IMAGE  PressedUpArrowImage;
	static UG_IMAGE  DownArrowImage;
	static UG_IMAGE  PressedDownArrowImage;

	UG_WindowCreate(&aboutWindow, ObjWinBuf, WINDOW_MAX_OBJECTS, NULL);
	/* Set window characteristics */
	UG_WindowSetTitleText(&aboutWindow, "Acerca de");
  UG_WindowSetTitleTextFont(&aboutWindow, FONT_6X8);
  UG_WindowSetTitleHeight(&aboutWindow, 15);
  UG_WindowSetXStart(&aboutWindow, 0);
  UG_WindowSetYStart(&aboutWindow, 0);
  UG_WindowSetXEnd(&aboutWindow, LCD_WIDTH-1);
  UG_WindowSetYEnd(&aboutWindow, LCD_HEIGHT-1);
  UG_WindowSetBackColor(&aboutWindow, C_WHITE);
  /* Building Images */
  UI_CreateImage(&aboutWindow, &UpArrowImage, UP_ARROW_IMG_ID, &UpArrow, false, UP_ARROW_POS_X, UP_ARROW_POS_Y);
  UI_CreateImage(&aboutWindow, &PressedUpArrowImage, PRESSED_UP_ARROW_IMG_ID, &PressedUpArrow, false, UP_ARROW_POS_X, UP_ARROW_POS_Y);
  UI_CreateImage(&aboutWindow, &DownArrowImage, DOWN_ARROW_IMG_ID, &DownArrow, true, DOWN_ARROW_POS_X, DOWN_ARROW_POS_Y);
  UI_CreateImage(&aboutWindow, &PressedDownArrowImage, PRESSED_DOWN_ARROW_IMG_ID, &PressedDownArrow, false, DOWN_ARROW_POS_X, DOWN_ARROW_POS_Y);
  /* Building Textboxes */
  /* Page 1 */
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardRev.textbox, AboutMenuTextboxes.tbHardRev.id, "Hardware Revision", true, 35, TB_HARD_REV_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardMotherRevStr.textbox, AboutMenuTextboxes.tbHardMotherRevStr.id, "Motherboard Rev: 2.3", true, 20, TB_HARD_REV_MOTHER_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardDaughterRevStr.textbox, AboutMenuTextboxes.tbHardDaughterRevStr.id, "Daughterboard Rev: 1.2", true, 5, TB_HARD_REV_DAUGHT_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftVer.textbox, AboutMenuTextboxes.tbSoftVer.id, "Software Version", true, 35, TB_SOFT_VER_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftVerStr.textbox, AboutMenuTextboxes.tbSoftVerStr.id, SOFTWARE_VERSION, true, 43, TB_SOFT_VER_STR_POS_Y);
  /* Page 2 */
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardEngineers.textbox, AboutMenuTextboxes.tbHardEngineers.id, "Hardware Engineers", false, 20, TB_HARD_ENG_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardEngineerPedro.textbox, AboutMenuTextboxes.tbHardEngineerPedro.id, PEDRO_NAME, false, 5, TB_HARD_ENG_PEDRO_POS_Y);
  sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbHardEngineerPedroMail.textbox, AboutMenuTextboxes.tbHardEngineerPedroMail.id, PEDRO_MAIL, false, 0, TB_HARDENG_PEDROMAIL_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbHardEngineerDavid.textbox, AboutMenuTextboxes.tbHardEngineerDavid.id, DAVID_NAME, false, 5, TB_HARD_ENG_DAVID_POS_Y);
  sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbHardEngineerDavidMail.textbox, AboutMenuTextboxes.tbHardEngineerDavidMail.id, DAVID_MAIL, false, 0, TB_HARDENG_DAVIDMAIL_POS_Y);
  /* Page 3 */
  /* Page 4 */
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftEngineers.textbox, AboutMenuTextboxes.tbSoftEngineers.id, "Software Engineers", false, 20, TB_SOFT_ENG_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftEngineerPedro.textbox, AboutMenuTextboxes.tbSoftEngineerPedro.id, PEDRO_NAME, false, 5, TB_SOFT_ENG_PEDRO_POS_Y);
  sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbSoftEngineerPedroMail.textbox, AboutMenuTextboxes.tbSoftEngineerPedroMail.id, PEDRO_MAIL, false, 0, TB_SOFTENG_PEDROMAIL_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftEngineerAndres.textbox, AboutMenuTextboxes.tbSoftEngineerAndres.id, ANDRES_NAME, false, 5, TB_SOFT_ENG_ANDRES_POS_Y);
  sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbSoftEngineerAndresMail.textbox, AboutMenuTextboxes.tbSoftEngineerAndresMail.id, ANDRES_MAIL, false, 0, TB_SOFENG_ANDRESMAIL_POS_Y);
  /* Page 5 */
	sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftEngineerDavid.textbox, AboutMenuTextboxes.tbSoftEngineerDavid.id, DAVID_NAME, false, 5, TB_SOFT_ENG_DAVID_POS_Y);
	sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbSoftEngineerDavidMail.textbox, AboutMenuTextboxes.tbSoftEngineerDavidMail.id, DAVID_MAIL, false, 0, TB_SOFTENF_DAVIDMAIL_POS_Y);
	sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbSoftEngineerHector.textbox, AboutMenuTextboxes.tbSoftEngineerHector.id, HECTOR_NAME, false, 5, TB_SOFT_ENG_HECTOR_POS_Y);
	sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbSoftEngineerHectorMail.textbox, AboutMenuTextboxes.tbSoftEngineerHectorMail.id, HECTOR_MAIL, false, 0, TB_SOFENG_HECTORMAIL_POS_Y);
	/* Page 6 */
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbGitHubUsers.textbox, AboutMenuTextboxes.tbGitHubUsers.id, "GitHub Users", false, 20, TB_GITHUB_USERS_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbGitPedro.textbox, AboutMenuTextboxes.tbGitPedro.id, PEDRO_NAME, false, 5, TB_GIT_PEDRO);
  sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbGitPedroUser.textbox, AboutMenuTextboxes.tbGitPedroUser.id, PEDRO_GIT, false, 5, TB_GIT_PEDRO_USER_POS_Y);
  sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbGitDavid.textbox, AboutMenuTextboxes.tbGitDavid.id, DAVID_NAME, false, 5, TB_GIT_DAVID_POS_Y);
	UI_TextboxCreate(&aboutWindow, &AboutMenuTextboxes.tbGitDavidUser.textbox, AboutMenuTextboxes.tbGitDavidUser.id, 5, TB_GIT_DAVID_USER_POS_Y, 100, TB_GIT_DAVID_USER_POS_Y+MENU_SMALL_TEXTBOX_FONT_Y);
	UG_TextboxSetFont(&aboutWindow, AboutMenuTextboxes.tbGitDavidUser.id, MENU_SMALL_TEXTBOX_FONT);
	UG_TextboxSetText(&aboutWindow, AboutMenuTextboxes.tbGitDavidUser.id, DAVID_GIT);
	UG_TextboxHide(&aboutWindow, AboutMenuTextboxes.tbGitDavidUser.id);
  UG_TextboxSetAlignment(&aboutWindow, TB_DAVID_GIT, ALIGN_H_LEFT);
  UG_TextboxSetAlignment(&aboutWindow, TB_DAVID_GIT_USER, ALIGN_H_LEFT);
  /* Page 7 */
	sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbGitAndres.textbox, AboutMenuTextboxes.tbGitAndres.id, ANDRES_NAME, false, 5, TB_GIT_ANDRES_POS_Y);
	sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbGitAndresUser.textbox, AboutMenuTextboxes.tbGitAndresUser.id, ANDRES_GIT, false, 5, TB_GIT_ANDRES_USER_POS_Y);
	sAboutMenu_CreateTextbox(&AboutMenuTextboxes.tbGitHector.textbox, AboutMenuTextboxes.tbGitHector.id, HECTOR_NAME, false, 5, TB_GIT_HECTOR_POS_Y);
	sAboutMenu_CreateSmallTextbox(&AboutMenuTextboxes.tbGitHectorUser.textbox, AboutMenuTextboxes.tbGitHectorUser.id, HECTOR_GIT, false, 5, TB_GIT_HECTOR_USER_POS_Y);
}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */
/**
 * @brief  This function processes the buttons and
 *         change the page depending of the case
 * @param  btnPress : Button that has been pressed
 * @retval none
 */
static void sAboutMenu_ProcessButtonPress(Buttons btnPress)
{
	static Pages currentPage = Page1;
	static Pages pastPage = Page1;

	if(iUp == btnPress)
	{
		/* Up Button has been pressed */
		currentPage--;
		if(currentPage < Page1)
			currentPage = Page1;
		if(currentPage == Page3)
			currentPage--;
	}
	else if(iDown == btnPress)
	{
		/* Down Button has been pressed */
		currentPage++;
		if(currentPage > Page7)
			currentPage = Page7;
		if(currentPage == Page3)
			currentPage++;
	}
	else
	{
		/* Do Nothing */
	}

	switch(currentPage)
	{
		case Page1:
			/* Coming from Page 2 */
			if(currentPage != pastPage)
			{
				/* Entering on the page */
				sAboutMenu_MakeArrowPressAnim(btnPress);
				UG_ImageHide(&aboutWindow, UP_ARROW_IMG_ID);
				sAboutMenu_ShowPage(currentPage);
			}
			else
			{
				/* Do Nothing */
			}
		break;
		case Page2:
			/* Can be coming from Page 1 or Page 2 */
			if(Page1 == pastPage)
			{
				/*Coming from page 1*/
				sAboutMenu_MakeArrowPressAnim(btnPress);
				sAboutMenu_ShowPage(currentPage);
				UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
				UG_Update();
			}
			else
			{
				/* Coming from Page 3 */
				sAboutMenu_MakeArrowPressAnim(btnPress);
				sAboutMenu_ShowPage(currentPage);
				UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
				UG_Update();
			}
		break;
		case Page3:
		case Page4:
		case Page5:
			sAboutMenu_MakeArrowPressAnim(btnPress);
			sAboutMenu_ShowPage(currentPage);
			UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
			UG_Update();
		break;
		case Page6:
			if(Page7 == pastPage)
			{
				/*Coming from page 7*/
				sAboutMenu_MakeArrowPressAnim(iUp);
				sAboutMenu_ShowPage(currentPage);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT, DAVID_NAME);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT_USER, DAVID_GIT);
				UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
				UG_ImageShow(&aboutWindow, DOWN_ARROW_IMG_ID);
				UG_Update();
			}
			else
			{
				sAboutMenu_MakeArrowPressAnim(btnPress);
				sAboutMenu_ShowPage(currentPage);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT, DAVID_NAME);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT_USER, DAVID_GIT);
				UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
				UG_Update();
			}
		break;
		case Page7:
			/* Coming from Page 6 */
			if(currentPage != pastPage)
			{
				/* Entering on the page */
				sAboutMenu_MakeArrowPressAnim(btnPress);
				sAboutMenu_ShowPage(currentPage);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT, HECTOR_NAME);
				UG_TextboxSetText(&aboutWindow, TB_DAVID_GIT_USER, HECTOR_GIT);
				UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
				UG_ImageHide(&aboutWindow, DOWN_ARROW_IMG_ID);
				UG_Update();
			}
			else
			{
				/* Do Nothing */
			}
		break;
		case None:
		default:
		break;
	}
	pastPage = currentPage;
}
/**
 * @brief  Makes an animation of press changing the
 *         image of the arrow
 * @param  btnPress : Button that has been pressed
 * @retval none
 */
static void sAboutMenu_MakeArrowPressAnim(Buttons btnPress)
{
	const TickType_t AnimDelay = 100; /*MS*/
	if(iUp == btnPress)
	{
		/* Up Button has been pressed */
		UG_ImageShow(&aboutWindow, PRESSED_UP_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&aboutWindow, UP_ARROW_IMG_ID);
		UG_Update();
	}
	else if(iDown == btnPress)
	{
		/* Down button has been pressed */
		UG_ImageShow(&aboutWindow, PRESSED_DOWN_ARROW_IMG_ID);
		UG_Update();
		osDelay(pdMS_TO_TICKS(AnimDelay));
		UG_ImageShow(&aboutWindow, DOWN_ARROW_IMG_ID);
		UG_Update();
	}
	else
	{
		/* Do Nothing */
	}
}
/**
 * @brief  Hides and shows the marked text boxes
 * @note   Due to uGUI renderization algorithm
 *         the function needs to hide first the
 *         textboxes and then show the other ones
 * @param  page : Page thats needs to be shown
 * @retval none
 */
static void sAboutMenu_ShowPage(Pages page)
{
	AboutMenu_Textbox *tbPointer = (AboutMenu_Textbox *)&AboutMenuTextboxes; /* Pointing to the top of the structure */

	/* Process the structure */

	/* Hide all other text-boxes */
	for(uint16_t i = 0; i < sizeof(AboutMenuTextboxes)/sizeof(AboutMenu_Textbox); i++, tbPointer++)
	{
		if(tbPointer->page != page)
		{
			/* Hide all other textboxes */
			UG_TextboxHide(&aboutWindow, tbPointer->id);
		}
		else
		{
			/* Do Nothing */
		}
	}
	UG_Update();
	tbPointer = (AboutMenu_Textbox *)&AboutMenuTextboxes; /* Pointing to the top of the structure */
	/* Show the text-boxes */
	for(uint16_t i = 0; i < sizeof(AboutMenuTextboxes)/sizeof(AboutMenu_Textbox); i++, tbPointer++)
	{
		if(tbPointer->page == page)
		{
			/* Show the textbox */
			UG_TextboxShow(&aboutWindow, tbPointer->id);
		}
		else
		{
			/* Do Nothing */
		}
	}
	UG_Update();
}

/**
 * @brief  Create a custom textbox for this menu
 * @param  *txb : Textbox object
 * @param  id   : id of the texbox
 * @param  *str : String of the textbox
 * @param  show : if true, it shows the textbox on the first init
 * @param  xs   : Start position in x of the textbox
 * @param  ys   : Start position in y of the textbox
 * @retval none
 */
static void sAboutMenu_CreateTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys)
{
	uint16_t xe = xs + ((ABOUT_MENU_TEXTBOX_FONT_X-10) * strlen(str));
	uint16_t ye = ys + ABOUT_MENU_TEXTBOX_FONT_Y;

	UI_TextboxCreate(&aboutWindow, txb, id, xs, ys, xe, ye);
	UG_TextboxSetFont(&aboutWindow, id, MENU_TEXTBOX_FONT);
	UG_TextboxSetText(&aboutWindow, id, str);
	if(!show)
	{
		UG_TextboxHide(&aboutWindow, id);
	}
}

/**
 * @brief  Create a custom small textbox with
 *         the variation of the font
 * @param  *txb : Textbox object
 * @param  id   : id of the texbox
 * @param  *str : String of the textbox
 * @param  show : if true, it shows the textbox on the first init
 * @param  xs   : Start position in x of the textbox
 * @param  ys   : Start position in y of the textbox
 * @retval none
 */
static void sAboutMenu_CreateSmallTextbox(UG_TEXTBOX* txb, UG_U8 id, char *str, bool show, UG_S16 xs, UG_S16 ys)
{
	uint16_t xe = xs + ((MENU_SMALL_TEXTBOX_FONT_X-7) * strlen(str));
	uint16_t ye = ys + MENU_SMALL_TEXTBOX_FONT_Y;

	UI_TextboxCreate(&aboutWindow, txb, id, xs, ys, xe, ye);
	UG_TextboxSetFont(&aboutWindow, id, MENU_SMALL_TEXTBOX_FONT);
	UG_TextboxSetText(&aboutWindow, id, str);
	if(!show)
	{
		UG_TextboxHide(&aboutWindow, id);
	}
}
