// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      app.c
*@author    Ziga Miklosic
*@date      22.10.2022
*@project   Base code for nRF5_nRF52840_DevBoard
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup APP
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "app.h"
#include "project_config.h"


#include "pin_mapper.h"

#include "nrf_gpio.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////



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
*		Application initialization
*
* @return 		none
*/
////////////////////////////////////////////////////////////////////////////////
void app_init(void)
{
    // Init periphery
    //gpio_init();


    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_1__PORT, LED_1__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_2__PORT, LED_2__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_3__PORT, LED_3__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_4__PORT, LED_4__PIN ) );

}

////////////////////////////////////////////////////////////////////////////////
/**
*         Application 10ms cyclic function
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void app_hndl_10ms(void)
{
    nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_1__PORT, LED_1__PIN ) );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       Application 100ms cyclic function
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void app_hndl_100ms(void)
{
      nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_2__PORT, LED_2__PIN ) );
}

////////////////////////////////////////////////////////////////////////////////
/**
*     Application 1000ms cyclic function
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void app_hndl_1000ms(void)
{
    nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_3__PORT, LED_3__PIN ) );

}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
