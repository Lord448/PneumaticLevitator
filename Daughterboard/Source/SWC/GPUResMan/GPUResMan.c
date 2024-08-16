/**
 * @file     GPUResMan.c
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

#include "GPUResMan.h"

extern osMemoryPoolId_t MemoryPool8;  /*Memory Pool designed for members of 1 Byte size*/
extern osMemoryPoolId_t MemoryPool16; /*Memory Pool designed for members of 2 Byte size*/
extern osMemoryPoolId_t MemoryPool32; /*Memory Pool designed for members of 4 Byte size*/

extern osMemoryPoolId_t MemoryPool16_UI_PixelsValue; /*Component Specific Memory Pool*/
extern osMemoryPoolId_t MemoryPool16_UI_PixelsIndex; /*Component Specific Memory Pool*/

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream1; /*DMA Stream for 1 Byte data*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream4; /*DMA Stream for 2 Byte data*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream5; /*DMA Stream for 4 Byte data*/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0; /*DMA Stream for Empty fill (designed for 16 bit data)*/

extern osSemaphoreId_t xSemaphoreDMACplt0Handle; /*DMA Semaphore for Stream 0*/
extern osSemaphoreId_t xSemaphoreDMACplt3Handle; /*DMA Semaphore for Stream 3*/
extern osSemaphoreId_t xSemaphoreDMACplt4Handle; /*DMA Semaphore for Stream 4*/
extern osSemaphoreId_t xSemaphoreDMACplt5Handle; /*DMA Semaphore for Stream 5*/

extern osMessageQueueId_t xFIFOSetGPUReqHandle; /*FIFO to request dynamic allocation*/
extern osMessageQueueId_t xFIFOGetGPUBufHandle; /*FIFO to access the memory pool*/

/*Function Prototypes*/
static result_t allocateMemPool(osMemoryPoolId_t *MemPoolID, uint32_t size, const void *src, void **dest);
static result_t copyBufferData(osMemoryPoolId_t *MemPoolID, const void *src, void *dest, uint32_t size);
static result_t allocateEmptyMemPool(osMemoryPoolId_t *MemPoolID, uint32_t size, void **dest, bool cleanUp);
static result_t cleanMemoryPool(void *initAddr, uint32_t size);
static result_t changeMemoryPoolSize(osMemoryPoolId_t *MemPoolID, uint8_t newSize);
static void DMATransferCpltCallback(DMA_HandleTypeDef *DmaHandle);

/**
 * @brief  This function initializes the memory pools
 * @note   This function needs to be called after the scheduler starts
 * @param  none
 * @retval none
 */
void memoryPoolInit(void)
{
  MemoryPool8 = osMemoryPoolNew(INITIAL_POOL_SIZE, sizeof(uint8_t), NULL);
  MemoryPool16 = osMemoryPoolNew(INITIAL_POOL_SIZE, sizeof(uint16_t), NULL);
  MemoryPool32 = osMemoryPoolNew(INITIAL_POOL_SIZE, sizeof(uint32_t), NULL);

  MemoryPool16_UI_PixelsIndex = osMemoryPoolNew(INITIAL_POOL_SIZE, sizeof(uint16_t), NULL);
  MemoryPool16_UI_PixelsValue = osMemoryPoolNew(INITIAL_POOL_SIZE, sizeof(uint16_t), NULL);
}

/**
 * @brief  It allocates a memory pool with the selected
 *         constant video buffer allocated on FLASH.
 *
 * @note	 This function it's meant to be called on the
 *         task that need the memory pool service and it
 *         serves as an interface between the abstraction
 *         layer of the memory pool implementation.
 *         Consider that here the CPU has a different
 *         context.
 *
 * @param  MemPoolID: ID used by the OS
 * @param  Size: Size of the buffer that will be allocated
 * @param  *src: Pointer of the buffer that will be allocated
 * @param  Timeout: Timeout of the function gived on ticks
 * @retval void pointer with the address of the memory pool
 */
