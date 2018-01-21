/**
 ******************************************************************************
 * @file    TIM/TIM_6Steps/Src/main.c
 * @author  MCD Application Team
 * @brief   This example shows how to use TIM1 peripheral to generate 6 Steps.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
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
static __INLINE void BLDCMotorPrepareCommutation(void);

#define FALSE 0
#define TRUE  1

static const uint8_t BLDC_BRIDGE_STATE_BACKWARD[8][6] =   // Motor step
{
        { FALSE, FALSE,      FALSE, FALSE,      FALSE, FALSE },  //  //000
        { FALSE, FALSE,      TRUE , FALSE,      FALSE, TRUE  },
        { TRUE , FALSE,      FALSE, TRUE ,      FALSE, FALSE },
        { TRUE , FALSE,      FALSE, FALSE,      FALSE, TRUE  },
        { FALSE, TRUE ,      FALSE, FALSE,      TRUE , FALSE },
        { FALSE, TRUE ,      TRUE , FALSE,      FALSE, FALSE },
        { FALSE, FALSE,      FALSE, TRUE ,      TRUE , FALSE },
        { FALSE, FALSE,      FALSE, FALSE,      FALSE, FALSE },  //  //111
};

static const uint8_t BLDC_BRIDGE_STATE_FORWARD[8][6] =   // Motor step
{
        { FALSE, FALSE,     FALSE, FALSE,       FALSE, FALSE },  // 0 //000
        { FALSE, FALSE,     FALSE, TRUE ,       TRUE , FALSE },
        { FALSE, TRUE ,     TRUE , FALSE,       FALSE, FALSE },
        { FALSE, TRUE ,     FALSE, FALSE,       TRUE , FALSE },
        { TRUE , FALSE,     FALSE, FALSE,       FALSE, TRUE  },
        { TRUE , FALSE,     FALSE, TRUE ,       FALSE, FALSE },
        { FALSE, FALSE,     TRUE , FALSE,       FALSE, TRUE  },
        { FALSE, FALSE,     FALSE, FALSE,       FALSE, FALSE },  // 0 //111
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
    HAL_TIMEx_ConfigCommutationEvent_IT(&htim1, TIM_TS_ITR1, TIM_COMMUTATION_TRGI);

    HAL_TIMEx_HallSensor_Start(&htim2);
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

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//    if (htim == &htim2)
//    {
//        BLDCMotorPrepareCommutation();
//    }
//}

/**
 * @brief  This is called from HALL timer interrupt handler
   remember:
     if hall a hall edge is detected
     first the motor commutation event is done
     next this routine is called which has to prepare the next motor step
     (which FET must be switched on or off)
   active freewhelling is used to minimize power loss

 * @param  None
 * @retval None
 */
static __INLINE void BLDCMotorPrepareCommutation(void)
{
    // next bridge step calculated by HallSensor inputs
    // if there was an hall event without changing the hall position,
    // do nothing.
    //
    // In principle, on every hall event you can go to the next motor
    // step but i had sometimes problems that the motor was running
    // on an harmonic wave (??) when the motor was without load

    uint8_t BH1, BL1, BH2, BL2, BH3, BL3;
    uint16_t newcomm_pos =  ((LL_GPIO_ReadInputPort(GPIOA) & 0x8000) >> 13) |
            ((LL_GPIO_ReadInputPort(GPIOB) & 0x8) >> 2) |
            ((LL_GPIO_ReadInputPort(GPIOB) & 0x400) >> 10);
    uint16_t comm_pos = 0;

    if (newcomm_pos == comm_pos) return;

    comm_pos = newcomm_pos;

    if(bldc_dir)
    {
        BH1 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][0];
        BL1 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][1];

        BH2 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][2];
        BL2 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][3];

        BH3 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][4];
        BL3 = BLDC_BRIDGE_STATE_FORWARD[comm_pos][5];
    }
    else
    {
        BH1 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][0];
        BL1 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][1];

        BH2 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][2];
        BL2 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][3];

        BH3 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][4];
        BL3 = BLDC_BRIDGE_STATE_BACKWARD[comm_pos][5];
    }

    // **** this is with active freewheeling ****

    // Bridge FETs for Motor Phase U
    if (BH1)
    {
        // PWM at low side FET of bridge U
        // active freewheeling at high side FET of bridge U
        // if low side FET is in PWM off mode then the hide side FET
        // is ON for active freewheeling. This mode needs correct definition
        // of dead time otherwise we have shoot-through problems

        sPWMConfig1.OCMode     = TIM_OCMODE_PWM1;
        HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);

        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
        HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);

    } else
    {
        // Low side FET: OFF
        HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_1);

        if (BL1){
            // High side FET: ON
            sPWMConfig1.OCMode     = TIM_OCMODE_FORCED_ACTIVE;
            HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig1, TIM_CHANNEL_1);

            HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);

        } else {

            // High side FET: OFF
            HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_1);
        }
    }

    // Bridge FETs for Motor Phase V
    if (BH2)
    {
        sPWMConfig2.OCMode     = TIM_OCMODE_PWM1;
        HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig2, TIM_CHANNEL_2);

        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
        HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
    } else
    {
        HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_2);

        if (BL2)
        {
            sPWMConfig2.OCMode     = TIM_OCMODE_FORCED_ACTIVE;
            HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig2, TIM_CHANNEL_2);

            HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
        } else
        {
            HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_2);
        }
    }

    // Bridge FETs for Motor Phase W

    if (BH3) {
        sPWMConfig3.OCMode     = TIM_OCMODE_PWM1;
        HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig3, TIM_CHANNEL_3);

        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
        HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
    } else
    {
        HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_3);

        if (BL3)
        {
            sPWMConfig3.OCMode     = TIM_OCMODE_FORCED_ACTIVE;
            HAL_TIM_PWM_ConfigChannel(&htim1, &sPWMConfig3, TIM_CHANNEL_3);

            HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
        } else
        {
            HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_3);
        }
    }
}

/**
 * @brief  Commutation event callback in non blocking mode
 * @param  htim : Timer handle
 * @retval None
 */
void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim1)
    {
        BLDCMotorPrepareCommutation();
    }
}
