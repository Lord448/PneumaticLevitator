/**
 * @file      EEPROMTypes.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Dec 6, 2023
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#ifndef INC_EEPROMTYPES_H_
#define INC_EEPROMTYPES_H_

#include "main.h"

#define EEPROM_ADDR 0xA0

typedef union TwoByte{
	uint16_t data;
	uint8_t sendData[2];
};

typedef struct EEPROM_uint8
{
	uint32_t Address;
	uint8_t Value;
}Euint8;

typedef struct EEPROM_int8
{
	uint32_t Address;
	uint8_t Value;
}Eint8;

typedef struct EEPROM_uint16
{
	uint32_t Address;
	uint16_t Value;
}Euint16;

typedef struct EEPROM_int16
{
	uint32_t Address;
	int16_t Value;
}Eint16;

typedef struct EEPROM_uint32
{
	uint32_t BaseAddress;
	uint32_t Value;
}Euint32;

typedef struct EEPROM_int32
{
	uint32_t BaseAddress;
	int32_t Value;
}Eint32;

typedef struct EEPROM_float
{
	uint32_t BaseAddress;
	float Value;
}Efloat;

HAL_StatusTypeDef EEPROM_ReadUint8(Euint8 *data);
HAL_StatusTypeDef EEPROM_WriteUint8(Euint8 *data);
HAL_StatusTypeDef EEPROM_ReadInt32(Eint32 *data);
HAL_StatusTypeDef EEPROM_WriteInt32(Eint32 *data);
HAL_StatusTypeDef EEPROM_ReadFloat(Efloat *data);
HAL_StatusTypeDef EEPROM_WriteFloat(Efloat *data);

#endif /* INC_EEPROMTYPES_H_ */
