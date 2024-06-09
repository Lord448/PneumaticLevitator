/*
 * EEPROMTypes.c
 *
 *  Created on: Dec 6, 2023
 *      Author: lord448
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
