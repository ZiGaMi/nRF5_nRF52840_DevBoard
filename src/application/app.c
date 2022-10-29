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


#include "gpio.h"

// HMI
#include "button/src/button.h"

// Middleware
#include "middleware/cli/cli/src/cli.h"
#include "middleware/parameters/parameters/src/par.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static void app_user_btn_pressed	(void);
static void app_user_btn_released	(void);


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
    gpio_init();

    // Init Cli
    if ( eCLI_OK != cli_init())
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }

    // Init buttons
    if ( eBUTTON_OK != button_init())
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }
    else
    {
        // Register btn event callbacks
        button_register_callback( eBUTTON_1, &app_user_btn_pressed, &app_user_btn_released );
    }

	// Init device paramters
	if ( ePAR_OK != par_init())
	{
		PROJECT_CONFIG_ASSERT( 0 );
	}

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
    // Handle HMI
    button_hndl();

	// Handle CLI
	cli_hndl();


	// Debugging
    gpio_toggle( eGPIO_LED_1 );

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
    gpio_toggle( eGPIO_LED_2 );


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
    gpio_toggle( eGPIO_LED_3 );


}


////////////////////////////////////////////////////////////////////////////////
/**
*       User button pressed event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_user_btn_pressed(void)
{
    gpio_set( eGPIO_LED_4, eGPIO_HIGH );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button released event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_user_btn_released(void)
{
    gpio_set( eGPIO_LED_4, eGPIO_LOW );
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
