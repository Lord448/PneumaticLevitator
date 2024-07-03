/*
 * UI.h
 *
 *  Created on: Jul 3, 2024
 *      Author: lord448
 */

#ifndef UI_H_
#define UI_H_

#include "main.h"
#include "lcd.h"
#include "ugui.h"
#include "Bitmaps.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal_dma.h"
#include <stdio.h>
#include <string.h>

#define RGB565Color(R, G, B) (uint16_t)(r<<11 | g<<5 | b)

void vTaskUI(void *argument);
void DMATrasferCpltCallback(DMA_HandleTypeDef *DmaHandle);

#endif /* UI_H_ */
