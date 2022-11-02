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
#include "drivers/hmi/button/button/src/button.h"
#include "drivers/hmi/led/led/src/led.h"

// Middleware
#include "middleware/cli/cli/src/cli.h"
#include "middleware/parameters/parameters/src/par.h"

#include "uart.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static void app_btn_1_pressed	(void);
static void app_btn_1_released	(void);
static void app_btn_2_pressed	(void);
static void app_btn_2_released	(void);
static void app_btn_3_pressed	(void);
static void app_btn_3_released	(void);
static void app_btn_4_pressed	(void);
static void app_btn_4_released	(void);


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

    // Init LEDs
    if ( eLED_OK != led_init())
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }
    else
    {
        // Hearthbeat
        led_blink( eLED_1, 1.0f, 2.0f, eLED_BLINK_CONTINUOUS );
    }

    // Init buttons
    if ( eBUTTON_OK != button_init())
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }
    else
    {
        // Register btn event callbacks
        button_register_callback( eBUTTON_1, &app_btn_1_pressed, &app_btn_1_released );
        button_register_callback( eBUTTON_2, &app_btn_2_pressed, &app_btn_2_released );
        button_register_callback( eBUTTON_3, &app_btn_3_pressed, &app_btn_3_released );
        button_register_callback( eBUTTON_4, &app_btn_4_pressed, &app_btn_4_released );
    }

	// Init device paramters
	if ( ePAR_OK != par_init())
	{
		PROJECT_CONFIG_ASSERT( 0 );
	}


	if ( eUART_OK != uart_1_init())
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
    led_hndl();
    button_hndl();

	// Handle CLI
	cli_hndl();



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

	uart_1_write( "Hello World" );


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


}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 1 pressed event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_1_pressed(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 1 pressed!" );
    led_blink( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );
	
	// Set parameter
	par_set( ePAR_BTN_1, (uint8_t*) &(uint8_t){1} );

	// Further actions here...

    

}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 1 released event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_1_released(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 1 releassed!" );

	// Set parameter
	par_set( ePAR_BTN_1, (uint8_t*) &(uint8_t){0} );

	// Further actions here...


}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 2 pressed event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_2_pressed(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 2 pressed!" );
    led_blink( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

	// Set parameter
	par_set( ePAR_BTN_2, (uint8_t*) &(uint8_t){1} );

	// Further actions here...

}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 2 released event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_2_released(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 2 releassed!" );

	// Set parameter
	par_set( ePAR_BTN_2, (uint8_t*) &(uint8_t){0} );

	// Further actions here...
}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 3 pressed event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_3_pressed(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 3 pressed!" );
    led_blink( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

	// Set parameter
	par_set( ePAR_BTN_3, (uint8_t*) &(uint8_t){1} );

	// Further actions here...
}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 3 released event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_3_released(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 3 releassed!" );

	// Set parameter
	par_set( ePAR_BTN_3, (uint8_t*) &(uint8_t){0} );

	// Further actions here...
}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 4 pressed event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_4_pressed(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 4 pressed!" );
    led_blink( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

	// Set parameter
	par_set( ePAR_BTN_4, (uint8_t*) &(uint8_t){1} );

	// Further actions here...
}

////////////////////////////////////////////////////////////////////////////////
/**
*       User button 4 released event
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_btn_4_released(void)
{
	cli_printf_ch( eCLI_CH_APP, "User btn 4 releassed!" );

	// Set parameter
	par_set( ePAR_BTN_4, (uint8_t*) &(uint8_t){0} );

	// Further actions here...
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
