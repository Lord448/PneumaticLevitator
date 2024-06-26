/*
 * UI.h
 *
 *  Created on: Dec 27, 2023
 *      Author: lord448
 */

#ifndef UI_INC_UI_H_
#define UI_INC_UI_H_

#include "lcd.h"
#include "ugui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define FIRMWARE_VERSION "0.0.1b"

typedef enum MenuStages {
	sInitialAnim,
	sMainLobby,
	sMenu,
	sConfiguration,
	sAbout,
	sPlot,
	sUSBConfig,
	sStepResponse
}MenuStage;

typedef enum Input {
	iOk,
	iUp,
	iDown,
	iLeft,
	iRight,
	iMenu,
	iEncoder,
	iReset
}Input;

typedef struct UIWindow {
	UG_WINDOW window;
	UG_OBJECT *obj_buff;
	uint8_t obj_len;
}UIWindow;

uint16_t RGB565Color(uint8_t r, uint8_t g, uint8_t b);

void UIDrawCenterBitmap(UG_BMP* bmp);

void vTaskUI(MenuStage *stage, Input *input);

void UIWindowCreate(UIWindow *window, ...);

#endif /* UI_INC_UI_H_ */
