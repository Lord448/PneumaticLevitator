/**
 * @file      GPUResMan.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jul 5, 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_GPURESMAN_GPURESMAN_H_
#define SWC_GPURESMAN_GPURESMAN_H_

#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal_dma.h"
#include <stdio.h>
#include <string.h>

#define USE_HARDWARE_ACC /*When defined the hardware acceleration with DMA is enabled*/

#define Byte_16To8(x) x/2 /*Macro to convert 16 bit format size to 8 bit*/
#define Byte_32To8(x) x/4 /*Macro to convert 32 bit format size to 8 bit*/

#define DEFAULT_POOL_SIZE 256   /*Bytes*/
#define MAXIMUM_POOL_SIZE 20480 /*20KB*/

#define MEMORY_POOL_SIZE_ERROR 0 /*Error code returned from "osMemoryPoolGetCapacity()"*/

typedef struct GPUReq_t{
	osMemoryPoolId_t MemPoolID;
	uint16_t BufferID;
	uint16_t Size;
	void *src;
}GPUReq_t;

void vTaskGPUResMan(void *argument);
void *requestMemoryPoolAlloc(osMemoryPoolId_t MemPoolID, uint8_t Size, void *src);
result_t FreeMemoryPool(osMemoryPoolId_t MemPoolID);

#endif /* SWC_GPURESMAN_GPURESMAN_H_ */