void *memoryRequestResource(osMemoryPoolId_t MemPoolID, uint32_t size, const void *src, uint32_t Timeout)
{
	void *memoryPoolAddr;
	/*Building the structure for the FIFO transmission*/
	GPUReq_t GPUReq = {
			.MemPoolID = MemPoolID,
			.Size = size,
			.OpReq.FillType = FillBuffer,
			.src = src
	};
	/*Sending to the FIFO*/
	if(osOK == osMessageQueuePut(xFIFOSetGPUReqHandle, &GPUReq, 0U, 0U))
	{
		/*Poll the caller task to wait the data processed*/
		osStatus_t result = osMessageQueueGet(xFIFOGetGPUBufHandle, &memoryPoolAddr, NULL, Timeout);
		switch(result)
		{
			case osError:
				memoryPoolAddr = NULL;
				/*TODO Trigger DTC Error on Memory Pool FIFO Get*/
			break;
			case osErrorTimeout:
				/*TODO Trigger DTC TimeoutError : Memory Pool FIFO Get*/
				memoryPoolAddr = NULL;
			break;
			case osErrorResource:
				/*TODO Trigger DTC osResource no available*/
				memoryPoolAddr = NULL;
			break;
			case osErrorNoMemory:
				/*TODO Trigger DTC Not Enough memory*/
				memoryPoolAddr = NULL;
			break;
			case osErrorParameter:
			case osErrorISR:
			case osStatusReserved:
				memoryPoolAddr = NULL;
			case osOK:
				/*Do Nothing, the pointer value has been charged through the API*/
			default:
			break;
		}
	}
	else
	{
		/*Cannot put the data on the FIFO*/
		/*TODO Trigger DTC Cannot send FIFO Data*/
		memoryPoolAddr = NULL;
	}
	return memoryPoolAddr;
}

void *memoryRequestEmptyPool(osMemoryPoolId_t MemPoolID, uint32_t size, uint32_t Timeout, GPUOpType_t OpType)
{
	void *memoryPoolAddr;
	/*Building the structure for the FIFO transmission*/
	GPUReq_t GPUReq = {
			.MemPoolID = MemPoolID,
			.Size = size,
			.OpReq.FillType = EmptyBuffer,
			.OpReq.OpType = OpType,
			.src = NULL
	};

	/*Sending to the FIFO*/
	if(osOK == osMessageQueuePut(xFIFOSetGPUReqHandle, &GPUReq, 0U, 0U))
	{
		/*Poll the caller task to wait the data processed*/
		osStatus_t result = osMessageQueueGet(xFIFOGetGPUBufHandle, &memoryPoolAddr, NULL, Timeout);

		switch(result)
		{
			case osError:
				memoryPoolAddr = NULL;
				/*TODO Trigger DTC Error on Memory Pool FIFO Get*/
			break;
			case osErrorTimeout:
				/*TODO Trigger DTC TimeoutError : Memory Pool FIFO Get*/
				memoryPoolAddr = NULL;
			break;
			case osErrorResource:
				/*TODO Trigger DTC osResource no available*/
				memoryPoolAddr = NULL;
			break;
			case osErrorNoMemory:
				/*TODO Trigger DTC Not Enough memory*/
				memoryPoolAddr = NULL;
			break;
			case osErrorParameter:
			case osErrorISR:
			case osStatusReserved:
				memoryPoolAddr = NULL;
			case osOK:
				/*Do Nothing, the pointer value has been charged through the API*/
			default:
			break;
		}
	}
	else
	{
		/*Cannot put the data on the FIFO*/
		/*TODO Trigger DTC Cannot send FIFO Data*/
		memoryPoolAddr = NULL;
	}
	return memoryPoolAddr;
}

