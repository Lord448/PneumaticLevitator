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
void nvmSave_8Bit(uint16_t NVMVariable, uint8_t value)
{

}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with 16 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmSave_16Bit(uint16_t NVMVariable, NVMType16 value)
{

}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with 32 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmSave_32Bit(uint16_t NVMVariable, NVMType32 value)
{

}

/**
 * @brief  Saves the selected variable with the indicated
 *         value on the EEPROM with float data
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmSave_float(uint16_t NVMVariable, float value)
{

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
void nvmRead_8Bit(uint16_t NVMVariable, uint8_t *data)
{

}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with 16 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmRead_16Bit(uint16_t NVMVariable, NVMType16 *data)
{

}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with 32 bits
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmRead_32Bit(uint16_t NVMVariable, NVMType32 *data)
{

}

/**
 * @brief  Reads the selected variable with the indicated
 *         value on the EEPROM with float variables
 * @param  NVMVariable: The variable that will be saved
 * @param  value: value of the variable
 * @retval result of the operation
 */
void nvmRead_float(uint16_t NVMVariable, float *data)
{

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
	return NULL; /*Stubbed code*/
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
	return NULL; /*Stubbed code*/
}

/**
 * @brief  Fills the hole EEPROM memory with zeroes
 * @retval result: the result of the operation
 */
void nvmClear(void)
{

}

/**
 * @brief  Loads the default values on the EEPROM memory
 * @retval the result of the operation
 */
void nvmLoadDefaultValues(void)
{

}

/**
 * ---------------------------------------------------------
 * 					 SOFTWARE COMPONENT LOCAL FUNCTIONS
 * ---------------------------------------------------------
 */

