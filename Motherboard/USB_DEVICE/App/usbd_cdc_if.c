/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "cmsis_os.h"
#include "DiagAppl/DiagAppl.h"
#include "COM/COM.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
#define RTOS
#define PDU_CONTROL_SIZE 2 /* In bytes */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */
extern char ResBuffer[64];
extern uint8_t ReceiveFlag;
extern osMessageQueueId_t xFIFO_DiagShortHandle;
extern osMessageQueueId_t xFIFO_DiagsLongHandle;
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
	int8_t result = USBD_OK;
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
#ifndef RTOS
  memset(ResBuffer, '\0', 64); /* Clear the buffer */
  uint8_t len = (uint8_t)*Len;
  memcpy(ResBuffer, Buf, len); /* Copy the data to buffer */
  memset(Buf, '\0', len); /* Clear Buf */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
  ReceiveFlag = 1;
#else
  /*TODO: Instrumented code (handle all on COM)*/
  if(DIAGNOSTICS_PCI == Buf[0])
  {
  	/* It's a diagnostics frame */
  	uint32_t payloadSize = strlen((char *)Buf)-PDU_CONTROL_SIZE;

  	if(payloadSize > sizeof(uint32_t))
  	{
  		/* It's more than 32bits */
  		if(payloadSize > sizeof(uint64_t))
  		{
  			/* It's more than 64bits, need a compound PDU */
  			uint16_t index = 2; /*Saving the value before first for*/
  			uint64_t payloadBuf = 0;

				/* Fill the payload buffer*/
				for(uint16_t shift = 0; index < sizeof(uint64_t)+2; index++, shift+=8)
					payloadBuf |= Buf[index]<<shift; /* Data will be analyzed with memory pointer transposition */
				/*Build PDU*/
				DiagPDU_t diagPDU = {
						.fields.PDU_ID = PCI_COMPOUND_STREAM_64BIT,
						.fields.SID = Buf[1],
						.fields.payload = payloadBuf
				};
    		/*Sending the PDU to DiagAppl*/
    		result = osOK == osMessageQueuePut(xFIFO_DiagsLongHandle, &diagPDU, 0U, 0U) ?
    				USBD_OK : USBD_FAIL;

    		while(index < payloadSize && USBD_OK == result)
    		{
    			payloadBuf = 0;
  				/* Fill the payload buffer*/
  				for(uint16_t shift = 0; index < sizeof(uint64_t); index++, shift+=8)
  					payloadBuf |= Buf[index]<<shift; /* Data will be analyzed with memory pointer transposition */
  				/* Build PDU */
  				DiagPDU_t diagPDULoop = {
  						.fields.PDU_ID = NULL_SERVICE,
							.fields.SID = NULL_SERVICE,
							.fields.payload = payloadBuf
  				};
      		/* Send to the FIFO */
      		result = osOK == osMessageQueuePut(xFIFO_DiagsLongHandle, &diagPDULoop, 0U, 0U) ?
      				USBD_OK : USBD_FAIL;
    		}
  		}
  		else /* if(payloadSize < 8) bytes*/
  		{
  			/* Fits on a 64 bits PDU */
  			uint64_t payloadBuf = 0;
    		/* Fill the payload buffer*/
    		for(uint16_t index = 2, shift = 0; index < payloadSize; index++, shift+=8)
    			payloadBuf |= Buf[index]<<shift; /* Data will be analyzed with memory pointer transposition */
    		/*Build the PDU*/
    		DiagPDU_t diagPDU = {
    				.fields.PDU_ID = PCI_SINGLE_STREAM_64BIT,
						.fields.SID = Buf[1],
						.fields.payload = payloadBuf
    		};
    		/*Sending the PDU to DiagAppl*/
    		result = osOK == osMessageQueuePut(xFIFO_DiagsLongHandle, &diagPDU, 0U, 0U) ?
    				USBD_OK : USBD_FAIL;
  		}
  	}
  	else /* if(payload < 4) bytes */
  	{
  		/* Fits in a regular PDU */
  		uint32_t payloadBuf = 0;
  		/* Fill the payload buffer*/
  		for(uint16_t index = 2, shift = 0; index < payloadSize; index++, shift+=8)
  			payloadBuf |= Buf[index]<<shift; /* Data will be analyzed with memory pointer transposition */
  		/* Build the PDU */
  		PDU_t diagPDU = {
					.rawData[0] = PCI_SINGLE_STREAM_32BIT, /* Message ID */
					.rawData[1] = Buf[1],									 /* Type&Prior -> SID */
					.fields.payload = payloadBuf					 /* Payload */
			};
  		/*Sending the PDU to DiagAppl*/
  		result = osOK == osMessageQueuePut(xFIFO_DiagShortHandle, &diagPDU, 0U, 0U) ?
  				USBD_OK : USBD_FAIL;
  	}
  }
  else
  {
  	/*TODO Implement regular logic*/
  }
  /*Build the */
#endif
  return result;
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
uint8_t CDC_getReady(void)
{
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *) hUsbDeviceFS.pClassData;
	if(hcdc -> TxState != 0)
	{
		return USBD_BUSY;
	}
	else
	{
		return USBD_OK;
	}
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
