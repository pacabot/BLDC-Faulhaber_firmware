/**
 ******************************************************************************
 * @file    TIM/TIM_6Steps/Src/main.c
 * @author  MCD Application Team
 * @brief   This example shows how to use TIM1 peripheral to generate 6 Steps.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm32h7xx_ll_gpio.h"
#include "tim.h"
#include <bldc.h>

/* External variables --------------------------------------------------------*/
/* Timer handler declaration */
extern TIM_HandleTypeDef    htim1;
extern TIM_HandleTypeDef    htim2;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t bldc_dir = 0;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef              sPWMConfig1, sPWMConfig2, sPWMConfig3;
/* Timer Break Configuration Structure declaration */
TIM_BreakDeadTimeConfigTypeDef sBreakConfig;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

enum bldcState
{
	LL, LH, PL, PN
};

typedef uint8_t BLDC_BRIDGE_STATE[8][3];

static BLDC_BRIDGE_STATE BLDC_BRIDGE_STATE_ASYNC_CW = // Motor step
{
		{ LL, LL, LL },  //  //000
		{ PL, LH, LL },
		{ PL, LL, LH },
		{ LL, PL, LH },
		{ LH, PL, LL },
		{ LH, LL, PL },
		{ LL, LH, PL },
		{ LL, LL, LL },  //  //111
};

static BLDC_BRIDGE_STATE BLDC_BRIDGE_STATE_ASYNC_CCW =   // Motor step
{
		{ LL, LL, LL },  //  //000
		{ LH, PL, LL },
		{ LH, LL, PL },
		{ LL, LH, PL },
		{ PL, LH, LL },
		{ PL, LL, LH },
		{ LL, PL, LH },
		{ LL, LL, LL },  //  //111
};

/**
 * @brief  BLDC Init
 * @param  None
 * @retval None
 */
void BLDC_init(void)
{
	/*##-1- Configure the output channels ######################################*/
	/* Common configuration for all channels */
	sPWMConfig1.OCMode       = TIM_OCMODE_TIMING;
	sPWMConfig1.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sPWMConfig1.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sPWMConfig1.OCIdleState  = TIM_OCIDLESTATE_SET;
	sPWMConfig1.OCNIdleState = TIM_OCNIDLESTATE_SET;
	sPWMConfig1.OCFastMode   = TIM_OCFAST_DISABLE;

	/* Set the pulse value for channel 1 */
	sPWMConfig1.Pulse = 4095;

	/* Set the pulse value for channel 2 */
	sPWMConfig2 = sPWMConfig1;
	sPWMConfig2.Pulse = sPWMConfig1.Pulse; //2047

	/* Set the pulse value for channel 3 */
	sPWMConfig3 = sPWMConfig1;
	sPWMConfig3.Pulse = sPWMConfig1.Pulse; //511

	/*##-2- Configure the commutation event: software event ####################*/
	//    HAL_TIMEx_ConfigCommutationEvent_IT(&htim1, TIM_TS_ITR1, TIM_COMMUTATION_TRGI);

	HAL_TIMEx_HallSensor_Start(&htim2);

	HAL_GPIO_WritePin(EN_PIN_GPIO_Port, EN_PIN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RESET_PN_GPIO_Port, RESET_PN_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Commutation event callback in non blocking mode
 * @param  Direction : CW or CCW
 * @retval None
 */
void BLDC_setDir(enum rotationType dir)
{
	if (dir == CW)
		bldc_dir = 0;
	else if (dir == CCW)
		bldc_dir = 1;
}

/**
 * @brief  Commutation event callback in non blocking mode
 * @param  Duty cycle : 0 to 4096
 * @retval None
 */
void BLDC_setDutyCycle(uint16_t duty_cycle)
{
	/* Set the pulse value for channel 1 */
	sPWMConfig1.Pulse = duty_cycle;

	/* Set the pulse value for channel 2 */
	sPWMConfig2.Pulse = sPWMConfig1.Pulse;

	/* Set the pulse value for channel 3 */
	sPWMConfig3.Pulse = sPWMConfig1.Pulse;
}

__INLINE void BLDCMotorPrepareCommutation(uint16_t comm_pos)
{
	BLDC_BRIDGE_STATE *bldc_bridge_state;

	if(bldc_dir)
	{
		bldc_bridge_state = &BLDC_BRIDGE_STATE_ASYNC_CW;
	}
	else
	{
		bldc_bridge_state = &BLDC_BRIDGE_STATE_ASYNC_CCW;
	}

	// Bridge FETs for Motor Phase U
	switch ((*bldc_bridge_state)[comm_pos][2])
	{
	case LL :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_1);
		break;
	case LH :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
		// Low side FET: ON
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
		break;
	case PL :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_1);
		break;
	case PN :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		// Low side FET: NPWM
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
		break;
	}

	// Bridge FETs for Motor Phase V
	switch ((*bldc_bridge_state)[comm_pos][1])
	{
	case LL :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_2);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_2);
		break;
	case LH :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_2);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
		// Low side FET: ON
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
		break;
	case PL :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_2);
		break;
	case PN :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		// Low side FET: NPWM
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
		break;
	}

	// Bridge FETs for Motor Phase W
	switch ((*bldc_bridge_state)[comm_pos][0])
	{
	case LL :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_3);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_3);
		break;
	case LH :
		// High side FET: OFF
		sPWMConfig1.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_3);
		HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
		// Low side FET: ON
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
		break;
	case PL :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		// Low side FET: OFF
		HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_3);
		break;
	case PN :
		// High side FET: PWM
		sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
		HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		// Low side FET: NPWM
		HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
		break;
	}
}
