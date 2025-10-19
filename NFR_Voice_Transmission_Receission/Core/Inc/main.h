/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define nrf_csn_Pin GPIO_PIN_1
#define nrf_csn_GPIO_Port GPIOA
#define nrf_sck_Pin GPIO_PIN_2
#define nrf_sck_GPIO_Port GPIOA
#define nrf_mosi_Pin GPIO_PIN_3
#define nrf_mosi_GPIO_Port GPIOA
#define nrf_miso_Pin GPIO_PIN_5
#define nrf_miso_GPIO_Port GPIOA
#define nrf_ce_Pin GPIO_PIN_6
#define nrf_ce_GPIO_Port GPIOA
#define oled_Pin GPIO_PIN_12
#define oled_GPIO_Port GPIOB
#define oledB13_Pin GPIO_PIN_13
#define oledB13_GPIO_Port GPIOB
#define state_key_Pin GPIO_PIN_5
#define state_key_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
