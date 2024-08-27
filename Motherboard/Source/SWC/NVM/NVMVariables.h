/**
 * @file      NVMVariables.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 * 						Hector Rojo (hectoraroca@outlook.com)
 *
 * @brief     List of the variables of the NVM
 *            each symbol have with it the respective
 *            address of the variable in EEPROM
 *
 * @date      17 jul 2024
 *
 * @license   This Source Code Form is subject to the terms of 
 *            the Mozilla Public License, v. 2.0. If a copy of 
 *            the MPL was not distributed with this file, You 
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */
#ifndef SWC_NVM_NVMVARIABLES_H_
#define SWC_NVM_NVMVARIABLES_H_

#include "NVM.h"
#include "ModeManager.h"

/**
 * ---------------------------------------------------------
 * 					      CONFIGURATIONS REGION
 * ---------------------------------------------------------
 */

#define FABRIC_CONFIG_BASE_ADDR (uint16_t) 0x0000
#define MODE_CONFIG_BASE_ADDR   (uint16_t) 0x0001


/**
 * ---------------------------------------------------------
 * 					     			 PID REGION
 * ---------------------------------------------------------
 */

#define KP_PID_BASE_ADDR        (uint16_t) 0x000B
#define KI_PID_BASE_ADDR        (uint16_t) 0x000F
#define KD_PID_BASE_ADDR        (uint16_t) 0x0013
#define PLIMIT_PID_BASE_ADDR    (uint16_t) 0x0017
#define ILIMIT_PID_BASE_ADDR    (uint16_t) 0x001B
#define DLIMIT_PID_BASE_ADDR    (uint16_t) 0x001F
#define SETPOINT_PID_BASE_ADDR  (uint16_t) 0x0023

/* TODO: Add the sampling rate as a new variable */

/**
 * ---------------------------------------------------------
 * 		  			 DIAGNOSTICS MOTHERBOARD REGION
 * ---------------------------------------------------------
 */

/**
 * ---------------------------------------------------------
 * 		  			 DIAGNOSTICS DAUGHTER REGION
 * ---------------------------------------------------------
 */

#endif /* SWC_NVM_NVMVARIABLES_H_ */
