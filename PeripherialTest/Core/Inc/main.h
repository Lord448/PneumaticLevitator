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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ProgramRunning_Pin GPIO_PIN_13
#define ProgramRunning_GPIO_Port GPIOC
#define DevMode_IT_Pin GPIO_PIN_0
#define DevMode_IT_GPIO_Port GPIOA
#define DevMode_IT_EXTI_IRQn EXTI0_IRQn
#define EncoderSW_Pin GPIO_PIN_1
#define EncoderSW_GPIO_Port GPIOA
#define Left_Pin GPIO_PIN_2
#define Left_GPIO_Port GPIOA
#define Right_Pin GPIO_PIN_3
#define Right_GPIO_Port GPIOA
#define Down_Pin GPIO_PIN_4
#define Down_GPIO_Port GPIOA
#define Up_Pin GPIO_PIN_5
#define Up_GPIO_Port GPIOA
#define Ok_Pin GPIO_PIN_6
#define Ok_GPIO_Port GPIOA
#define Reset_IT_Pin GPIO_PIN_7
#define Reset_IT_GPIO_Port GPIOA
#define Reset_IT_EXTI_IRQn EXTI9_5_IRQn
#define AT24C04_WP_Pin GPIO_PIN_0
#define AT24C04_WP_GPIO_Port GPIOB
#define Menu_IT_Pin GPIO_PIN_1
#define Menu_IT_GPIO_Port GPIOB
#define Menu_IT_EXTI_IRQn EXTI1_IRQn
#define LCD_RST_Pin GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_14
#define LCD_DC_GPIO_Port GPIOB
#define SDO_Pin GPIO_PIN_15
#define SDO_GPIO_Port GPIOB
#define EncoderA_Pin GPIO_PIN_8
#define EncoderA_GPIO_Port GPIOA
#define EncoderB_Pin GPIO_PIN_9
#define EncoderB_GPIO_Port GPIOA
#define TOF_XSHUT_Pin GPIO_PIN_10
#define TOF_XSHUT_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_15
#define LCD_CS_GPIO_Port GPIOA
#define PWM_FAN_Pin GPIO_PIN_4
#define PWM_FAN_GPIO_Port GPIOB
#define EnableFAN_Pin GPIO_PIN_5
#define EnableFAN_GPIO_Port GPIOB
#define RPMSpeed_IC_Pin GPIO_PIN_6
#define RPMSpeed_IC_GPIO_Port GPIOB
#define LCD_BLK_Pin GPIO_PIN_7
#define LCD_BLK_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
