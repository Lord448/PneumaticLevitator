/**
 * @file      EEPROMTypes.c
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

#include "EEPROMTypes.h"

extern I2C_HandleTypeDef hi2c2;

HAL_StatusTypeDef EEPROM_ReadUint8(Euint8 *data)
{
	return HAL_I2C_Mem_Read(&hi2c2, EEPROM_ADDR, data->Address, 1, &data->Value, 1, 100);
}
HAL_StatusTypeDef EEPROM_WriteUint8(Euint8 *data)
{
	return HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, data->Address, 1, &data->Value, 1, 100);
}
HAL_StatusTypeDef EEPROM_ReadInt32(Eint32 *data)
{
	return HAL_I2C_Mem_Read(&hi2c2, EEPROM_ADDR, data->BaseAddress, 1, (uint8_t*)&data->Value, 1, 100);
}
HAL_StatusTypeDef EEPROM_WriteInt32(Eint32 *data)
{
	return HAL_I2C_Mem_Write(&hi2c2, EEPROM_ADDR, data->BaseAddress, 1, (uint8_t*)&data->Value, 1, 100);
}
HAL_StatusTypeDef EEPROM_ReadFloat(Efloat *data)
{
	return HAL_I2C_Mem_Read_DMA(&hi2c2, EEPROM_ADDR, data->BaseAddress, 1, (uint8_t*)&data->Value, sizeof(float));
}
HAL_StatusTypeDef EEPROM_WriteFloat(Efloat *data)
{
	return HAL_I2C_Mem_Write_DMA(&hi2c2, EEPROM_ADDR, data	->BaseAddress, 1, (uint8_t*)&data->Value, sizeof(float));
}
