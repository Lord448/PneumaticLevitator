/**
 * @file     GPUResMan.h
 * @author   Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief    This component it's meant to load all the
 * 			     graphical resources on memory pools and deliver
 * 			     with hardware acceleration (DMA) to deliver on
 * 			     memory pools with dynamic allocation to other
 * 			     components that need a large amount of data
 *
 * 			     This component allows at least (TODO) of dynamic
 * 			     memory buffers this component can reach only a
 * 			     maximum size defined of "MAXIMUM_POOL_SIZE"
 *
 * @date     Jul 5, 2024
 *
 * @license  This Source Code Form is subject to the terms of
 *           the Mozilla Public License, v. 2.0. If a copy of
 *           the MPL was not distributed with this file, You
 *           can obtain one at https://mozilla.org/MPL/2.0/.
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
#define ALLOCATE_POOLS_AT_INIT /*When defined the OS allocate the memory pools at the start of "vTaskGPUResMan"*/

#define Byte_16To8(x) x/2 /*Macro to convert 16 bit format size to 8 bit*/
#define Byte_32To8(x) x/4 /*Macro to convert 32 bit format size to 8 bit*/

#define INITIAL_POOL_SIZE 1   /*Bytes*/
//#define MAXIMUM_POOL_SIZE 20480 /*20KB*/

#define MEMORY_POOL_SIZE_ERROR 0 /*Error code returned from "osMemoryPoolGetCapacity()"*/

typedef struct GPUReq_t{
	osMemoryPoolId_t MemPoolID;
	uint16_t Size;
	const void *src;
}GPUReq_t;

void memoryPoolInit(void);
void vTaskGPUResMan(void *argument);
void *memoryRequestResource(osMemoryPoolId_t MemPoolID, uint32_t Size, const void *src, uint32_t Timeout);
result_t FreeMemoryPool(osMemoryPoolId_t MemPoolID, void *block);

#endif /* SWC_GPURESMAN_GPURESMAN_H_ */
