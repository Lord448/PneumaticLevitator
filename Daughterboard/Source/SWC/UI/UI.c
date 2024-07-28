/**
 * @file      UI.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     On this component it's implemented the UI task
 *            and all the animations and state machines
 *            that the UI have
 *
 * @date      Jul 3, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#include "UI.h"

extern osMemoryPoolId_t MemoryPool16; /*Memory Pool designed for members of 2 Byte size*/
extern osMemoryPoolId_t MemoryPool16_UI_PixelsValue; /*Component Specific Memory Pool*/
extern osMemoryPoolId_t MemoryPool16_UI_PixelsIndex; /*Component Specific Memory Pool*/

extern osEventFlagsId_t xEventFinishedInitHandle;

extern osSemaphoreId_t xSemaphoreDMACompleteHandle;
extern osSemaphoreId_t xSemaphoreCOMReadyHandle;

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream3;

extern const uint16_t ITMLogoData[IMAGE_SIZE(ITMLogo)];

static UG_WINDOW mainWindow;

/**
* @brief Function implementing the TaskUI thread.
* @param argument: Not used
* @retval None
*/
void vTaskUI(void *argument)
{
	UG_OBJECT ObjWinBuf[WINDOW_MAX_OBJECTS];
	UG_IMAGE AnimatedPCFANImage;
	/*Progress bar*/
	UG_PROGRESS pgbDistance;
	UG_PROGRESS pgbSetPoint;
	UG_PROGRESS pgbActionControl;
	/* TextBox */
	//UG_TEXTBOX  tbDate;
	UG_TEXTBOX  tbRPM;
	UG_TEXTBOX  tbRPMStr;
	UG_TEXTBOX  tbDistance;
	UG_TEXTBOX  tbDistanceStr;
	UG_TEXTBOX  tbSetPoint;
	UG_TEXTBOX  tbSetPointStr;
	UG_TEXTBOX  tbCtrlAction;
	//UG_TEXTBOX  tbCtrlActionStr;
	//UG_TEXTBOX  tbKP;
	//UG_TEXTBOX  tbKI;
	//UG_TEXTBOX  tbKD;
	//UG_TEXTBOX  tbKPStr;
	//UG_TEXTBOX  tbKIStr;
	//UG_TEXTBOX  tbKDStr;

	LCD_init();
#ifndef SKIP_INTRO_ANIM
	FadeWhiteIn(5);
	ITMLogoFadeIn();
	StringFadeIn();
#endif
	/* Creating and loading all the graphical resources */
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
  /* ITM Logo */
  //UI_ImageCreate(&mainWindow, &ITMLogoImageSmall, IMG_ID_0, 10, 0);
  //UG_ImageSetBMP(&mainWindow, IMG_ID_0, &ITMLogoSmall);
  /* Animated FAN */
  UI_ImageCreate(&mainWindow, &AnimatedPCFANImage, IMG_ID_0, LCD_WIDTH-80, 3);
  UG_ImageSetBMP(&mainWindow, IMG_ID_0, &AnimatedPCFan);

  /* Setting progress bar */
  /* Distance of the ball */
  UG_ProgressCreate(&mainWindow, &pgbDistance, PGB_ID_0,
  		PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_DISTANCE+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PGB_ID_0, C_ROYAL_BLUE);
  /* Set point */
  UG_ProgressCreate(&mainWindow, &pgbSetPoint, PGB_ID_1,
  		PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_SET_POINT+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PGB_ID_1, C_ROYAL_BLUE);
  /* Action Control */
  UG_ProgressCreate(&mainWindow, &pgbActionControl, PGB_ID_2,
  		PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL-PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_START_LOC,
			PROGRESS_BAR_X_CENTRAL_LOC_ACTION_CONTROL+PROGRESS_BAR_OFFSET, PROGRESS_BAR_Y_END_LOC);
  UG_ProgressSetForeColor(&mainWindow, PGB_ID_2, C_ROYAL_BLUE);

  /* Setting text boxes */
  /* Date */
  //UI_TextboxCreate(&mainWindow, &tbDate, TB_DATE_ID, xs, ys, xe, ye);
  /* RPM*/
  UI_TextboxCreate(&mainWindow, &tbRPM, TB_RPM_ID,
  		TB_RPM_X_INIT, TB_RPM_Y,
			TB_RPM_X_INIT+(TB_RPM_CHARS*TEXTBOX_FONT_X)-6, TB_RPM_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_RPM_ID, "RPM");
  /* RPM STR*/
  UI_TextboxCreate(&mainWindow, &tbRPMStr, TB_RPM_STR_ID,
  		TB_RPM_STR_X_INIT, TB_RPM_STR_Y,
			TB_RPM_STR_X_INIT+(TB_RPM_STR_CHARS*TEXTBOX_FONT_X)-10, TB_RPM_STR_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_RPM_STR_ID, "10000");
  /* Distance */
  UI_TextboxCreate(&mainWindow, &tbDistance, TB_DISTANCE_ID,
  		TB_DISTANCE_INIT, TB_DISTANCE_Y,
			(TB_DISTANCE_CHARS*TEXTBOX_FONT_X)-40, TB_DISTANCE_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_DISTANCE_ID, "Distancia");
  /* Distance STR */
  UI_TextboxCreate(&mainWindow, &tbDistanceStr, TB_DISTANCE_STR_ID,
  		TB_DISTANCE_STR_INIT, TB_DISTANCE_STR_Y,
			TB_DISTANCE_STR_INIT+(TB_DISTANCE_STR_CHARS*TEXTBOX_FONT_X)-10, TB_DISTANCE_STR_Y+TEXTBOX_FONT_Y);
	UG_TextboxSetText(&mainWindow, TB_DISTANCE_STR_ID, "500");
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
			TB_CTRL_ACTION_INIT+(TB_ACTION_MODE_CHARS*TEXTBOX_FONT_X)-30, TB_CTRL_ACTION_Y+TEXTBOX_FONT_Y);
  UG_TextboxSetText(&mainWindow, TB_CTRL_ACTION_ID, "Accion Ctrl");
  /* Action Mode */
  /* Kp */
  /* Ki */
  /* Kd */
  /* Error */


	/*Report to motherboard that all is ready to run*/
	//osSemaphoreAcquire(xSemaphoreCOMReadyHandle, osWaitForever); /*Wait until Motherboard is ready*/