/**
* @brief Function implementing the TaskGPUResMan thread.
* @param argument: Not used
* @retval None
*/
void vTaskGPUResMan(void *argument)
{
	GPUReq_t GPUResReq;
	void *GPUBuf = NULL;

#ifdef ALLOCATE_POOLS_AT_INIT
	uint8_t  *pBlock8 = NULL;
	uint16_t *pBlock16 = NULL;
	uint32_t *pBlock32 = NULL;
	uint16_t *pBlockPixelsIndex = NULL;
	uint16_t *pBlockPixelsValue = NULL;

	/*Allocate the memory pools*/
	pBlock8 = osMemoryPoolAlloc(MemoryPool8, osWaitForever);
	if(NULL == pBlock8)
	{
		/*TODO Trigger DTC Not enough memory 8Block*/
		/*TODO Turn on flag garbage*/
		while(1); /*Halting the task until it's destroyed by garbage collector*/
	}
	pBlock16 = osMemoryPoolAlloc(MemoryPool16, osWaitForever);
	if(NULL == pBlock16)
	{
		/*TODO Trigger DTC Not enough memory 16Block*/
		/*TODO Turn on flag garbage*/
		while(1); /*Halting the task until it's destroyed by garbage collector*/
	}
	pBlock32 = osMemoryPoolAlloc(MemoryPool32, osWaitForever);
	if(NULL == pBlock32)
	{
		/*TODO Trigger DTC Not enough memory 32Block*/
		/*TODO Turn on flag garbage*/
		while(1); /*Halting the task until it's destroyed by garbage collector*/
	}
	pBlockPixelsIndex = osMemoryPoolAlloc(MemoryPool16_UI_PixelsIndex, osWaitForever);
	if(NULL == pBlockPixelsIndex)
	{
		/*TODO Trigger DTC Not enough memory 32Block*/
		/*TODO Turn on flag garbage*/
		while(1); /*Halting the task until it's destroyed by garbage collector*/
	}
	pBlockPixelsValue = osMemoryPoolAlloc(MemoryPool16_UI_PixelsValue, osWaitForever);
	if(NULL == pBlockPixelsValue)
	{
		/*TODO Trigger DTC Not enough memory 32Block*/
		/*TODO Turn on flag garbage*/
		while(1); /*Halting the task until it's destroyed by garbage collector*/
	}

#endif

	/*Register all the DMA ISRs*/
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream0, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream1, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream4, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream5, HAL_DMA_XFER_CPLT_CB_ID, DMATransferCpltCallback);
	for(;;)
	{
		/*Getting request for the memory pool*/
		osMessageQueueGet(xFIFOSetGPUReqHandle, &GPUResReq, NULL, osWaitForever); /*Polling here the task*/

		switch(GPUResReq.OpReq.FillType)
		{
			case FillBuffer:
				if(OK != allocateMemPool(&GPUResReq.MemPoolID, GPUResReq.Size, GPUResReq.src, &GPUBuf))
				{
					/*TODO All possible DTCs are covered, make a retry*/
					GPUBuf = NULL;
				}
				else
				{
					/*Do Nothing*/
				}
			break;
			case EmptyBuffer:
				if(OK != allocateEmptyMemPool(&GPUResReq.MemPoolID, GPUResReq.Size, &GPUBuf, true))
				{
					/*TODO All possible DTCs are covered, make a retry*/
					GPUBuf = NULL;
				}
				else
				{
					/*Do Nothing*/
				}
			break;
			default:
				GPUBuf = NULL;
			break;
		}
		/*Sending address of the memory allocated*/
		osMessageQueuePut(xFIFOGetGPUBufHandle, &GPUBuf, 0U, 0U);
	}
}

/**
 * @brief  Free the memory pool allocated
 * @param  MemPoolID: ID of the OS resource
 * @param  Address block associated to the memory pool
 * @retval result of the operation
 */
result_t FreeMemoryPool(osMemoryPoolId_t MemPoolID, void *block)
{
	/*TODO Add safe logic here or make this function a macro*/
	return osOK == osMemoryPoolFree(MemPoolID, block) ? OK : Error;
}

/**
 * @brief  Allocates the memory pool on the heap system, and fills the the allocated buffer
 *         with the information gived by the *src pointer, it can be filled using DMA
 *         hardware acceleration or with the CPU using "memcpy" the "USE_HARDWARE_ACC" symbol
 *         can be used to select this option
 *
 * @note   This function it's meant to be used inside the SWC task, so it can be controlled
 *         with the rest of the services
 *
 * @param  MemPoolID: ID of the OS resource
 * @param  size: Size of the buffer that will be allocated and filled with the data
 * @param  *src: Pointer of the data to fill
 * @param  *dest: Pointer of the data that it's filled
 * @retval result of the operation
 */
