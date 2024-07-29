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

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL PROTOYPES
 * ---------------------------------------------------------
 */

static result_t SendUSB(char *format, ...); /*TODO Erase when COM it's finished*/

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
 * @param  startAddr: Start Address of the memory dumped
 * @param  endAddr: End Address of the memory dumped
 * @retval uint8_t* : Pointer of the memory allocated (TODO: Possible use of void * pointer)
 */
uint8_t *nvmRegionDump(uint16_t startAddr, uint16_t endAddr)
{
	return NULL; /*TODO: Stubbed code*/
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
