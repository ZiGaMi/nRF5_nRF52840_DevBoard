// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      main.c
*@author    Ziga Miklosic
*@date      22.10.2022
*@project   Base code for nRF5_nRF52840_DevBoard
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup MAIN
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>

// Project configurations
#include "project_config.h"
#include "pin_mapper.h"

// Application
#include "app.h"

// Periphery
#include "systick.h"


#include "nrf_gpio.h"
#include "nrf_drv_pwm.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////



static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);


/*
nrf_drv_pwm_config_t const config0 = {
    .output_pins =
        {
            NRF_GPIO_PIN_MAP(0,15),   // P0.17 channel 0
            13,   // P0.18 channel 1
            NRF_GPIO_PIN_MAP(1,10),   // P0.19 channel 2
            NRF_GPIO_PIN_MAP(1,11),   // P0.20 channel 3
        },
    .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
    .base_clock     = NRF_PWM_CLK_250kHz,
    .count_mode     = NRF_PWM_MODE_UP,
    .top_value      = 100,
    //.load_mode      = NRF_PWM_LOAD_COMMON,
    .load_mode      = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode      = NRF_PWM_STEP_AUTO
};
*/

#define PWM_NUM_OF_CH   4
static volatile uint16_t u16_ch_val[PWM_NUM_OF_CH] = { 10, 20, 30, 50 };


////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*	Main entry function
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	uint32_t cnt           = systick_get_ms();
	uint32_t cnt_p_10ms    = cnt;
	uint32_t cnt_p_100ms   = cnt;
	uint32_t cnt_p_1000ms  = cnt;

    // Init systick
    systick_init();

    // Init application
    app_init();

    nrf_drv_pwm_config_t config0 = {
    .output_pins =
        {
            NRF_GPIO_PIN_MAP(0,15),   // P0.17 channel 0
            13,   // P0.18 channel 1
            NRF_GPIO_PIN_MAP(1,10),   // P0.19 channel 2
            NRF_GPIO_PIN_MAP(1,11),   // P0.20 channel 3
        },
    .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
    .base_clock     = NRF_PWM_CLK_250kHz,
    .count_mode     = NRF_PWM_MODE_UP,
    .top_value      = 100,
    //.load_mode      = NRF_PWM_LOAD_COMMON,
    .load_mode      = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode      = NRF_PWM_STEP_AUTO
};

    if ( NRF_SUCCESS != nrf_drv_pwm_init( &m_pwm0, &config0, NULL ))
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }



    static nrf_pwm_sequence_t seq = 
    {   
        .values.p_individual = (nrf_pwm_values_individual_t*) &u16_ch_val,

        .length             = PWM_NUM_OF_CH,    // Number of 16-bit values in the array pointed by @p values
        .repeats            = 0,
        .end_delay          = 0
    };

        
    // NRFX_PWM_FLAG_LOOP

    nrf_drv_pwm_simple_playback( &m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP );


    // Main loop
    while ( 1 )
    {
        // Get current systick
        cnt = systick_get_ms();

        // 10ms loop
        if (((uint32_t)( cnt - cnt_p_10ms )) >= 10UL )
        {
            cnt_p_10ms = cnt;

            app_hndl_10ms();
        }

        // 100ms loop
        if (((uint32_t)( cnt - cnt_p_100ms )) >= 100UL )
        {
            cnt_p_100ms = cnt;

            app_hndl_100ms();
        }

        // 1000ms loop
        if (((uint32_t)( cnt - cnt_p_1000ms )) >= 1000UL )
        {
            cnt_p_1000ms = cnt;

            app_hndl_1000ms();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

