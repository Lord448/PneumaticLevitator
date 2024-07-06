/**
 * @file      GPUResMan.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     This component it's meant to load all the
 * 						graphical resources on memory pools and deliver
 * 						with hardware acceleration (DMA) to deliver on
 * 						memory pools with dynamic allocation to other
 * 						components that need a large amount of data
 *
 * 						This component allows at least (TODO) of dynamic
 * 						memory buffers this component can reach only a
 * 						maximum size of 20KB (TODO Check the limit
 * 						depending on RAM consumption)
 *						Total RAM consumption on memory pools: -60KB-
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

#include "GPUResMan.h"

osMemoryPoolId_t MemoryPool8;  /*Memory Pool designed for members of 1 Byte size*/
osMemoryPoolId_t MemoryPool16; /*Memory Pool designed for members of 2 Byte size*/
osMemoryPoolId_t MemoryPool32; /*Memory Pool designed for members of 4 Byte size*/

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream3; /*DMA Stream for 1 Byte data*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream4; /*DMA Stream for 2 Byte data*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream5; /*DMA Stream for 4 Byte data*/

extern osSemaphoreId_t xSemaphoreDMACplt3Handle; /*DMA Semaphore for Stream 3*/
extern osSemaphoreId_t xSemaphoreDMACplt4Handle; /*DMA Semaphore for Stream 4*/
extern osSemaphoreId_t xSemaphoreDMACplt5Handle; /*DMA Semaphore for Stream 5*/

extern osMessageQueueId_t xFIFOSetGPUReqHandle; /*FIFO to request dynamic allocation*/
extern osMessageQueueId_t xFIFOGetGPUBufHandle; /*FIFO to access the memory pool*/

/*Function Prototypes*/
static result_t changeMemoryPoolSize(osMemoryPoolId_t *MemPoolID, uint8_t newSize);
static result_t allocateMemPool(osMemoryPoolId_t *MemPoolID, uint8_t Size, void *src, void *dest);
static void DMATransferCpltCallback(DMA_HandleTypeDef *DmaHandle);

/**
 * @brief
 * @param
 * @retval
 */
void vTaskGPUResMan(void *argument)
{
	GPUReq_t GPUResReq;
	void *GPUBuf = NULL;

	/*Register all the DMA ISRs*/
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream3, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream4, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream5, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	for(;;)
	{
		osMessageQueueGet(xFIFOSetGPUReqHandle, &GPUResReq, NULL, osWaitForever); /*Getting request for the memory pool*/
		if(OK != allocateMemPool(GPUResReq.MemPoolID, GPUResReq.Size, GPUResReq.src, GPUBuf))
		{
			/*TODO All DTCs are covered, make a retry*/
		}
		else
		{
			/*Sending address of the memory allocated*/
			osMessageQueuePut(xFIFOGetGPUBufHandle, GPUBuf, 0U, 0U);
		}
	}
}

/**
 * @brief
 * @param
 * @retval
 */
result_t FreeMemoryPool(osMemoryPoolId_t MemPoolID)
{

}

/**
 * @brief
 * @param
 * @param
 * @param
 * @param
 * @retval
 */
static result_t allocateMemPool(osMemoryPoolId_t *MemPoolID, uint8_t Size, void *src, void *dest)
{
	result_t retval = Error; /*Result of the operation*/
	void *BufferLocation;    /*Local buffer of the allocation*/

	if(MAXIMUM_POOL_SIZE >= Size)
	{
		/*Size inside the bounds*/
		uint32_t poolSize = osMemoryPoolGetCapacity(*MemPoolID);

		if(poolSize != Size && poolSize != MEMORY_POOL_SIZE_ERROR)
		{
			/*The memory pool doesn't have the required size*/
			retval = changeMemoryPoolSize(MemPoolID, Size);
		}
		else if(MEMORY_POOL_SIZE_ERROR == poolSize)
		{
			/*The memory pool were not allocated*/
			/*TODO Trigger DTC "Memory Pool "X" were not allocated"*/
			retval = Error;
		}
		else
		{
			/*Do Nothing, The memory pool have the required size*/
		}

		if(Error != retval)
		{
			BufferLocation = osMemoryPoolAlloc(*MemPoolID, 0U); /*Allocating the memory*/
			if(NULL == BufferLocation)
			{
				/*TODO Trigger DTC "Not enough memory for memory pool "X"*/
				retval = Error;
			}
			else
			{

				/*TODO Fill the memory pool with the requested data*/
#ifdef USE_HARDWARE_ACC
				/*TODO Halt here the task*/
#else

#endif
				dest = BufferLocation;
				retval = OK;
			}
		}
		else
		{
			/*Do Nothing, already reported DTC*/
		}
	}
	else
	{
		/*Trying to allocate more memory than the allowed*/
		/*TODO Trigger DTC Trying to allocate more than "MAXIMUM_POOL_SIZE"*/
		retval = Warning;
	}
	return retval;
}

/**
 * @brief
 * @param
 * @param
 * @retval
 */
static result_t changeMemoryPoolSize(osMemoryPoolId_t *MemPoolID, uint8_t newSize)
{
	result_t retval = OK;
	uint32_t block_size = osMemoryPoolGetBlockSize(*MemPoolID);

	retval = osMemoryPoolDelete(*MemPoolID);
	if((result_t)osOK == retval)
	{
		/*Creating the new memory pool*/
		*MemPoolID = osMemoryPoolNew(newSize, block_size, NULL);
		if(NULL == *MemPoolID)
		{
			/*Cannot create the memory pool*/
			/*TODO Trigger DTC Cannot create "X" memory pool*/
			retval = Error;
		}
		else
		{
			retval = OK;
		}
	}
	else
	{
		/*Do Nothing due to errors*/
	}
	return retval;
}

/**
 * @brief
 * @param
 * @retval
 */
static void DMATransferCpltCallback(DMA_HandleTypeDef *DmaHandle)
{
	if(DMA2_Stream3 == DmaHandle->Instance)
	{
		/*It's DMA Stream 3 - 8 Bit*/
		osSemaphoreRelease(xSemaphoreDMACplt3Handle);
	}
	else if(DMA2_Stream4 == DmaHandle->Instance)
	{
		/*It's DMA Stream 4 - 16 Bit*/
		osSemaphoreRelease(xSemaphoreDMACplt4Handle);
	}
	else /*if(DMA2_Stream5 == DmaHandle->Instance)*/
	{
		/*It's DMA Stream 5 - 32 Bit*/
		osSemaphoreRelease(xSemaphoreDMACplt5Handle);
	}
}
