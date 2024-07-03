/*
 * UI.c
 *
 *  Created on: Jul 3, 2024
 *      Author: lord448
 */

#include "UI.h"

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream1;
extern osSemaphoreId_t xSemaphoreDMACompleteHandle;
extern const uint16_t ITMLogoData[ITMLOGO_SIZE];

static void DynamicErrorHandler(char *str);
static void Init(void); /*TODO: Give the proper name*/

/**
* @brief Function implementing the TaskUI thread.
* @param argument: Not used
* @retval None
*/
void vTaskUI(void *argument)
{
	uint16_t *ITMLogoRAMBuffer;
	uint16_t *NonWhitePixelsValue;
	uint32_t *NonWhitePixelsIndex;
	uint16_t PixelsIndex = 0;

	LCD_init();
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream1, HAL_DMA_XFER_CPLT_CB_ID, DMATrasferCpltCallback);

	/*Fade white in*/
	for(uint16_t r = 0, g = 0, b = 0; g < 63; r++, g+=2, b++)
	{
		UG_FillScreen(RGB565Color(r, g, b));
		UG_Update();
		osDelay(pdMS_TO_TICKS(20));
	}

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
  for(;;)
  {
  	LCD_Test();
  }
}

static void Init(void)
{

}

static void DynamicErrorHandler(char *str)
{
	/*Halting the task*/
	while(1)
	{
		/*Do nothing, check buffer with debugger*/
	}
}

void DMATrasferCpltCallback(DMA_HandleTypeDef *DmaHandle)
{
	osSemaphoreRelease(xSemaphoreDMACompleteHandle);
}
