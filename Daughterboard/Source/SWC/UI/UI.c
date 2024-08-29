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

extern osThreadId_t TaskUIHandle;

extern osMessageQueueId_t xFIFO_ControlConstantsHandle;
extern osMessageQueueId_t xFIFO_DistanceHandle;
extern osMessageQueueId_t xFIFO_RPMHandle;

extern osEventFlagsId_t xEventFinishedInitHandle;
extern osEventFlagsId_t xEvent_UIConfigsHandle;

extern osSemaphoreId_t xSemaphoreDMACompleteHandle;
extern osSemaphoreId_t xSemaphoreCOMReadyHandle;

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream3;

extern const uint16_t ITMLogoData[IMAGE_SIZE(ITMLogo)];

extern UG_WINDOW mainWindow;

static void waitForConstants(void);
static void loadGraphicalResources(void);

/**
* @brief Function implementing the TaskUI thread.
* @param argument: Not used
* @retval None
*/
void vTaskUI(void *argument)
{
	LCD_init();
#ifndef SKIP_INTRO_ANIM
	FadeWhiteIn(5);
	ITMLogoFadeIn();
	StringFadeIn();
#endif
	/* Creating and loading all the graphical resources */
	loadGraphicalResources();
	waitForConstants();
#ifndef SKIP_INTRO_ANIM
	StringFadeOut();
	ITMLogoFadeOut();
#endif
	UG_WindowShow(&mainWindow);
	UG_Update();
	MainMenu_setSetPoint(DEFAULT_SET_POINT);

  for(;;)
  {
  	UIMainSM_InfiniteLoop();
  	//LCD_Test(); /*TODO Remove the test from here*/
  }
}

/**
 * @brief
 * @param  none
 * @retval none
 */
static void loadGraphicalResources(void)
{
	AboutMenu_buildObjects();
	ConfigsMenu_buildObjects();
	MainMenu_buildObjects();
	Menu_buildObjects();
	UIMainSM_Init();
}

/**
 * @brief
 * @param  none
 * @retval none
 */
static void waitForConstants(void)
{
	ControlConst controlConst;
	if(osOK != osMessageQueueGet(xFIFO_ControlConstantsHandle, &controlConst, NULL, osNoTimeout))
	{
		/* Need to wait */
		if(osOK == osMessageQueueGet(xFIFO_ControlConstantsHandle, &controlConst, NULL,
#ifndef SKIP_INTRO_ANIM
				pdMS_TO_TICKS(5000)))
#else
				osNoTimeout))
#endif

		{
			/* Correctly received the data from COM */
			MainMenu_setControlConstants(controlConst.kp, controlConst.ki, controlConst.kd);
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		/* Correctly received the data from COM */
		MainMenu_setControlConstants(controlConst.kp, controlConst.ki, controlConst.kd);
#ifndef SKIP_INTRO_ANIM
		osDelay(pdMS_TO_TICKS(1000));
#endif
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
	result = UG_TextboxSetBackColor(wnd, id, C_WHITE_94); /* TODO: debug to see the hole cage */
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
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 * @retval
 */
UG_RESULT UI_CheckboxCreate(UG_WINDOW* wnd, UG_CHECKBOX* chb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
	result_t result;
	result = UG_CheckboxCreate(wnd, chb, id, xs, ys, xe, ye);
	if(result != 0)
		return result;
	result = UG_CheckboxSetAlignment(wnd, id, ALIGN_CENTER);
	if(result != 0)
			return result;
	result = UG_CheckboxSetForeColor(wnd, id, C_GRAY);
	if(result != 0)
			return result;
	result = UG_CheckboxSetForeColor(wnd, id, C_BLACK);
	if(result != 0)
			return result;
	result = UG_CheckboxSetAlternateBackColor(wnd, id, C_BLACK);
	if(result != 0)
			return result;
	result = UG_CheckboxSetAlternateForeColor(wnd, id, C_GRAY);
	if(result != 0)
			return result;
	result = UG_CheckboxSetFont(wnd, id, FONT_6X8);
	if(result != 0)
			return result;
	result = UG_CheckboxSetBackColor(wnd, id, C_WHITE);
	return result;
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
UG_RESULT UI_CreateImage(UG_WINDOW* wnd, UG_IMAGE* img, UG_U8 id, const UG_BMP* bmp, bool show, UG_S16 xs, UG_S16 ys)
{
	UG_RESULT result;
	result = UI_ImageCreate(wnd, img, id, xs, ys);
	if(result != 0)
		return result;
	result = UG_ImageSetBMP(wnd, id, bmp);
	if(result != 0)
		return result;
	if(!show)
	{
		/* Need to hide the image */
		result = UG_ImageHide(wnd, id);
		if(result != 0)
			return result;
	}
	return result;

}
