// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      timer.c
*@brief     Timer low level driver
*@author    Ziga Miklosic
*@date      23.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup TIMER
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_pwm.h"

#include "pin_mapper.h"
#include "project_config.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *  Timer PWM0 PWM output frequency
 *
 *  Unit: Hz
 */
#define TIMER_PWM0_FREQ_HZ          ( 5000 )

/**
 *  Timer PWM0 Base frequency
 *
 * @note   Set this value based on Timer0 cfg ".base_clock"
 *
 *  Unit: Hz
 */
#define TIMER_PWM0_BASE_FREQ_HZ     ( 1000000UL )  

/**
 *      Calculate timer period
 */
#define TIMER_PWM0_PERIOD           ((uint16_t)( TIMER_PWM0_BASE_FREQ_HZ / TIMER_PWM0_FREQ_HZ ))


#if 0
typedef struct
{
    nrf_drv_pwm_t *     p_tim;          /**<Timer instance */
    float32_t           freq;           /**<Base frequency of timer */
    nrf_pwm_mode_t      mode;           /**<Counter mode */
} timer_tim_t;

typedef enum
{
    eTIMER_CH1 = 0,
    eTIMER_CH2,
    eTIMER_CH3,
    eTIMER_CH4
} timer_ch_opt_t;
#endif


/**
 * 	Timer configuration table structure
 */
typedef struct
{
    uint32_t        port;       /**<Port */
    uint32_t        pin;    	/**<Pin */
	nrf_drv_pwm_t * p_tim;      /**<Pointer to timer handler */
} timer_tim_ch_t;



////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialization guards
 */
static bool gb_is_init = false;

/**
 *  Compare register values
 *
 * @note    Must have static storage duration!
 */
static volatile uint16_t gu16_compare_val[eTIMER_CH_NUM_OF] = {0};

/**
 *  Timer PWM0 instance
 */
static nrf_drv_pwm_t gh_timer_pwm_0 = NRF_DRV_PWM_INSTANCE(0);

/**
 *  Timer PWM0 Sequence
 */
static nrf_pwm_sequence_t g_pwm0_sequence = 
{   
    .values.p_individual    = (nrf_pwm_values_individual_t*) &gu16_compare_val,
    .length                 = eTIMER_CH_NUM_OF, 
    .repeats                = 0,
    .end_delay              = 0
};

/**
 *  Timer configuraiton table
 */
static const timer_tim_ch_t g_timer_cfg[eTIMER_CH_NUM_OF] =
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //      TIMER 0 CONFIGURATION
    //
    // @note Seqeunce dictates timer channel!
    // ----------------------------------------------------------------------------------------------------------------
    //                      GPIO Port               GPIO Pin            Timer handler             
    // ----------------------------------------------------------------------------------------------------------------
    [eTIMER_PWM0_CH1] = { .port = LED_1__PORT,  .pin = LED_1__PIN,  .p_tim = &gh_timer_pwm_0,   },
    [eTIMER_PWM0_CH2] = { .port = LED_2__PORT,  .pin = LED_2__PIN,  .p_tim = &gh_timer_pwm_0    },
    [eTIMER_PWM0_CH3] = { .port = LED_3__PORT,  .pin = LED_3__PIN,  .p_tim = &gh_timer_pwm_0    },
    [eTIMER_PWM0_CH4] = { .port = LED_4__PORT,  .pin = LED_4__PIN,  .p_tim = &gh_timer_pwm_0    },

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};


////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
////////////////////////////////////////////////////////////////////////////////
static timer_status_t timer_pwm0_init(void);


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////





static timer_status_t timer_pwm0_init(void)
{
    timer_status_t status = eTIMER_OK;

    // Prepare config for Timer0
    nrf_drv_pwm_config_t timer_0_cfg = 
    {
        .output_pins =
        {
            NRF_GPIO_PIN_MAP( g_timer_cfg[eTIMER_PWM0_CH1].port, g_timer_cfg[eTIMER_PWM0_CH1].pin ),   
            NRF_GPIO_PIN_MAP( g_timer_cfg[eTIMER_PWM0_CH2].port, g_timer_cfg[eTIMER_PWM0_CH2].pin ),   
            NRF_GPIO_PIN_MAP( g_timer_cfg[eTIMER_PWM0_CH3].port, g_timer_cfg[eTIMER_PWM0_CH3].pin ),   
            NRF_GPIO_PIN_MAP( g_timer_cfg[eTIMER_PWM0_CH4].port, g_timer_cfg[eTIMER_PWM0_CH4].pin ),   
        },

        .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
        .base_clock     = NRF_PWM_CLK_1MHz,
        .count_mode     = NRF_PWM_MODE_UP,
        .top_value      = TIMER_PWM0_PERIOD,
        .load_mode      = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode      = NRF_PWM_STEP_AUTO
    };

    // Init timer 0
    if ( NRF_SUCCESS != nrf_drv_pwm_init( &gh_timer_pwm_0, &timer_0_cfg, NULL ))
    {
        status = eTIMER_ERROR;
    }

    // Setup sequence
    nrf_drv_pwm_simple_playback( &gh_timer_pwm_0, &g_pwm0_sequence, 1, NRFX_PWM_FLAG_LOOP );

    return status;  
}





////////////////////////////////////////////////////////////////////////////////
/*!
 * @} <!-- END GROUP -->
 */
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup TIMER_API
* @{ <!-- BEGIN GROUP -->
*
* 	Following functions are part of API calls.
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Timer initialization
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
timer_status_t timer_init(void)
{
    timer_status_t status = eTIMER_OK;

    if ( false == gb_is_init )
    {
        // Init timer PWM0
        status |= timer_pwm0_init();

        // Init success
        if ( eTIMER_OK == status )
        {
            gb_is_init = true;
        }
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Get init status
*
* @param[out]   p_is_init   - Initialization flag
* @return 		status      - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
timer_status_t timer_is_init(bool * const p_is_init)
{
    timer_status_t status = eTIMER_OK;

    if ( NULL != p_is_init )
    {
        *p_is_init = gb_is_init;
    }
    else
    {
        status = eTIMER_ERROR;
    }

    return status;   
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Set timer PWM duty
*
* @param[in] 	ch 		- Channel number
* @param[in] 	duty 	- Duty cycle in range 0.0-1.0
* @return 		status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
timer_status_t timer_set_pwm(const timer_ch_t ch, const float32_t duty)
{
    timer_status_t status = eTIMER_OK;

    PROJECT_CONFIG_ASSERT( true == gb_is_init );
    PROJECT_CONFIG_ASSERT( ch < eTIMER_CH_NUM_OF );

    if  (   ( true == gb_is_init )
        &&  ( ch < eTIMER_CH_NUM_OF ))
    {
        if (( duty >= 0.0f ) && ( duty <= 1.0f ))
		{
            gu16_compare_val[ch] = (uint16_t)( TIMER_PWM0_PERIOD * ( 1.0f - duty ));
		}
		else
		{
			status = eTIMER_ERROR;
		}
    }
    else
    {
        status = eTIMER_ERROR;
    }

    return status;   
}
    

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
