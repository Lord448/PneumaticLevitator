/**
 * @file      NVM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      May 30, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#include "NVM.h"

extern I2C_HandleTypeDef hi2c2;
extern osSemaphoreId_t xSemaphore_DMA_TransferCpltHandle;
extern osSemaphoreId_t xSemaphore_MemoryPoolUsedHandle;
extern osMemoryPoolId_t MemoryPoolNVM; /* Memory Pool for NVM data allocation*/

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */

static result_t SendUSB(char *format, ...); /*TODO Erase when COM it's finished*/
static result_t transferEEPROMData(uint16_t startAddr, uint16_t endAddr, uint8_t *memoryPool, uint32_t timeout);

/**
 * ---------------------------------------------------------
 * 					          INIT FUNCTION
 * ---------------------------------------------------------
 */
void NVM_Init(void)
{
	osSemaphoreRelease(xSemaphore_MemoryPoolUsedHandle);
}

/**
 * ---------------------------------------------------------
 * 					 FUNCTION OVERLOADING FOR "nvmSave"
 * ---------------------------------------------------------
 */

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with 8 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmSave_8Bit(uint16_t NVMVariable, uint8_t value)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with 16 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmSave_16Bit(uint16_t NVMVariable, NVMType16 value)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with 32 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmSave_32Bit(uint16_t NVMVariable, NVMType32 value)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with float data
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmSave_float(uint16_t NVMVariable, float value)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * ---------------------------------------------------------
 * 					 FUNCTION OVERLOADING FOR "nvmRead"
 * ---------------------------------------------------------
 */

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with 8 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmRead_8Bit(uint16_t NVMVariable, uint8_t *data)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with 16 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmRead_16Bit(uint16_t NVMVariable, NVMType16 *data)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with 32 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmRead_32Bit(uint16_t NVMVariable, NVMType32 *data)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with float variables
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
result_t nvmRead_float(uint16_t NVMVariable, float *data)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * ---------------------------------------------------------
 * 					      MEMORY CONTROL FUNCTIONS
 * ---------------------------------------------------------
 */

/**
 * @brief  This function returns a buffer with the data
 *         of the selected memory region
 * @note   This functions uses dynamic memory allocation
 *         so the memory needs to be deallocated once the data is
 *				 not needed
 *         TODO: This function may call a memory pool
 *         TODO: Possible static memory allocation
 * @param  startAddr: Start Address of the memory dump
 * @param  endAddr: End Address of the memory dump
 * @retval uint8_t* : Pointer of the memory allocated (TODO: Possible use of void * pointer)
 */
uint8_t *nvmRegionDump(uint16_t startAddr, uint16_t endAddr, uint32_t timeout)
{
	osStatus result;
	uint8_t *buffPointer = NULL;

	if(EEPROM_SIZE > endAddr)
	{
		/*Inside the address bounds, all good*/
		result = osSemaphoreAcquire(xSemaphore_MemoryPoolUsedHandle, timeout);

		if(osOK == result)
		{
			/* The memory pool is free */
			buffPointer = (uint8_t *)osMemoryPoolAlloc(MemoryPoolNVM, timeout);
			if(NULL != buffPointer)
			{
				/* All good, can fill the memory pool with EEPROM data */
				if(OK != transferEEPROMData(startAddr, endAddr, buffPointer, timeout))
				{
					/* Errors during transfer, all DTCs handled */
					osMemoryPoolFree(MemoryPoolNVM, buffPointer);
					buffPointer = NULL;
				}
				else
				{
					/* Transfer successful, Do nothing*/
				}
			}
			else
			{
				/* Cannot allocate the memory */
				/* TODO: Turn on DTC Not enough memory */
			}
		}
		else
		{
			/* The memory pool is being used */
			buffPointer = NULL;
		}
	}
	else
	{
		/*Error, endAddr cannot be higher than EEPROM_SIZE*/
		buffPointer = NULL;
	}

	return buffPointer;
}

/**
 * @brief  This function returns a pointer to the buffer with
 *         the data of the hole memory
 * @note   This function allocates dynamic memory so
 *         once the buffer is no longer used the memory
 *         needs to be deallocated
 *
 * 				 when used this function the returned pointer
 * 				 needs to be casted to the main type
 *         TODO: This function may call a memory pool
 *         TODO: Possible static memory allocation
 * @retval uint8_t *: Pointer to the buffer
 */
