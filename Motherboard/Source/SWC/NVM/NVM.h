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

#define EEPROM_SIZE 512

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
#define nvmSave(variable, value) _Generic((value),					\
																	uint8_t  : nvmSave_8Bit,  \
																	NVMType16 : nvmSave_16Bit, \
																	NVMType32 : nvmSave_32Bit, \
																	float    : nvmSave_float)(variable, value)

void nvmSave_8Bit(uint16_t NVMVariable, uint8_t value);
void nvmSave_16Bit(uint16_t NVMVariable, NVMType16 value);
void nvmSave_32Bit(uint16_t NVMVariable, NVMType32 value);
void nvmSave_float(uint16_t NVMVariable, float value);

/**
 * ---------------------------------------------------------
 * 					 FUNCTION OVERLOADING FOR "nvmRead"
 * ---------------------------------------------------------
 */

/*nvmRead overload interface*/
#define nvmRead(variable, value) _Generic((value),					 \
																	uint8_t * : nvmRead_8Bit   \
																	NVMType16 *: nvmRead_16Bit  \
																	NVMType32 *: nvmRead_32Bit  \
																	float	*		: nvmRead_float)(variable, value)

void nvmRead_8Bit(uint16_t NVMVariable, uint8_t *data);
void nvmRead_16Bit(uint16_t NVMVariable, NVMType16 *data);
void nvmRead_32Bit(uint16_t NVMVariable, NVMType32 *data);
void nvmRead_float(uint16_t NVMVariable, float *data);

/**
 * ---------------------------------------------------------
 * 					      MEMORY CONTROL FUNCTIONS
 * ---------------------------------------------------------
 */
uint8_t *nvmRegionDump(uint16_t startAddr, uint16_t endAddr);
uint8_t *nvmMemoryDump(void);
void nvmClear(void);
void nvmLoadDefaultValues(void);

#endif /* SWC_NVM_NVM_H_ */
