/**
 * @file      DiagAppl.h
 * @author    Pedro Rojo (pedroeroca@outlook.com)
 *
 * @brief     TODO
 *
 * @date      Jun 2, 2024
 *
 * @license   This Source Code Form is subject to the terms of
 *            the Mozilla Public License, v. 2.0. If a copy of
 *            the MPL was not distributed with this file, You
 *            can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * @copyright Copyright (c) 2024
 */

#ifndef SWC_DIAGAPPL_DIAGAPPL_H_
#define SWC_DIAGAPPL_DIAGAPPL_H_

#include "main.h"
#include "DIDList.h"
#include "DTCList.h"

/*
 	 	 	 	 	        GENERAL DIAGNOSTICS FRAME
 	 	 	 	 	 	 	 	 	     (BASED ON  UDS)
  |------------------|----------------|---------------|
  |	       PCI       |       SID      |       RPD     |
  | Protocol Control |  Service Data  |  Request Data |
  |    Information   |   Identifier   |   Parameters  |
  |      (0xAA)      |                |               |
	|------------------|----------------|---------------|
*/

/*
 *-----------------------------------------
 *        UDS SERVICES IDENTIFIERS
 *-----------------------------------------
 */
#define DIAGNOSTICS_PCI                         0xAA /* ID for the diagnostics frame */
#define NULL_SERVICE														0x00 /**/
/*Diagnostic and Communication management*/
#define SERVICE_SESSION_CONTROL_REQUEST         0x10
#define SERVICE_SESSION_COTROL_RESPONSE         0x50
#define SERVICE_ECU_RESET_REQUEST               0x11
#define SERVICE_ECU_RESET_RESPONSE              0x51
#define SERVICE_COMMUNICATION_CONTROL_REQUEST   0x28
#define SERVICE_COMMUNICATION_CONTROL_RESPONSE  0x68
/*Data Transmission (Only valid for NVM data)*/
#define SERVICE_READ_DATA_BY_ID_REQUEST         0x22
#define SERVICE_READ_DATA_BY_ID_RESPONSE        0x62
#define SERVICE_READ_MEMORY_BY_ADDRESS_REQUEST  0x23
#define SERVICE_READ_MEMORY_BY_ADDRESS_RESPONSE 0x63
#define SERVICE_WRITE_DATA_BY_ID_REQUEST        0x2E
#define SERVICE_WRITE_DATA_BY_ID_RESPONSE       0x6E
#define SERVICE_WRITE_DATA_BY_ADDRESS_REQUEST   0x3D
#define SERVICE_WRITE_DATA_BY_ADDRESS_RESPONSE  0x7D
/*DTCs*/
#define SERVICE_CLEAR_DTCS_REQUEST              0x14
#define SERVICE_CLEAR_DTCS_RESPONSE             0x54
#define SERVICE_READ_DTC_INFORMATION_REQUEST    0x19
#define SERVICE_READ_DTC_INFORMATION_RESPONSE   0x59
/*Negative response*/
#define SERVICE_NEGATIVE_RESPONSE               0x7F
/*
 *-----------------------------------------
 *           PDU INDENTIFIERS
 *-----------------------------------------
 */
#define PCI_SINGLE_STREAM_32BIT    0x32
#define PCI_SINGLE_STREAM_64BIT    0x64
#define PCI_COMPOUND_STREAM_32BIT  0xAA
#define PCI_COMPOUND_STREAM_64BIT  0xBB
#define PCI_COMPOUND_END_STREAM    0xCC


void DiagAppl_Init(void);

void vTaskDiagAppl(void *argument);

#endif /* SWC_DIAGAPPL_DIAGAPPL_H_ */
