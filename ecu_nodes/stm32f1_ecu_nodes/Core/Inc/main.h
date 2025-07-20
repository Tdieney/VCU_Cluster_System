/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
#define DIGITAL_OUT_8_Pin GPIO_PIN_6
#define DIGITAL_OUT_8_GPIO_Port GPIOA
#define DIGITAL_OUT_12_Pin GPIO_PIN_7
#define DIGITAL_OUT_12_GPIO_Port GPIOA
#define DIGITAL_OUT_22_Pin GPIO_PIN_0
#define DIGITAL_OUT_22_GPIO_Port GPIOB
#define DIGITAL_OUT_32_Pin GPIO_PIN_1
#define DIGITAL_OUT_32_GPIO_Port GPIOB
#define BTN_COL0_Pin GPIO_PIN_12
#define BTN_COL0_GPIO_Port GPIOB
#define BTN_COL1_Pin GPIO_PIN_13
#define BTN_COL1_GPIO_Port GPIOB
#define BTN_COL2_Pin GPIO_PIN_14
#define BTN_COL2_GPIO_Port GPIOB
#define BTN_COL3_Pin GPIO_PIN_15
#define BTN_COL3_GPIO_Port GPIOB
#define DIGITAL_OUT_28_Pin GPIO_PIN_8
#define DIGITAL_OUT_28_GPIO_Port GPIOA
#define DIGITAL_OUT_19_Pin GPIO_PIN_9
#define DIGITAL_OUT_19_GPIO_Port GPIOA
#define DIGITAL_OUT_10_Pin GPIO_PIN_10
#define DIGITAL_OUT_10_GPIO_Port GPIOA
#define DIGITAL_OUT_1_Pin GPIO_PIN_11
#define DIGITAL_OUT_1_GPIO_Port GPIOA
#define BTN_ROW0_Pin GPIO_PIN_4
#define BTN_ROW0_GPIO_Port GPIOB
#define BTN_ROW0_EXTI_IRQn EXTI4_IRQn
#define BTN_ROW1_Pin GPIO_PIN_5
#define BTN_ROW1_GPIO_Port GPIOB
#define BTN_ROW1_EXTI_IRQn EXTI9_5_IRQn
#define BTN_ROW2_Pin GPIO_PIN_6
#define BTN_ROW2_GPIO_Port GPIOB
#define BTN_ROW2_EXTI_IRQn EXTI9_5_IRQn
#define BTN_ROW3_Pin GPIO_PIN_7
#define BTN_ROW3_GPIO_Port GPIOB
#define BTN_ROW3_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */
#define ZERO                    0U
#define ONE                     1U
#define TWO                     2U
#define THREE                   3U
#define FOUR                    4U

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
