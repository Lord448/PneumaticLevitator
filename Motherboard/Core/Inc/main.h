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
#define ABS(x) x > 0 ? x:-x
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_OR_Pin GPIO_PIN_13
#define LED_OR_GPIO_Port GPIOC
#define DevMode_IT_Pin GPIO_PIN_0
#define DevMode_IT_GPIO_Port GPIOA
#define GPLed1_Pin GPIO_PIN_3
#define GPLed1_GPIO_Port GPIOA
#define GPLed2_Pin GPIO_PIN_4
#define GPLed2_GPIO_Port GPIOA
#define WP_Pin GPIO_PIN_7
#define WP_GPIO_Port GPIOA
#define PWM_FAN_Pin GPIO_PIN_4
#define PWM_FAN_GPIO_Port GPIOB
#define EnableFAN_Pin GPIO_PIN_5
#define EnableFAN_GPIO_Port GPIOB
#define RPMSpeed_IC_Pin GPIO_PIN_6
#define RPMSpeed_IC_GPIO_Port GPIOB
#define TOF_XSHUT_Pin GPIO_PIN_7
#define TOF_XSHUT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
