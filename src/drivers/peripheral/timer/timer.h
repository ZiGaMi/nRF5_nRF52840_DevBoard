// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      timer.h
*@brief     Timer low level driver
*@author    Ziga Miklosic
*@date      23.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup TIMER
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __TIMER_H
#define __TIMER_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 	Timer status
 */
typedef enum
{
    eTIMER_OK = 0,	/**<Normal operation */
    eTIMER_ERROR,	/**<General error code */
} timer_status_t;

/**
 * 	Timer channels
 */
typedef enum
{
	eTIMER_PWM0_CH1 = 0,
    eTIMER_PWM0_CH2,
    eTIMER_PWM0_CH3,
    eTIMER_PWM0_CH4,

	eTIMER_CH_NUM_OF,
} timer_ch_t;

/**
 *     32-bit floating point definition
 */
typedef float float32_t;

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
timer_status_t 	timer_init			(void);
timer_status_t 	timer_is_init		(bool * const p_is_init);
timer_status_t 	timer_set_pwm		(const timer_ch_t ch, const float32_t duty);

#endif // __TIMER_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
