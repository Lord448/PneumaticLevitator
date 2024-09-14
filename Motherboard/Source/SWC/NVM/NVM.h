/**
 * @file      NVM.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *            Hector Rojo (hectoraroca@outlook.com)
 *
 * @brief     This software component works as an abstraction
 *            layer for the usage of the EEPROM, implementing
 *            overloaded function for all the different types
 *            that can be saved on the EEPROM assuming each
 *            address of the memory can save 1 byte.
 *
 *            For the memory dump function that works with the
 *            DiagAppl features and the status of the system
 *            component managers, are implemented a heap
 *            dynamic memory allocation called memory pools
 *
 *            TODO: This component and features are disabled on
 *            the first versions of the project
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
#include "ModeManager.h"
#include "PID.h"

#define MAKE_HARD_CODED_TEST

#ifdef MAKE_HARD_CODED_TEST
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usbd_cdc_if.h"
#endif


/**
 * ---------------------------------------------------------
 * 					         NVM GENERAL SYMBOLS
 * ---------------------------------------------------------
 */
#define EEPROM_ADDR 0xA0
#define EEPROM_SIZE 512
#define EEPROM_INIT_ADDR 0
#define EEPROM_END_ADDR  512

#define MEMORY_POOL_USED_FLAG 1
#define EEPROM_USED_FLAG      2

#define Enable_EEPROM() HAL_GPIO_WritePin(WP_GPIO_Port, WP_Pin, 0)
#define Disable_EEPROM() HAL_GPIO_WritePin(WP_GPIO_Port, WP_Pin, 1)

//#define ENABLE_EEPROM

/**
 * ---------------------------------------------------------
 * 					          NVM GENERAL TYPES
 * ---------------------------------------------------------
 */
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
 * 					 FUNCTION OVERLOADING FOR "NVM_Save"
 * ---------------------------------------------------------
 */

/*nvmSave overload interface*/
#define NVM_Save(variable, value) _Generic((value),					 \
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
 * 					 FUNCTION OVERLOADING FOR "NVM_Read"
 * ---------------------------------------------------------
 */

/*nvmRead overload interface*/
#define NVM_Read(variable, data) _Generic((data),					    \
																	uint8_t*: NVM_read8Bit,     \
																	NVMType16*: NVM_read16Bit,  \
																	NVMType32*: NVM_read32Bit,  \
																	float*: NVM_readFloat)(variable, data)

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
