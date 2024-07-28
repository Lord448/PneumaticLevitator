/**
 * @file      Anim.c
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

#include "Anim.h"

/**
 * @brief Fades in the white color on the screen
 * @param Delay for the animation in milliseconds
 * @retval none
 */
void FadeWhiteIn(uint8_t animDelay)
{
	for(uint16_t r = 0, g = 0, b = 0; g < 63; r++, g+=2, b++)
	{
		UG_FillScreen(RGB565Color(r, g, b));
		UG_Update();
		osDelay(pdMS_TO_TICKS(animDelay));
	}
	UG_FillScreen(C_WHITE);
	UG_Update();
}

/**
 * @brief Fades in and then fades out the logo
 * @param none
 * @retval none
 */
void ITMLogoFadeIn(void)
{
#ifdef BETA_UI
	UG_DrawBMP((LCD_WIDTH-ITMLogo.width)/2, (LCD_HEIGHT-ITMLogo.height)/2-15, &ITMLogo);
	UG_Update();
#else
	/* TODO This feature won't be release on the first version */
	uint16_t *ITMLogoRAMBuffer;
	uint16_t *NonWhitePixelsValue;
	uint16_t *NonWhitePixelsIndex;
	uint16_t PixelsIndex = 0;

	ITMLogoRAMBuffer = (uint16_t*)memoryRequestResource(MemoryPool16, ITMLOGO_SIZE*2, ITMLogoData, osWaitForever);
	/*Calloc Failure*/
	if(ITMLogoRAMBuffer == NULL)
	{
		/*Trigger DTC not enough RAM*/
		/*Skip the animation*/
		/*Report for the debugger*/
		DynamicErrorHandler("ITMLogoRAMBuffer"); /*TODO remove this*/
	}
	else
	{
		/*Do Nothing*/
	}

	UG_BMP ITMLogoRAM = {
		.p = ITMLogoRAMBuffer,
		.width = 161,
		.height = 153,
		.bpp = BMP_BPP_16
	};

	/*Getting the non white pixels*/
	for(uint16_t i = 0; i < ITMLOGO_SIZE*2; i++)
	{
		if((uint16_t)ITMLogoRAMBuffer[i] != WHITE_PIXEL)
		{
			/*Found a non white pixel*/
			PixelsIndex++;
		}
	}
	/*By here we have the amount of non white pixels, so we can allocate the pools*/
	NonWhitePixelsIndex = (uint16_t*)memoryRequestEmptyPool(MemoryPool16_UI_PixelsIndex, PixelsIndex, osWaitForever, MallocType);
	NonWhitePixelsValue = (uint16_t*)memoryRequestEmptyPool(MemoryPool16_UI_PixelsValue, PixelsIndex, osWaitForever, MallocType);
	if(NonWhitePixelsValue == NULL)
	{
		/*Trigger DTC not enough RAM*/
		/*Skip the animation*/
		/*Report for the debugger*/
		DynamicErrorHandler("NonWhitePixelsValue"); /*TODO remove this*/
	}
	else if(NonWhitePixelsIndex == NULL)
	{
		/*Trigger DTC not enough RAM*/
		/*Skip the animation*/
		/*Report for the debugger*/
		DynamicErrorHandler("NonWhitePixelsIndex"); /*TODO remove this*/
	}
	else
	{
		/*Do Nothing*/
	}
	PixelsIndex = 0; /*Reusing variable for as index for the buffers*/
	for(uint16_t i = 0; i < ITMLOGO_SIZE*2; i++)
	{
		if((uint16_t)ITMLogoRAMBuffer[i] != 0xFFFF)
		{
			/*Found a non white pixel*/
			NonWhitePixelsValue[PixelsIndex] = ITMLogoRAMBuffer[i];
			NonWhitePixelsIndex[PixelsIndex] = i;
			PixelsIndex++; /*At the end of the loop, will have the max number of data needed*/
		}
	}

	/*Fade animation logo in*/
	memset(ITMLogoRAMBuffer, 0xFFFF, ITMLOGO_SIZE*2); /*Setting white buffer*/
	ITMLogoRAM.p = ITMLogoRAMBuffer;
	while(memcmp(ITMLogoRAMBuffer, ITMLogoData, ITMLOGO_SIZE) != 0)
	{
		for(uint16_t i = 0; i < PixelsIndex; i++)
		{
			if(ITMLogoRAMBuffer[NonWhitePixelsIndex[i]] != NonWhitePixelsValue[i])
			{
				ITMLogoRAMBuffer[NonWhitePixelsIndex[i]]--; /*Until it gets the color*/
			}
		}
		ITMLogoRAM.p = ITMLogoRAMBuffer;
		UG_DrawBMP((LCD_WIDTH-ITMLogoRAM.width)/2, (LCD_HEIGHT-ITMLogoRAM.height)/2, &ITMLogoRAM);
		UG_Update();
		osDelay(pdMS_TO_TICKS(10));
	}

	UG_DrawBMP((LCD_WIDTH-ITMLogoRAM.width)/2, (LCD_HEIGHT-ITMLogoRAM.height)/2, &ITMLogoRAM);
	UG_Update();
	osDelay(pdMS_TO_TICKS(3000));

	free(ITMLogoRAMBuffer);
	free(NonWhitePixelsValue);
	free(NonWhitePixelsIndex);
	/*!HIGH RAM CONSUMPTION ZONE*/
#endif
}

void ITMLogoFadeOut(void)
{
#ifdef BETA_UI
	UG_FillScreen(C_WHITE);
	UG_Update();
#endif
}

void StringFadeIn(void)
{
#ifdef BETA_UI
	LCD_PutStr(10, LCD_HEIGHT-40, "Pneumatic Levitator", FONT_arial_25X28_CYRILLIC, C_BLACK, C_WHITE);
	UG_Update();
#else
	/* TODO Implement on future releases */
#endif
}

void StringFadeOut(void)
{
  /* TODO Implement on future releases */
}
