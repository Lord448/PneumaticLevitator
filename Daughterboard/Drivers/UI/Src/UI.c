/**
 * @file   UI.c
 * @author Pedro Rojo (pedroeroca@outlook.com)
 * @brief
 * @note Considering 1ms parsed loop
 * @date 2023-12-27
 *
 * @copyright Copyright (c) 2023
 * @license
 */

#include "UI.h"

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern TIM_HandleTypeDef htim2;
extern UG_BMP ITMLogo;
const uint8_t ITMLogoID = 1;

//TODO Make fade animation of the school logo
static void vInitialAnim(MenuStage *stage)
{
	/**
	 * Fade the white color
	 * Fade the Image
	 * Fade the image out
	 * Fade the Text
	 * Fade the text out
	 * Finish with white color
	 */
	enum stages{
		FadeIn,
		DMATransfer,
		AnalyzeBMP,
		FadeInIm,
		FadeOutIm,
		ShowVer,
		FadeWhite,
		Finish
	}static states = FadeIn;
	static uint8_t r = 0, g = 0, b = 0;
	static uint16_t ForCounter = 0;
	static uint16_t DelayCounter = 0;
	static uint16_t *ITMLogoRAMBuffer;
	static uint32_t *PixelsAddr;
	static uint16_t *PixelsValues;
	static uint16_t PixelsIndex = 1;
	UG_BMP ITMLogoRAM = {
		.p = ITMLogoRAMBuffer,
		.width = 161,
		.height = 153,
		.bpp = BMP_BPP_16
	};

	switch(states)
	{
		case FadeIn:
			if(g >= 63)
				states = DMATransfer;
			else
			{
				if(DelayCounter == 20) //20ms Delay for animations
				{
					UG_FillScreen(RGB565Color(r, g, b));
					UG_Update();
					r++;
					g+=2;
					b++;
					DelayCounter = 0;
				}
			}
			DelayCounter++;
		break;
		case DMATransfer:
			UG_FillScreen(C_WHITE);
			UG_Update();
			ITMLogoRAMBuffer = (uint16_t*)calloc(ITMLOGO_SIZE, sizeof(uint16_t));
			if(ITMLogoRAMBuffer != NULL)
			{
				//TODO Correct the DMA transfer
				if(HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0, (uint32_t)&ITMLogo.p, (uint32_t)&ITMLogoRAMBuffer, ITMLOGO_SIZE) != HAL_OK)
				{
					Error_Handler();
				}
				//Wait for DMA to finish
				HAL_TIM_Base_Stop_IT(&htim2);
				HAL_SuspendTick();
				HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
				HAL_ResumeTick();
				HAL_TIM_Base_Start_IT(&htim2);
				PixelsAddr = (uint32_t*)calloc(13068, sizeof(uint32_t));
				PixelsValues = (uint16_t*)calloc(13068, sizeof(uint16_t));
				states = FadeInIm;
				if(PixelsAddr == NULL)
					Error_Handler();
			}
			else
			{
				Error_Handler();
			}
		break;
		case AnalyzeBMP:
			//TODO Make fade animation of the school logo
			for(uint16_t i = 0; i < 1024; i++, ForCounter++)
			{
				if(ForCounter >= ITMLOGO_SIZE)
				{
					states = FadeInIm;
					ForCounter = ITMLOGO_SIZE;
					break;
				}
				else if(ITMLogoRAMBuffer[ForCounter] != 0xFFFF) //Searching for non white pixels
				{
					PixelsAddr[PixelsIndex] = (uint32_t)&ITMLogoRAMBuffer[ForCounter]; //Getting the address of the pixel
					PixelsValues[PixelsIndex] = ITMLogoRAMBuffer[ForCounter]; //Getting the value of the pixel
					PixelsIndex++;
				}
			}
		break;
		case FadeInIm:
			UG_DrawBMP((LCD_WIDTH-ITMLogo.width)/2, (LCD_HEIGHT-ITMLogo.height)/2, &ITMLogoRAM);
			UG_Update();
			states = FadeOutIm;
		break;
		case FadeOutIm:
			if(DelayCounter == 1500)
			{
				UG_FillScreen(C_WHITE);
				UG_Update();
				//TODO Adjust positions
				LCD_PutStr(20, 2, "Firmware", FONT_arial_49X57, C_BLACK, C_WHITE);
				LCD_PutStr(30, 60, "version", FONT_arial_49X57, C_BLACK, C_WHITE);
				LCD_PutStr(40, 130, FIRMWARE_VERSION, FONT_arial_49X57, C_BLACK, C_WHITE);
				LCD_PutStr(30, 188, "(Beta)", FONT_arial_49X57, C_BLACK, C_WHITE);
				UG_Update();
				states = ShowVer;
				DelayCounter = 0;
			}
			DelayCounter++;
		break;
		case ShowVer:

			if(DelayCounter == 1500)
			{
				states = FadeWhite;
				DelayCounter = 0;
			}

			DelayCounter++;
		break;
		case FadeWhite:
			UG_FillScreen(C_WHITE);
			UG_Update();
			states = ShowVer;
		break;
		case Finish:
			free(ITMLogoRAMBuffer);
			free(PixelsAddr);
			free(PixelsValues);
		break;
	}
}
static void vMainLobby(MenuStage *stage, Input *input)
{

}
static void vMenu(MenuStage *stage, Input *input)
{

}
static void vConfigurationMenu(MenuStage *stage, Input *input)
{

}
static void vAboutMenu(MenuStage *stage, Input *input)
{

}
static void vPlotMenu(MenuStage *stage, Input *input)
{

}
static void vUSBConfigMenu(MenuStage *stage, Input *input)
{

}
static void vStepResponseMenu(MenuStage *stage, Input *input)
{

}

uint16_t RGB565Color(uint8_t r, uint8_t g, uint8_t b)
{
	return (uint16_t)(r<<11 | g<<5 | b);
}

void UIDrawCenterBitmap(UG_BMP* bmp)
{
	UG_DrawBMP((LCD_WIDTH-bmp->width)/2, (LCD_HEIGHT-bmp->height)/2, bmp);
}

void vTaskUI(MenuStage *stage, Input *input)
{
	switch(*stage)
	{
		case sInitialAnim:
			vInitialAnim(stage);
		break;
		case sMainLobby:
			vMainLobby(stage, input);
		break;
		case sMenu:
			vMenu(stage, input);
		break;
		case sConfiguration:
			vConfigurationMenu(stage, input);
		break;
		case sAbout:
			vAboutMenu(stage, input);
		break;
		case sPlot:
			vPlotMenu(stage, input);
		break;
		case sUSBConfig:
			vUSBConfigMenu(stage, input);
		break;
		case sStepResponse:
			vStepResponseMenu(stage, input);
		break;
	}
}

void UIWindowCreate(UIWindow *window, ...)
{
	va_list args;
	va_start(args, *window);
	void (*pCallback)(UG_MESSAGE *) = va_arg(args, void(*)(UG_MESSAGE*));
	UG_WindowCreate(&window->window, window->obj_buff, window->obj_len, pCallback);
	va_end(args);
}