#ifndef SKIP_INTRO_ANIM
  osDelay(pdMS_TO_TICKS(5000));
	StringFadeOut();
	ITMLogoFadeOut();
#endif

	UG_WindowShow(&mainWindow);
	UG_Update();
	osDelay(pdMS_TO_TICKS(5000));
  for(;;)
  {
  	/*TODO: Possible implementation of parsed loop scheme*/
  	//LCD_Test(); /*TODO Remove the test from here*/

  }
}

/**
 * @brief
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 * @retval
 */
UG_RESULT UI_TextboxCreate(UG_WINDOW* wnd, UG_TEXTBOX* txb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
	UG_RESULT result;
	result = UG_TextboxCreate(wnd, txb, id, xs, ys, xe, ye);
	if(result != 0)
		return result;
	result = UG_TextboxSetFont(wnd, id, TEXTBOX_FONT);
	if(result != 0)
		return result;
	result = UG_TextboxSetBackColor(wnd, id, C_WHITE_94);
	if(result != 0)
		return result;
	result = UG_TextboxSetForeColor(wnd, id, C_BLACK);
	if(result != 0)
		return result;
	result = UG_TextboxSetAlignment(wnd, id, ALIGN_CENTER);
	if(result != 0)
		return result;
	return result;
}

/**
 * @brief
 * @param
 * @retval
 */
result_t UI_setRPMText(uint32_t rpm)
{
	result_t result = OK;
	return result;
}
