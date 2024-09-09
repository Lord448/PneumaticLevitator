/**
 * @file      UI.h
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

#ifndef UI_H_
#define UI_H_

#include "main.h"
#include "lcd.h"
#include "ugui.h"
#include "cmsis_os.h"
#include "Bitmaps.h"
#include "OSHandler.h"
#include "GPUResMan.h"
#include "UIMainSM.h"
#include "Anim.h"
#include "AboutMenu.h"
#include "ConfigsMenu.h"
#include "MainMenu.h"
#include "ControlModeMenu.h"
#include "Menu.h"
#include "COM.h"
#include <stdio.h>
#include <string.h>

/**
 * ---------------------------------------------------------
 * 					         UI GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define charslen(_) (int32_t)(sizeof(_)-1)
#define WINDOW_MAX_OBJECTS 32
#define UI_STARTED_THREAD_FLAG (uint32_t) 1U << 0

//#define SKIP_INTRO_ANIM   /* Skips the initial image fade on the init of the project */
//#define SEE_TEXTBOX_CAGE  /* Prints all the textbox cages with a gray color */
/**
 * ---------------------------------------------------------
 * 					        FUNCTION LIKE MACRO
 * ---------------------------------------------------------
 */
#define UI_ImageCreate(window, ImgObj, IMG_ID, xs, ys) UG_ImageCreate(window, ImgObj, IMG_ID, xs, ys, 0, 0) /*Helps on the creation of a BMP*/
#define RGB565Color(R, G, B) (uint16_t)(r<<11 | g<<5 | b) /*Macro to convert RGB color to RGB565 color code*/
/**
 * ---------------------------------------------------------
 * 					        UI GLOBAL FUNCTIONS
 * ---------------------------------------------------------
 */
void vTaskUI(void *argument);
UG_RESULT UI_TextboxCreate(UG_WINDOW* wnd, UG_TEXTBOX* txb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye);
UG_RESULT UI_CheckboxCreate(UG_WINDOW* wnd, UG_CHECKBOX* chb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye);
UG_RESULT UI_CreateImage(UG_WINDOW* wnd, UG_IMAGE* img, UG_U8 id, const UG_BMP* bmp, bool show, UG_S16 xs, UG_S16 ys);

#endif /* UI_H_ */
