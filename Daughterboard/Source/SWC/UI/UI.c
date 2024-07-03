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

extern osEventFlagsId_t xEventFinishedInitHandle;
extern osSemaphoreId_t xSemaphoreDMACompleteHandle;
extern osSemaphoreId_t xSemaphoreCOMReadyHandle;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream1;
extern const uint16_t ITMLogoData[ITMLOGO_SIZE];

static void FadeWhiteIn(uint8_t animDelay);
static void ITMLogoFadeIn(void);
static void ITMLogoFadeOut(void);
static void DynamicErrorHandler(char *str);
static void StringFadeIn(void);
static void StringFadeOut(void);

/**
* @brief Function implementing the TaskUI thread.
* @param argument: Not used
* @retval None
*/
void vTaskUI(void *argument)
{
	LCD_init();
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream1, HAL_DMA_XFER_CPLT_CB_ID, DMATrasferCpltCallback);
	FadeWhiteIn(10);
	ITMLogoFadeIn();
	StringFadeIn();
	/*Charge all the graphic resources in BG*/
	/*Report to motherboard that all is ready to run*/
	osSemaphoreAcquire(xSemaphoreCOMReadyHandle, osWaitForever); /*Wait until Motherboard is ready*/
	StringFadeOut();
	/*Letting the rest of the tasks to run*/
	osEventFlagsSet(xEventFinishedInitHandle, FINISH_INIT_ID);
	ITMLogoFadeOut();

  for(;;)
  {
  	/*TODO: Possible implementation of parsed loop scheme*/
  	LCD_Test(); /*TODO Remove the test from here*/
  }
}

/**
 * @brief Fades in the white color on the screen
 * @param Delay for the animation in milliseconds
 * @retval none
 */
static void FadeWhiteIn(uint8_t animDelay)
{
	for(uint16_t r = 0, g = 0, b = 0; g < 63; r++, g+=2, b++)
	{
		UG_FillScreen(RGB565Color(r, g, b));
		UG_Update();
		osDelay(pdMS_TO_TICKS(animDelay));
	}
}

/**
 * @brief Fades in and then fades out the logo
 * @param none
 * @retval none
 */
static void ITMLogoFadeIn(void)
{
	uint16_t *ITMLogoRAMBuffer;
	uint16_t *NonWhitePixelsValue;
	uint32_t *NonWhitePixelsIndex;
	uint16_t PixelsIndex = 0;

	/*TODO: Change to memory pool to use heap memory instead of stack*/
	ITMLogoRAMBuffer = (uint16_t*)calloc(ITMLOGO_SIZE*2, sizeof(uint16_t));
	/*Calloc Failure*/
	if(ITMLogoRAMBuffer == NULL)
	{
		/*Report for the debugger*/
		DynamicErrorHandler("ITMLogoRAMBuffer");
	}
	else
	{
		/*Do Nothing*/
	}
	HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream1, (uint32_t)ITMLogoData, (uint32_t)ITMLogoRAMBuffer, ITMLOGO_SIZE);
	osSemaphoreAcquire(xSemaphoreDMACompleteHandle, osWaitForever);

	UG_BMP ITMLogoRAM = {
		.p = ITMLogoRAMBuffer,
		.width = 161,
		.height = 153,
		.bpp = BMP_BPP_16
	};

	/*Getting the non white pixels*/
	NonWhitePixelsValue = (uint16_t*)calloc(ITMLOGO_SIZE, sizeof(uint16_t));
	NonWhitePixelsIndex = (uint32_t*)calloc(ITMLOGO_SIZE, sizeof(uint32_t));
	if(NonWhitePixelsValue == NULL)
	{
		DynamicErrorHandler("NonWhitePixelsValue");
	}
	else if(NonWhitePixelsIndex == NULL)
	{
		DynamicErrorHandler("NonWhitePixelsIndex");
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
	/*Free RAM that is not used*/
	NonWhitePixelsValue = realloc(NonWhitePixelsValue, (PixelsIndex+1)*sizeof(uint16_t));
	NonWhitePixelsIndex = realloc(NonWhitePixelsIndex, (PixelsIndex+1)*sizeof(uint32_t));

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
		osDelay(pdMS_TO_TICKS(20));
	}

	UG_DrawBMP((LCD_WIDTH-ITMLogoRAM.width)/2, (LCD_HEIGHT-ITMLogoRAM.height)/2, &ITMLogoRAM);
	UG_Update();
	osDelay(pdMS_TO_TICKS(3000));

	free(ITMLogoRAMBuffer);
	free(NonWhitePixelsValue);
	free(NonWhitePixelsIndex);
	/*!HIGH RAM CONSUMPTION ZONE*/
}

static void ITMLogoFadeOut(void)
{

}

static void StringFadeIn(void)
{

}

static void StringFadeOut(void)
{

}

static void DynamicErrorHandler(char *str)
{
	/*Halting the task*/
	while(1)
	{
		/*Do nothing, check buffer "str" with debugger*/
	}
}

void DMATrasferCpltCallback(DMA_HandleTypeDef *DmaHandle)
{
	osSemaphoreRelease(xSemaphoreDMACompleteHandle);
}
