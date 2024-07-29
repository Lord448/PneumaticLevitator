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
#include "MainMenu.h"
#include "stm32f4xx_hal_dma.h"
#include <stdio.h>
#include <string.h>

#define WINDOW_MAX_OBJECTS 30

#define SKIP_INTRO_ANIM

/**
 * ---------------------------------------------------------
 * 					        FUNCTION LIKE MACRO
 * ---------------------------------------------------------
 */
/*Helps on the creation of a BMP*/
#define UI_ImageCreate(window, ImgObj, IMG_ID, xs, ys) UG_ImageCreate(window, ImgObj, IMG_ID, xs, ys, 0, 0)
/*Macro to convert RGB color to RGB565 color code*/
#define RGB565Color(R, G, B) (uint16_t)(r<<11 | g<<5 | b)

void vTaskUI(void *argument);
UG_RESULT UI_TextboxCreate(UG_WINDOW* wnd, UG_TEXTBOX* txb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye);
UG_RESULT UI_CheckboxCreate(UG_WINDOW* wnd, UG_CHECKBOX* chb, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye);

#endif /* UI_H_ */
