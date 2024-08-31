/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "cmsis_os2.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum bool {
	false,
	true
}bool;
typedef enum result {
	OK = osOK,        /* 0*/
	Error = osError,  /*-1*/
	Warning = 1
}result_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_GP_Pin GPIO_PIN_13
#define LED_GP_GPIO_Port GPIOC
#define BtnDown_Pin GPIO_PIN_15
#define BtnDown_GPIO_Port GPIOC
#define BtnLeft_Pin GPIO_PIN_0
#define BtnLeft_GPIO_Port GPIOA
#define BtnOK_Pin GPIO_PIN_1
#define BtnOK_GPIO_Port GPIOA
#define BtnRight_Pin GPIO_PIN_2
#define BtnRight_GPIO_Port GPIOA
#define BtnUP_Pin GPIO_PIN_3
#define BtnUP_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_5
#define LCD_SCK_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOA
#define LCD_SDO_Pin GPIO_PIN_7
#define LCD_SDO_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOB
#define LCD_BLK_Pin GPIO_PIN_1
#define LCD_BLK_GPIO_Port GPIOB
#define Reset_IT_Pin GPIO_PIN_12
#define Reset_IT_GPIO_Port GPIOB
#define Reset_IT_EXTI_IRQn EXTI15_10_IRQn
#define LED_COMM_Pin GPIO_PIN_13
#define LED_COMM_GPIO_Port GPIOB
#define LED_USB_Pin GPIO_PIN_14
#define LED_USB_GPIO_Port GPIOB
#define LED_CONTROL_Pin GPIO_PIN_15
#define LED_CONTROL_GPIO_Port GPIOB
#define EncoderA_Pin GPIO_PIN_8
#define EncoderA_GPIO_Port GPIOA
#define EncoderB_Pin GPIO_PIN_9
#define EncoderB_GPIO_Port GPIOA
#define EncoderSW_Pin GPIO_PIN_10
#define EncoderSW_GPIO_Port GPIOA
#define BtnMenu_Pin GPIO_PIN_11
#define BtnMenu_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define osNoTimeout 0U

#define FINISH_INIT_ID 0x1U

#define ALL_OK                             0U
#define FATAL_ERROR_MOTHER_COMM (uint32_t) 1U << 0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
