////////////////////////////////////////////////////////////////////////////////
/**
*@file      project_config.c
*@brief     Project configurations
*@author    Ziga Miklosic
*@date      01.08.2022
*@project	
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup PROJECT_CONFIGURATIONS
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "project_config.h"
#include "nrf_delay.h"
#include "gpio.h"

#include "drivers/peripheral/timer/timer.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*	Project level assert handler
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void project_config_assert_fail(void)
{
    timer_deinit();
    gpio_deinit();
    gpio_init();

    gpio_set( eGPIO_LED_1, eGPIO_HIGH );
    gpio_set( eGPIO_LED_2, eGPIO_LOW );
    gpio_set( eGPIO_LED_3, eGPIO_LOW );
    gpio_set( eGPIO_LED_4, eGPIO_HIGH );

    // Panic mode !!!
    while(1)
    {
        gpio_toggle( eGPIO_LED_1 );
        gpio_toggle( eGPIO_LED_2 );
        gpio_toggle( eGPIO_LED_3 );
        gpio_toggle( eGPIO_LED_4 );

        nrf_delay_ms(100);
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
