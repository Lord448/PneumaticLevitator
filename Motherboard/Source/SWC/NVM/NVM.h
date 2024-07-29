/**
 * @file      NVM.h
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

#ifndef SWC_NVM_NVM_H_
#define SWC_NVM_NVM_H_

#include "main.h"
#include "cmsis_os.h"
#include "NVMVariables.h"

#define EEPROM_ADDR 0b1010000
#define EEPROM_SIZE 512
#define EEPROM_INIT_ADDR 0
#define EEPROM_END_ADDR  512

#define MEMORY_POOL_USED_FLAG 1
#define EEPROM_USED_FLAG      2

#define EEPROM_I2C_WRITE(MemAddress, pData, Size, Timeout) HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, MemAddress, sizeof(uint8_t), pData, Size, Timeout)

/**
 * @brief Union for the 32 bit data handling
 *        on the NVM and the HAL function
 */
typedef union NVMType32
{
	uint32_t data32;
	float    dataFloat;
	uint8_t  rawData[4];
}NVMType32;

/**
 * @brief Union for the 16 bit data handling
 *        on the NVM and the HAL function
 */
typedef union NVMType16
{
	uint16_t data16;
	uint8_t  rawData[2];
}NVMType16;

/**
 * ---------------------------------------------------------
 * 					 FUNCTION OVERLOADING FOR "nvmSave"
 * ---------------------------------------------------------
 */

/*nvmSave overload interface*/
#define NVM_save(variable, value) _Generic((value),					\
																	uint8_t   : NVM_save8Bit,  \
																	NVMType16 : NVM_save16Bit, \
																	NVMType32 : NVM_save32Bit, \
																	float     : NVM_saveFloat)(variable, value)

result_t NVM_save8Bit(uint16_t NVMVariable, uint8_t value);
result_t NVM_save16Bit(uint16_t NVMVariable, NVMType16 value);
result_t NVM_save32Bit(uint16_t NVMVariable, NVMType32 value);
result_t NVM_saveFloat(uint16_t NVMVariable, float value);

/**
 * ---------------------------------------------------------
 * 					 FUNCTION OVERLOADING FOR "nvmRead"
 * ---------------------------------------------------------
 */

/*nvmRead overload interface*/
#define NVM_read(variable, value) _Generic((value),					 \
																	uint8_t   *: NVM_read8Bit   \
																	NVMType16 *: NVM_read16Bit  \
																	NVMType32 *: NVM_read32Bit  \
																	float	  *: NVM_readFloat)(variable, value)

result_t NVM_read8Bit(uint16_t NVMVariable, uint8_t *data);
result_t NVM_read16Bit(uint16_t NVMVariable, NVMType16 *data);
result_t NVM_read32Bit(uint16_t NVMVariable, NVMType32 *data);
result_t NVM_readFloat(uint16_t NVMVariable, float *data);

/**
 * ---------------------------------------------------------
 * 					      MEMORY CONTROL FUNCTIONS
 * ---------------------------------------------------------
 */
void NVM_Init(void);
uint8_t *NVM_regionDump(uint16_t startAddr, uint16_t endAddr, uint32_t timeout);
uint8_t *NVM_memoryDump(uint32_t timeout);
result_t NVM_freeMemoryPool(uint8_t *blockPointer);
result_t NVM_clear(void);
result_t NVM_loadDefaultValues(void);

#endif /* SWC_NVM_NVM_H_ */
