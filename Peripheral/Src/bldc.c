/**
 ******************************************************************************
 * @file    TIM/TIM_6Steps/Src/main.c
 * @author  MCD Application Team
 * @brief   This example shows how to use TIM1 peripheral to generate 6 Steps.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"

#include "stdbool.h"

#include <bldc.h>

/* External variables --------------------------------------------------------*/
/* Timer handler declaration */
extern TIM_HandleTypeDef    htim1;
extern TIM_HandleTypeDef    htim2;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MOTOR_PWM_FREQ  		100000
#define MOTOR_PWM_RESOLUTION 	1000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile bool bldc_dir = 0;
volatile bool bldc_sync = 0;

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

static BLDC_BRIDGE_STATE BLDC_BRIDGE_STATE_SYNC_CW = // Motor step
{
		{ LL, LL, LL },  //  //000
		{ PN, LH, LL },
		{ PN, LL, LH },
		{ LL, PN, LH },
		{ LH, PN, LL },
		{ LH, LL, PN },
		{ LL, LH, PN },
		{ LL, LL, LL },  //  //111
};

static BLDC_BRIDGE_STATE BLDC_BRIDGE_STATE_SYNC_CCW =   // Motor step
{
		{ LL, LL, LL },  //  //000
		{ LH, PN, LL },
		{ LH, LL, PN },
		{ LL, LH, PN },
		{ PN, LH, LL },
		{ PN, LL, LH },
		{ LL, PN, LH },
		{ LL, LL, LL },  //  //111
};

/**
 * @brief  BLDC Init
 * @param  None
 * @retval None
 */
void BLDC_init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	/* Compute the prescaler value */
	uint32_t uwPrescalerValue = (uint32_t) ((HAL_RCC_GetPCLK2Freq() * 2) / (MOTOR_PWM_FREQ * MOTOR_PWM_RESOLUTION - 1));

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = uwPrescalerValue;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = MOTOR_PWM_RESOLUTION - 1;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = MOTOR_PWM_RESOLUTION - 1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;
	if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 18;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/*##-1- Configure the output channels ######################################*/
	/* Common configuration for all channels */
	sPWMConfig1.OCMode       = TIM_OCMODE_TIMING;
	sPWMConfig1.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sPWMConfig1.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sPWMConfig1.OCIdleState  = TIM_OCIDLESTATE_SET;
	sPWMConfig1.OCNIdleState = TIM_OCNIDLESTATE_SET;
	sPWMConfig1.OCFastMode   = TIM_OCFAST_DISABLE;

	/* Set the pulse value for channel 1 */
	sPWMConfig1.Pulse = 0;

	/* Set the pulse value for channel 2 */
	sPWMConfig2 = sPWMConfig1;
	sPWMConfig2.Pulse = sPWMConfig1.Pulse; //2047

	/* Set the pulse value for channel 3 */
	sPWMConfig3 = sPWMConfig1;
	sPWMConfig3.Pulse = sPWMConfig1.Pulse; //511

	HAL_GPIO_WritePin(EN_PIN_GPIO_Port, EN_PIN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RESET_PN_GPIO_Port, RESET_PN_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Set motor direction
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
 * @brief  Set motor synchronization mode
 * @param  Direction : SYNC or ASYNC
 * @retval None
 */
void BLDC_setSync(enum syncType sync)
{
	if (sync == SYNC)
		bldc_sync = 0;
	else if (sync == ASYNC)
		bldc_sync = 1;
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

	if(bldc_dir == CW)
	{
		if(bldc_sync == SYNC)
			bldc_bridge_state = &BLDC_BRIDGE_STATE_SYNC_CW;
		else
			bldc_bridge_state = &BLDC_BRIDGE_STATE_ASYNC_CW;;
	}
	else
	{
		if(bldc_sync == SYNC)
			bldc_bridge_state = &BLDC_BRIDGE_STATE_SYNC_CCW;
		else
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
