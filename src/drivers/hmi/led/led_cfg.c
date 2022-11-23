// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
* @file     led_cfg.c
* @brief    LED configurations
* @author   Ziga Miklosic
* @date     27.09.2021
* @version	V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
 * @addtogroup LED_CFG
 * @{ <!-- BEGIN GROUP -->
 *
 *
 * 	Configuration for LED
*
* 	User shall put code inside inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
 *
 */
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "led_cfg.h"
#include "led/src/led.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * 	LED configuration table
 *
 *
 *	@brief 	This table is being used for setting up LED low level drivers.
 *
 *			Two options are supported:
 *				1. GPIO
 *				2. Timer PWM
 *
 *
 * 	@note 	Low level gpio and timer code must be compatible!
 */
static const led_cfg_t g_led_cfg[ eLED_NUM_OF ] =
{

	// USER CODE BEGIN...

	// -------------------------------------------------------------------------------------------------------------------------------------------------------
	//              Driver type                         LED driver channel					Initial State					Polarity
	// -------------------------------------------------------------------------------------------------------------------------------------------------------
	[eLED_1]    = { .drv_type = eLED_DRV_TIMER_PWM,		.drv_ch.tim_ch = eTIMER_PWM0_CH1,     .initial_state = eLED_ON, 		.polarity = eLED_POL_ACTIVE_LOW     },
	[eLED_2]    = { .drv_type = eLED_DRV_TIMER_PWM,		.drv_ch.tim_ch = eTIMER_PWM0_CH2,     .initial_state = eLED_OFF, 		.polarity = eLED_POL_ACTIVE_LOW     },
	[eLED_3]    = { .drv_type = eLED_DRV_TIMER_PWM,		.drv_ch.tim_ch = eTIMER_PWM0_CH3,     .initial_state = eLED_OFF, 		.polarity = eLED_POL_ACTIVE_LOW     },
	[eLED_4]    = { .drv_type = eLED_DRV_TIMER_PWM,		.drv_ch.tim_ch = eTIMER_PWM0_CH4,     .initial_state = eLED_OFF, 		.polarity = eLED_POL_ACTIVE_LOW     },


	// USER CODE END...

};

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Get LEDs configuration table
*
* @return		pointer to configuration table
*/
////////////////////////////////////////////////////////////////////////////////
const void * led_cfg_get_table(void)
{
	return (led_cfg_t*) &g_led_cfg;
}

////////////////////////////////////////////////////////////////////////////////
/*!
 * @} <!-- END GROUP -->
 */
////////////////////////////////////////////////////////////////////////////////