uint8_t *nvmMemoryDump(void)
{
	SendUSB("Sample"); /*TODO: Stubbed Code to avoid warnings*/
	return NULL; /*TODO: Stubbed code*/
}

/**
 * @brief  This function deallocates the memory pool in order to let another
 *         function or task of the system to use the memory pool
 * @param  blockPointer: Pointer to the block allocated of the memory pool
 * @retval result: the result of the operation
 */
result_t NVM_freeMemoryPool(uint8_t *blockPointer)
{
	result_t result;

	if(osOK == osMemoryPoolFree(MemoryPoolNVM, blockPointer))
	{
		/* The operation is successful */
		result = osSemaphoreRelease(xSemaphore_MemoryPoolUsedHandle);
	}
	else
	{
		/* The memory pool couln't be deallocated */
		/* TODO: Trigger DTC, software error line 248 */
		result = Error;
	}
	return result;
}

/**
 * @brief  Fills the hole EEPROM memory with zeroes
 * @retval result: the result of the operation
 */
result_t nvmClear(void)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Loads the default values on the EEPROM memory
 * @retval the result of the operation
 */
result_t nvmLoadDefaultValues(void)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */

/**
 * @brief  This function implements logic for the DMA transfer of the EEPROM
 *         data given a pointer to save all the desired data, it considers all
 *         the timeout cases and it's multi-task saves
 * @param  startAddr: Start Address of the EEPROM memory dump
 * @param  endAddr: End Address of the EEPROM memory dump
 * @param  memoryPool: Pointer to the memory pool base address (location to save)
 * @param  timeout: Timeout for the function in general and the OS function calls
 * @retval the result of the operation
 */
static result_t transferEEPROMData(uint16_t startAddr, uint16_t endAddr, uint8_t *memoryPool, uint32_t timeout)
{
	result_t result;
	uint16_t transferSize = startAddr - endAddr;
	uint32_t tickTime = osKernelGetTickCount();
	HAL_StatusTypeDef dmaResult;
	if(osWaitForever != timeout)
	{
		/* Calculating tick for exit */
		timeout += tickTime;
	}
	else
	{
		/* Do Nothing */
	}

	do {
		/* Starting the transfer */
		dmaResult = HAL_I2C_Mem_Read_DMA(&hi2c2, EEPROM_ADDR, startAddr, 1, memoryPool, transferSize);

		if(osWaitForever != timeout)
		{
			/* There is a timeout configured */
			tickTime = osKernelGetTickCount();
			if(tickTime > timeout)
			{
				/* Reached timeout, exit the function */
				dmaResult = HAL_TIMEOUT;
			}
			else
			{
				/* Do Nothing */
			}
		}
		else
		{
			/* Do Nothing */
		}
	}while(HAL_BUSY == dmaResult);

	switch(dmaResult)
	{
		case HAL_OK:
			/* All good, waiting data to be transfered */
			result = osSemaphoreAcquire(xSemaphore_DMA_TransferCpltHandle, timeout);
			if(result < (result_t)osErrorTimeout)
			{
				/*The OS returned a non compatible value*/
				result = Error;
			}
			else
			{
				/* Do Nothing */
			}
		break;
		case HAL_ERROR:
			/* TODO: Trigger Visual DTC */
			result = Error;
		break;
		case HAL_TIMEOUT:
			/* Timeout detected, exit the function */
			result = Timeout;
		break;
		default:
			/* Do Nothing */
		break;
	}

	return result;
}

static result_t SendUSB(char *format, ...) /*TODO: Instrumented code*/
{
	result_t retval = OK;
	char buffer[1024];
	va_list args;

	va_start(args, format);
	/*Filling buffer with zeroes*/
	memset(buffer, 0, sizeof(buffer));
	/*Filling the buffer with variadic args*/
	retval = vsprintf(buffer, format, args);
	if(USBD_OK == CDC_getReady() && OK == retval)
	{
		/*The USB is ready to transmit and the buffer is filled*/
		CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer));
	}
	else
	{
		/*The data cannot be transmitted*/
		retval = Error;
	}
	va_end(args);
	return retval;
}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT CALLBACKS
 * ---------------------------------------------------------
 */

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	osSemaphoreRelease(xSemaphore_DMA_TransferCpltHandle);
}
