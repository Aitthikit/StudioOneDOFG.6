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
#define SWJoy_Pin GPIO_PIN_0
#define SWJoy_GPIO_Port GPIOC
#define Emergency_Read_Pin GPIO_PIN_1
#define Emergency_Read_GPIO_Port GPIOC
#define LAMP1_Pin GPIO_PIN_2
#define LAMP1_GPIO_Port GPIOC
#define LAMP2_Pin GPIO_PIN_3
#define LAMP2_GPIO_Port GPIOC
#define DirectionMotor_Pin GPIO_PIN_0
#define DirectionMotor_GPIO_Port GPIOA
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define LeadSW1_Pin GPIO_PIN_4
#define LeadSW1_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TIM3_Encoder_A_Pin GPIO_PIN_6
#define TIM3_Encoder_A_GPIO_Port GPIOA
#define TIM3_Encoder_B_Pin GPIO_PIN_7
#define TIM3_Encoder_B_GPIO_Port GPIOA
#define PULL_Pin GPIO_PIN_5
#define PULL_GPIO_Port GPIOC
#define LeadSW2_Pin GPIO_PIN_0
#define LeadSW2_GPIO_Port GPIOB
#define JOY_Y_Pin GPIO_PIN_1
#define JOY_Y_GPIO_Port GPIOB
#define Floor1_Pin GPIO_PIN_10
#define Floor1_GPIO_Port GPIOB
#define JOY_X_Pin GPIO_PIN_13
#define JOY_X_GPIO_Port GPIOB
#define PUSH_Pin GPIO_PIN_6
#define PUSH_GPIO_Port GPIOC
#define Floor4_Pin GPIO_PIN_7
#define Floor4_GPIO_Port GPIOC
#define Vacuum_Pin GPIO_PIN_8
#define Vacuum_GPIO_Port GPIOC
#define Floor2_Pin GPIO_PIN_8
#define Floor2_GPIO_Port GPIOA
#define Floor3_Pin GPIO_PIN_9
#define Floor3_GPIO_Port GPIOA
#define HomeButton_Pin GPIO_PIN_10
#define HomeButton_GPIO_Port GPIOA
#define TIM4_PWM_Pin GPIO_PIN_11
#define TIM4_PWM_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define Lowerprox_Pin GPIO_PIN_4
#define Lowerprox_GPIO_Port GPIOB
#define Lowerprox_EXTI_IRQn EXTI4_IRQn
#define UpperProx_Pin GPIO_PIN_5
#define UpperProx_GPIO_Port GPIOB
#define UpperProx_EXTI_IRQn EXTI9_5_IRQn
#define Floor5_Pin GPIO_PIN_6
#define Floor5_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
