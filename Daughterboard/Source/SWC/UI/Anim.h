/**
 * @file      Anim.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jul 28, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_UI_ANIM_H_
#define SWC_UI_ANIM_H_

#include "main.h"
#include "lcd.h"
#include "ugui.h"
#include "cmsis_os.h"
#include "Bitmaps.h"
#include "OSHandler.h"
#include "GPUResMan.h"
#include "UIMainSM.h"
#include "UI.h"
#include "stm32f4xx_hal_dma.h"
#include <stdio.h>
#include <string.h>

/* No animations release */
#define BETA_UI

void FadeWhiteIn(uint8_t animDelay);
void ITMLogoFadeIn(void);
void ITMLogoFadeOut(void);
void StringFadeIn(void);
void StringFadeOut(void);

#endif /* SWC_UI_ANIM_H_ */
