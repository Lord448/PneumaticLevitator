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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define Down_IT_Pin GPIO_PIN_15
#define Down_IT_GPIO_Port GPIOC
#define Down_IT_EXTI_IRQn EXTI15_10_IRQn
#define Left_IT_Pin GPIO_PIN_0
#define Left_IT_GPIO_Port GPIOA
#define Left_IT_EXTI_IRQn EXTI0_IRQn
#define Ok_IT_Pin GPIO_PIN_1
#define Ok_IT_GPIO_Port GPIOA
#define Ok_IT_EXTI_IRQn EXTI1_IRQn
#define Right_IT_Pin GPIO_PIN_2
#define Right_IT_GPIO_Port GPIOA
#define Right_IT_EXTI_IRQn EXTI2_IRQn
#define Up_IT_Pin GPIO_PIN_3
#define Up_IT_GPIO_Port GPIOA
#define Up_IT_EXTI_IRQn EXTI3_IRQn
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
#define EncoderA_Pin GPIO_PIN_8
#define EncoderA_GPIO_Port GPIOA
#define EncoderB_Pin GPIO_PIN_9
#define EncoderB_GPIO_Port GPIOA
#define EncoderSW_Pin GPIO_PIN_10
#define EncoderSW_GPIO_Port GPIOA
#define EncoderSW_EXTI_IRQn EXTI15_10_IRQn
#define Menu_IT_Pin GPIO_PIN_11
#define Menu_IT_GPIO_Port GPIOA
#define Menu_IT_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
