/*
 * aesm-4096.c
 *
 *  Created on: 24 avr. 2019
 *      Author: zhonx
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "spi.h"
#include "tim.h"

#include "bldc.h"
#include "aesm-4096.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
ALIGN_32BYTES(static uint32_t left_SSI_data);
static uint32_t left_encoder_value = 0;

/* Private function prototypes -----------------------------------------------*/
static int32_t aesm4096_InitEncoderTIM(uint32_t freq);

/* Private user code ---------------------------------------------------------*/

void aesm4096_Init(void)
{
	aesm4096_InitEncoderTIM(30000);
}

uint32_t aesm4096_getPosition(void)
{
	return left_encoder_value;
}

/**
 * @brief  Init and set encoder it frequency
 * @param  freq
 * @retval Error
 */
int32_t aesm4096_InitEncoderTIM(uint32_t freq)
{
	TIM_MasterConfigTypeDef sMasterConfig;
	uint32_t uwPrescalerValue = 0;

	/* Compute the prescaler value */
	uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / (freq * 10));

	/*##-1- Configure the TIM peripheral #######################################*/
	/* Time base configuration */
	htim6.Instance 				 = TIM6;
	htim6.Init.Period            = 10 - 1;
	htim6.Init.Prescaler         = uwPrescalerValue;
	htim6.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.CounterMode       = TIM_COUNTERMODE_UP;
	htim6.Init.RepetitionCounter = 0;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}

	/* TIM6 TRGO selection */
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/*##-2- Enable TIM peripheral counter ######################################*/
	if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim != &htim6)
		return;

	HAL_SPI_Receive_IT(&hspi1, (uint8_t*)&left_SSI_data, 1);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	uint16_t newcomm_pos = 0;
    static uint16_t comm_pos = 0;

	if (hspi != &hspi1)
		return;

	left_SSI_data >>= 8;
	left_SSI_data &= 0b00000111111111111;
    left_encoder_value = left_SSI_data;

    newcomm_pos = (left_encoder_value / 683) + 1;

    if (newcomm_pos == comm_pos) return;

    comm_pos = newcomm_pos;

//    printf("%d\n", (int)newcomm_pos);
    BLDCMotorPrepareCommutation(newcomm_pos);
}
