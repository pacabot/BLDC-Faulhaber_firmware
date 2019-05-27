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
#include "stm32h7xx_hal.h"

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
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define USER_Btn_EXTI_IRQn EXTI15_10_IRQn
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define ADC_Bemf_CH1ADC_Bemf_CH1_Pin GPIO_PIN_3
#define ADC_Bemf_CH1ADC_Bemf_CH1_GPIO_Port GPIOC
#define ADC_CH_1_CURRENT_Pin GPIO_PIN_0
#define ADC_CH_1_CURRENT_GPIO_Port GPIOA
#define ADC_CH_2_SPEED_Pin GPIO_PIN_1
#define ADC_CH_2_SPEED_GPIO_Port GPIOA
#define ADC_Bemf_CH3_Pin GPIO_PIN_7
#define ADC_Bemf_CH3_GPIO_Port GPIOA
#define ADC_Bemf_CH2_Pin GPIO_PIN_0
#define ADC_Bemf_CH2_GPIO_Port GPIOB
#define HALL_3_Pin GPIO_PIN_10
#define HALL_3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define HALL_1_Pin GPIO_PIN_15
#define HALL_1_GPIO_Port GPIOA
#define GPIO_CH_COMM_Pin GPIO_PIN_10
#define GPIO_CH_COMM_GPIO_Port GPIOC
#define GPIO_CH_ZCR_Pin GPIO_PIN_12
#define GPIO_CH_ZCR_GPIO_Port GPIOC
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define HALL_2_Pin GPIO_PIN_3
#define HALL_2_GPIO_Port GPIOB
#define FAULT_PIN_Pin GPIO_PIN_4
#define FAULT_PIN_GPIO_Port GPIOB
#define EN_PIN_Pin GPIO_PIN_5
#define EN_PIN_GPIO_Port GPIOB
#define RESET_PN_Pin GPIO_PIN_6
#define RESET_PN_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