static result_t allocateMemPool(osMemoryPoolId_t *MemPoolID, uint32_t size, const void *src, void **dest)
{
	result_t retval = Error; /*Result of the operation*/
	void *BufAddr; /*Local pointer for the address allocation*/

#ifdef MAXIMUM_POOL_SIZE
	if(MAXIMUM_POOL_SIZE >= size)
#else
	if(1)
#endif
	{
		/*Size inside the bounds*/
		uint32_t poolSize = osMemoryPoolGetCapacity(*MemPoolID);

		if(poolSize != size && poolSize != MEMORY_POOL_SIZE_ERROR)
		{
			/*The memory pool doesn't have the required size*/
			retval = changeMemoryPoolSize(MemPoolID, size);
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
			BufAddr = osMemoryPoolAlloc(*MemPoolID, 0U); /*Allocating the memory*/
			if(NULL == BufAddr)
			{
				/*Cannot allocate the memory pool*/
				/*TODO Trigger DTC "Not enough memory for memory pool "X"*/
				retval = Error;
			}
			else
			{
				/*Memory Pool allocated succesfully*/
				if(OK == copyBufferData(MemPoolID, src, BufAddr, size))
				{
					/*Data copied to the buffer*/
					*dest = BufAddr;
					retval = OK;
				}
				else
				{
					/*Cannot copy the data to the buffer*/
					retval = Error;
					/*TODO Trigger DTC "Cannot copy buffer"*/
				}
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
 * @brief  Copy the data from src, or dest depending of the option
 *         selected by "USE_HARDWARE_ACC" symbol
 * @param  MemPoolID: ID of the OS resource
 * @param  *src: Pointer of the data to fill
 * @param  *dest: Pointer of the data that it's filled
 * @param  size: Size of the buffer that will be copied
 * @retval result of the operation
 */
static result_t copyBufferData(osMemoryPoolId_t *MemPoolID, const void *src, void *dest, uint32_t size)
{
	result_t result = OK;
#ifdef USE_HARDWARE_ACC
	DMA_HandleTypeDef DMAHandle;
	osSemaphoreId_t semaphore_id;

	/*Asking what stream is needed*/
	if(MemoryPool8 == MemPoolID)
	{
		DMAHandle = hdma_memtomem_dma2_stream1;
		semaphore_id = xSemaphoreDMACplt3Handle;
	}
	else if(MemoryPool16 == MemPoolID)
	{
		DMAHandle = hdma_memtomem_dma2_stream4;
		semaphore_id = xSemaphoreDMACplt4Handle;
	}
	else /*if(MemoryPool32 == MemPoolID)*/
	{
		DMAHandle = hdma_memtomem_dma2_stream5;
		semaphore_id = xSemaphoreDMACplt5Handle;
	}
	/*Start DMA transfer*/
	if(HAL_OK == HAL_DMA_Start_IT(&DMAHandle, (uint32_t)src, (uint32_t)dest, size))
	{
		/*Wait till the transfer is completed*/
		result = osOK == osSemaphoreAcquire(semaphore_id, osWaitForever) ? OK : Error;
	}
	else /*if(MemoryPool32 == *MemPoolID)*/
	{
		result = Error;
	}
#else
	memcpy(dest, src, size); /*The CPU moves the buffer*/
#endif
	return result;
}

/**
 * @brief
 * @param
 * @param
 * @retval
 */
static result_t allocateEmptyMemPool(osMemoryPoolId_t *MemPoolID, uint32_t size, void **dest, bool cleanUp)
{
	result_t retval = Error; /*Result of the operation*/
	void *BufAddr; /*Local pointer for the address allocation*/

#ifdef MAXIMUM_POOL_SIZE
	if(MAXIMUM_POOL_SIZE >= size)
#else
	if(1)
#endif
	{
		/*Size inside the bounds*/
		uint32_t poolSize = osMemoryPoolGetCapacity(*MemPoolID);

		if(poolSize != size && poolSize != MEMORY_POOL_SIZE_ERROR)
		{
			/*The memory pool doesn't have the required size*/
			retval = changeMemoryPoolSize(MemPoolID, size);
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
			BufAddr = osMemoryPoolAlloc(*MemPoolID, 0U); /*Allocating the memory*/
			if(NULL == BufAddr)
			{
				/*Cannot allocate the memory pool*/
				/*TODO Trigger DTC "Not enough memory for memory pool "X"*/
				retval = Error;
			}
			else
			{
				/*Memory pool allocated successfully*/
				if(cleanUp)
				{
					retval = cleanMemoryPool(&BufAddr, size);
				}
				else
				{
					retval = OK;
				}
			}
		}
	}
	return retval;
}

/**
 * @brief
 * @param
 * @param
 * @retval
 */
static result_t cleanMemoryPool(void *initAddr, uint32_t size)
{
	result_t retval = OK;
	const uint16_t ValueToFill = 0;
#ifdef USE_HARDWARE_ACC
	if(HAL_OK == HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0, (uint32_t)&ValueToFill, (uint32_t)initAddr, size))
	{
		retval = osOK == osSemaphoreAcquire(xSemaphoreDMACplt0Handle, osWaitForever) ? OK : Error;
	}
	else
	{
		/*TODO Trigger DTC cannot use DMA*/
		retval = Error;
	}
#else
	memset(&initAddr, size);
#endif
	return retval;
}

/**
 * @brief  Change the size of a memory pool by destroying
 *         the OS object and creating a new one with the desired size
 * @param  MemPoolID: ID of the OS resource
 * @param  newSize: New Size of the memory pool
 * @retval result of the operation
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
			/*TODO Trigger DTC Not enough RAM memory*/
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
 * @brief  ISR that it's executed when the DMA has finished
 *         the data transfer and notifies to the OS that the
 *         task can continue with the operation
 * @param  *DmaHandle: Handler of the HAL layer for the DMA
 * @retval none
 */
static void DMATransferCpltCallback(DMA_HandleTypeDef *DmaHandle)
{
	if(DMA2_Stream0 == DmaHandle->Instance)
	{
		/*It's DMA Stream 0 - Fill with zero*/
		osSemaphoreRelease(xSemaphoreDMACplt0Handle);
	}
	else if(DMA2_Stream3 == DmaHandle->Instance)
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
