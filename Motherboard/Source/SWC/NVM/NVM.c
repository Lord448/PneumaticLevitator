/**
 * @file      NVM.c
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 * 						Hector Rojo (hectoraroca@outlook.com)
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

static result_t transferEEPROMData(uint16_t startAddr, uint16_t endAddr, uint8_t *memoryPool, uint32_t timeout);
static result_t EEPROM_Write(uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t timeout);
static result_t EEPROM_Read(uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t timeout);

#ifdef MAKE_HARD_CODED_TEST
/* Instrumented code prototypes */
static void readDefaultValues(void);
#endif

/**
 * ---------------------------------------------------------
 * 					          INIT FUNCTION
 * ---------------------------------------------------------
 */
void NVM_Init(void)
{
	osSemaphoreRelease(xSemaphore_MemoryPoolUsedHandle);

#ifdef MAKE_HARD_CODED_TEST
	/* Hard coded test of the component */
	NVM_loadDefaultValues();
	readDefaultValues();
#endif
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
result_t NVM_save8Bit(uint16_t NVMVariable, uint8_t value)
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
result_t NVM_save16Bit(uint16_t NVMVariable, NVMType16 value)
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
result_t NVM_save32Bit(uint16_t NVMVariable, NVMType32 value)
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
result_t NVM_saveFloat(uint16_t NVMVariable, float value)
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
result_t NVM_read8Bit(uint16_t NVMVariable, uint8_t *data)
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
result_t NVM_read16Bit(uint16_t NVMVariable, NVMType16 *data)
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
result_t NVM_read32Bit(uint16_t NVMVariable, NVMType32 *data)
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
result_t NVM_readFloat(uint16_t NVMVariable, float *data)
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
uint8_t *NVM_regionDump(uint16_t startAddr, uint16_t endAddr, uint32_t timeout)
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
 *         the data of the whole memory
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
uint8_t *NVM_memoryDump(uint32_t timeout)
{
	return NVM_regionDump(EEPROM_INIT_ADDR, EEPROM_END_ADDR, timeout);
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
result_t NVM_clear(void)
{
	return OK; /*TODO: Stubbed code*/
}

/**
 * @brief  Loads the default values on the EEPROM memory
 * @retval the result of the operation
 */
result_t NVM_loadDefaultValues(void)
{
	/*TODO:In case of creating a new doc NVMVariables.c move the variables*/
	/*Configurations region*/
	const bool FabricConfigDefaultVal = true;
	const ActionMode ModeDefaultVal = AutoPID;
	/*PID region*/
	const NVMType32 KpDefaultVal = {.dataFloat = 0};
	const NVMType32 KiDefaultVal = {.dataFloat = 0};
	const NVMType32 KdDefaultVal = {.dataFloat = 0};
	const NVMType32 PlimitDefaultVal = {.data32 = 0};
	const NVMType32 IlimitDefaultVal = {.data32 = 0};
	const NVMType32 DlimitDefaultVal = {.data32 = 0};
	const NVMType32 SetpointDefaultVal = {.data32 = 250};
	/*Diagnostics Motherboard region*/


	/*Configurations variables*/
	EEPROM_Write(FABRIC_CONFIG_BASE_ADDR, (uint8_t *)&FabricConfigDefaultVal, sizeof(uint8_t), 100);
	EEPROM_Write(MODE_CONFIG_BASE_ADDR, (uint8_t *)&ModeDefaultVal, sizeof(uint8_t), 100);
	/*PID variables*/
	EEPROM_Write(KP_PID_BASE_ADDR, (uint8_t *)KpDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(KI_PID_BASE_ADDR, (uint8_t *)KiDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(KD_PID_BASE_ADDR, (uint8_t *)KdDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(PLIMIT_PID_BASE_ADDR, (uint8_t *)PlimitDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(ILIMIT_PID_BASE_ADDR, (uint8_t *)IlimitDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(DLIMIT_PID_BASE_ADDR, (uint8_t *)DlimitDefaultVal.rawData, sizeof(NVMType32), 100);
	EEPROM_Write(SETPOINT_PID_BASE_ADDR, (uint8_t *)SetpointDefaultVal.rawData, sizeof(NVMType32), 100);
	/*Diagnostics Mother*/
	/*Diagnostics Daughter*/
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

static result_t EEPROM_Write(uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t timeout)
{
	result_t retval = OK;
	TickType_t limitTick;
	/* Calling the save of the function */
	HAL_StatusTypeDef result = HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, MemAddress, sizeof(uint8_t), pData, Size, timeout);

	switch(result)
	{
		case HAL_ERROR:
			/* TODO: Turn on fatal error flag (Medium level)*/
			retval = Error;
		break;
		case HAL_BUSY:
			/* Wait for the bus to be unlocked */
			limitTick = osKernelGetTickCount() + timeout;
			do {
				osDelay(10); /* Wait 10 ticks to check */
				if(osKernelGetTickCount() == limitTick)
				{
					/* Timeout reached exit the function */
					return Timeout;
				}
				else
				{
					/* Do Nothing */
				}
			}while(HAL_LOCKED == hi2c2.Lock);

			/* Making second attempt */
			return EEPROM_Write(MemAddress, pData, Size, timeout);
		break;
		case HAL_TIMEOUT:
			/* Exit from the function */
			retval = Timeout;
		break;
		case HAL_OK:
		default:
			/* Do Nothing */
		break;
	}
	return retval;
}

static result_t EEPROM_Read(uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t timeout)
{
	result_t retval = OK;
	TickType_t limitTick;
	/* Calling the save of the function */
	HAL_StatusTypeDef result = HAL_I2C_Mem_Read(&hi2c2, EEPROM_ADDR, MemAddress, sizeof(uint8_t), pData, Size, timeout);

	switch(result)
	{
		case HAL_ERROR:
			/* TODO: Turn on fatal error flag (Medium level)*/
			retval = Error;
		break;
		case HAL_BUSY:
			/* Wait for the bus to be unlocked */
			limitTick = osKernelGetTickCount() + timeout;
			do {
				osDelay(10); /* Wait 10 ticks to check */
				if(osKernelGetTickCount() == limitTick)
				{
					/* Timeout reached exit the function */
					return Timeout;
				}
				else
				{
					/* Do Nothing */
				}
			}while(HAL_LOCKED == hi2c2.Lock);

			/* Making second attempt */
			return EEPROM_Read(MemAddress, pData, Size, timeout);
		break;
		case HAL_TIMEOUT:
			/* Exit from the function */
			retval = Timeout;
		break;
		case HAL_OK:
		default:
			/* Do Nothing */
		break;
	}
	return retval;
}

/**
 * ---------------------------------------------------------
 * 					    SOFTWARE COMPONENT CALLBACKS
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

#ifdef MAKE_HARD_CODED_TEST

/**
 * ---------------------------------------------------------
 * 					    HARD CODED TESTING SOFTWARE
 * ---------------------------------------------------------
 */

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

static void readDefaultValues(void)
{
	bool FabricConfig = false;
	ActionMode mode = Slave;
	NVMType32 kp = {.dataFloat = 100}, ki = {.dataFloat = 150}, kd = {.dataFloat = 105};
	NVMType32 Plimit = {.data32 = 1}, Ilimit = {.data32 = 2}, Dlimit = {.data32 = 3};
	NVMType32 setpoint = {.data32= 255};
	EEPROM_Read(FABRIC_CONFIG_BASE_ADDR, (uint8_t *)&FabricConfig, sizeof(uint8_t), 100);
	EEPROM_Read(MODE_CONFIG_BASE_ADDR, (uint8_t *)&mode, sizeof(uint8_t), 100);
	EEPROM_Read(KP_PID_BASE_ADDR, (uint8_t *)kp.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(KI_PID_BASE_ADDR, (uint8_t *)ki.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(KD_PID_BASE_ADDR, (uint8_t *)kd.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(PLIMIT_PID_BASE_ADDR, (uint8_t *)Plimit.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(ILIMIT_PID_BASE_ADDR, (uint8_t *)Ilimit.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(DLIMIT_PID_BASE_ADDR, (uint8_t *)Dlimit.rawData, sizeof(NVMType32), 100);
	EEPROM_Read(SETPOINT_PID_BASE_ADDR, (uint8_t *)setpoint.rawData, sizeof(NVMType32), 100);
	SendUSB("FabricConfig: %d", FabricConfig);
	SendUSB("Mode: %d", mode);
	SendUSB("Kp = %d", kp.data32);
	SendUSB("Ki = %d", ki.data32);
	SendUSB("Kd = %d", kd.data32);
	SendUSB("Plimit = %d", Plimit.data32);
	SendUSB("Ilimit = %d", Ilimit.data32);
	SendUSB("Dlimit = %d", Dlimit.data32);
	SendUSB("Set point = %d", setpoint.data32);
}


#endif
