/**
 * @file      DiagAppl.c
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

#include "DiagAppl.h"

extern osMessageQueueId_t xFIFO_DiagsLongHandle;
extern osMessageQueueId_t xFIFO_DiagShortHandle;

void DiagAppl_Init(void)
{

}

void vTaskDiagAppl(void *argument)
{
	DiagAppl_Init();
	for(;;)
	{

	}
}

//void sendDiagnosticStream(uint32)
