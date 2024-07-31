/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define GPIOProx10_Pin GPIO_PIN_0
#define GPIOProx10_GPIO_Port GPIOC
#define GPIOProx9_Pin GPIO_PIN_1
#define GPIOProx9_GPIO_Port GPIOC
#define GPIOProx1_Pin GPIO_PIN_3
#define GPIOProx1_GPIO_Port GPIOC
#define GPIOProx5_Pin GPIO_PIN_0
#define GPIOProx5_GPIO_Port GPIOA
#define GPIOProx6_Pin GPIO_PIN_1
#define GPIOProx6_GPIO_Port GPIOA
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define GPIOProx7_Pin GPIO_PIN_4
#define GPIOProx7_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define GPIO_InPUT12_Pin GPIO_PIN_4
#define GPIO_InPUT12_GPIO_Port GPIOC
#define SWin1_Pin GPIO_PIN_5
#define SWin1_GPIO_Port GPIOC
#define GPIOProx8_Pin GPIO_PIN_0
#define GPIOProx8_GPIO_Port GPIOB
#define GPIO_INPUT10_Pin GPIO_PIN_10
#define GPIO_INPUT10_GPIO_Port GPIOB
#define SWin2_Pin GPIO_PIN_11
#define SWin2_GPIO_Port GPIOB
#define GPIO_O11_Pin GPIO_PIN_12
#define GPIO_O11_GPIO_Port GPIOB
#define GPIO_O10MBBB_Pin GPIO_PIN_8
#define GPIO_O10MBBB_GPIO_Port GPIOC
#define GPIO_O4M4B_Pin GPIO_PIN_9
#define GPIO_O4M4B_GPIO_Port GPIOC
#define GPIO_O7M2_Pin GPIO_PIN_8
#define GPIO_O7M2_GPIO_Port GPIOA
#define GPIO_O6M1_Pin GPIO_PIN_9
#define GPIO_O6M1_GPIO_Port GPIOA
#define GPIO_O8M2_Pin GPIO_PIN_10
#define GPIO_O8M2_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define GPIOProx3_Pin GPIO_PIN_15
#define GPIOProx3_GPIO_Port GPIOA
#define GPIO_O1M4_Pin GPIO_PIN_10
#define GPIO_O1M4_GPIO_Port GPIOC
#define GPIO_O9M3_Pin GPIO_PIN_11
#define GPIO_O9M3_GPIO_Port GPIOC
#define GPIO_O2M4_Pin GPIO_PIN_12
#define GPIO_O2M4_GPIO_Port GPIOC
#define GPIO_O12M3_Pin GPIO_PIN_2
#define GPIO_O12M3_GPIO_Port GPIOD
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define GPIO_O5M1_Pin GPIO_PIN_6
#define GPIO_O5M1_GPIO_Port GPIOB
#define GPIO_O3_Pin GPIO_PIN_9
#define GPIO_O3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
