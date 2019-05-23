/**
 ******************************************************************************
 * @file    bldc.h
 * @author  PLF
 * @version V1.0.0
 * @date    10/01/2018
 * @brief   bldc header
 ******************************************************************************
 */

#ifndef __BLDC_H
#define __BLDC_H

/* Types definitions */
enum rotationType
{
    CW, CCW
};

void BLDC_init(void);
void BLDC_setDir(enum rotationType dir);
void BLDC_setDutyCycle(uint16_t duty_cycle);

#endif
