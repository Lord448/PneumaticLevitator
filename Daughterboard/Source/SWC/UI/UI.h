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
#include "UIMainSM.h"
#include "stm32f4xx_hal_dma.h"
#include <stdio.h>
#include <string.h>

/*Macro to convert RGB color to RGB565 color code*/
#define RGB565Color(R, G, B) (uint16_t)(r<<11 | g<<5 | b)

void vTaskUI(void *argument);
void DMATrasferCpltCallback(DMA_HandleTypeDef *DmaHandle);

#endif /* UI_H_ */
