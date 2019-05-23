/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"

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
#define BLUE_BUTTON_Pin LL_GPIO_PIN_13
#define BLUE_BUTTON_GPIO_Port GPIOC
#define BLUE_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define ADC_CH_4_TEMP_Pin LL_GPIO_PIN_1
#define ADC_CH_4_TEMP_GPIO_Port GPIOC
#define ADC_CH_2_VBUS_Pin LL_GPIO_PIN_2
#define ADC_CH_2_VBUS_GPIO_Port GPIOC
#define ADC_Bemf_CH1_Pin LL_GPIO_PIN_3
#define ADC_Bemf_CH1_GPIO_Port GPIOC
#define ADC_CH_1_CURRENT_Pin LL_GPIO_PIN_0
#define ADC_CH_1_CURRENT_GPIO_Port GPIOA
#define ADC_CH_2_SPEED_Pin LL_GPIO_PIN_1
#define ADC_CH_2_SPEED_GPIO_Port GPIOA
#define USART_TX_Pin LL_GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin LL_GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin LL_GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ADC_Bemf_CH3_Pin LL_GPIO_PIN_7
#define ADC_Bemf_CH3_GPIO_Port GPIOA
#define ADC_Bemf_CH2_Pin LL_GPIO_PIN_0
#define ADC_Bemf_CH2_GPIO_Port GPIOB
#define RESET_PIN_Pin LL_GPIO_PIN_7
#define RESET_PIN_GPIO_Port GPIOC
#define TMS_Pin LL_GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin LL_GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define GPIO_CH_COMM_Pin LL_GPIO_PIN_10
#define GPIO_CH_COMM_GPIO_Port GPIOC
#define GPIO_CH_ZCR_Pin LL_GPIO_PIN_12
#define GPIO_CH_ZCR_GPIO_Port GPIOC
#define EN_PIN_Pin LL_GPIO_PIN_4
#define EN_PIN_GPIO_Port GPIOB
#define FAULT_PIN_Pin LL_GPIO_PIN_5
#define FAULT_PIN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
