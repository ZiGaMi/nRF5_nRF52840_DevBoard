// Copyright (c) 2023 Ziga Miklosic
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

// Drivers
#include "drivers/peripheral/gpio/gpio.h"
#include "drivers/peripheral/uart/uart.h"
#include "drivers/peripheral/usb_cdc/usb_cdc.h"
#include "drivers/peripheral/timer/timer.h"

#include "drivers/peripheral/ble/ble_p.h"

// HMI
#include "drivers/hmi/button/button/src/button.h"
#include "drivers/hmi/led/led/src/led.h"
#include "drivers/peripheral/adc/adc.h"

// Middleware
#include "middleware/cli/cli/src/cli.h"
#include "middleware/parameters/parameters/src/par.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static void app_led_fade_setup  (void);
static void app_btn_1_pressed	(void);
static void app_btn_1_released	(void);
static void app_btn_2_pressed	(void);
static void app_btn_2_released	(void);
static void app_btn_3_pressed	(void);
static void app_btn_3_released	(void);
static void app_btn_4_pressed	(void);
static void app_btn_4_released	(void);

static void app_update_adc_pars (void);

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
    // Init gpio
    gpio_init();

    // Init Cli
    if ( eCLI_OK != cli_init())
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }

    // Init timer
    if ( eTIMER_OK != timer_init())
    {
        cli_printf_ch( eCLI_CH_APP, "Timer init error!" );
        PROJECT_CONFIG_ASSERT( 0 );        
    }

    // Init ADC
    if ( eADC_OK != adc_init())
    {
        cli_printf_ch( eCLI_CH_APP, "ADC init error!" );
        PROJECT_CONFIG_ASSERT( 0 );
    }

    // Init LEDs
    if ( eLED_OK != led_init())
    {
    cli_printf_ch( eCLI_CH_APP, "LED init error!" );
        PROJECT_CONFIG_ASSERT( 0 );
    }
    else
    {
        // Change led configs
        app_led_fade_setup();

        // Hearthbeat
        led_blink_smooth( eLED_1, 1.0f, 2.0f, eLED_BLINK_CONTINUOUS );
    }

    // Init buttons
    if ( eBUTTON_OK != button_init())
    {
        cli_printf_ch( eCLI_CH_APP, "BUTTON init error!" );
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
        cli_printf_ch( eCLI_CH_APP, "PAR init error!" );
		PROJECT_CONFIG_ASSERT( 0 );
	}
    
    // Init USB CDC Class
    if ( eUSB_CDC_OK != usb_cdc_init())
    {
        cli_printf_ch( eCLI_CH_APP, "USB CDC init error!" );
		PROJECT_CONFIG_ASSERT( 0 );
    }

    // Init UART 1
	if ( eUART_OK != uart_1_init())
	{
        cli_printf_ch( eCLI_CH_APP, "UART1 init error!" );
		PROJECT_CONFIG_ASSERT( 0 );
	}
    
    // Init BLE Peripheral device
    if ( eBLE_P_OK != ble_p_init())
    {
        cli_printf_ch( eCLI_CH_APP, "BLE Peripheral init error!" );
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

	// Update ADC raw values
	app_update_adc_pars();

	// Handle USB CDC
	usb_cdc_hndl();
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
    // Further actions here...
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
    // Further actions here...


    //ble_p_hndl();


    // TEST CODE
    /*
    uint8_t ble_rx_data;    
    while( eBLE_P_OK == ble_p_get( &ble_rx_data ))
    {
        cli_printf("BLE RX: 0x%02X", ble_rx_data );
    }
    */
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral events
*
* @param[in]	event	- Event type
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
void ble_p_evt_cb(const ble_p_evt_t event)
{
    // Based on event
    switch( event )
    {
        /**<Peer connected event */
        case eBLE_P_EVT_CONNECT:
            led_set_smooth( eLED_2, eLED_ON );
            break;

        /**<Peer dis-connected event */
        case eBLE_P_EVT_DISCONNECT:
            led_set_smooth( eLED_2, eLED_OFF );
            break;
        
        /**<Client writes to Rx characteristics */
        case eBLE_P_EVT_RX_DATA:
            led_blink_smooth( eLED_3, 0.10f, 0.20f, eLED_BLINK_2X );
            break;

        /**<Advertising started event */
        case eBLE_P_EVT_ADV_START:
            led_blink_smooth( eLED_2, 0.1f, 1.0f, eLED_BLINK_CONTINUOUS );
            break;

        /**<Advertising ended event */
        case eBLE_P_EVT_ADV_END:
            led_set_smooth( eLED_2, eLED_OFF );
            break;

        default:
            // No actions...
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*     Setup LED fade configs
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_led_fade_setup(void)
{
     led_fade_cfg_t led_cfg = 
    { 
        .fade_in_time   = 0.1f,
        .fade_out_time  = 0.1f,
        .max_duty       = 1.0f,
    };
    led_set_fade_cfg( eLED_1, &led_cfg ); 
    led_set_fade_cfg( eLED_2, &led_cfg ); 
    led_set_fade_cfg( eLED_3, &led_cfg ); 
    led_set_fade_cfg( eLED_4, &led_cfg );    
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
    led_blink_smooth( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );
	
	// Set parameter
	par_set( ePAR_BTN_1, (uint8_t*) &(uint8_t){1} );

	// Further actions here...
    

    static uint8_t cnt[200] = {0};

    cnt[1]++;

    ble_p_write((const uint8_t*) &cnt, 200 );
    

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
    led_blink_smooth( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

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
    led_blink_smooth( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

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
    led_blink_smooth( eLED_4, 0.1f, 0.2f, eLED_BLINK_1X );

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
*       Update ADC parameters
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
static void app_update_adc_pars(void)
{
	uint16_t adc_val;

	adc_val = adc_get_raw( eADC_AIN_1 );
	par_set( ePAR_AIN_1, (uint16_t*) &adc_val );

	adc_val = adc_get_raw( eADC_AIN_2 );
	par_set( ePAR_AIN_2, (uint16_t*) &adc_val );

	adc_val = adc_get_raw( eADC_AIN_4 );
	par_set( ePAR_AIN_4, (uint16_t*) &adc_val );

	adc_val = adc_get_raw( eADC_AIN_5 );
	par_set( ePAR_AIN_5, (uint16_t*) &adc_val );

	adc_val = adc_get_raw( eADC_AIN_6 );
	par_set( ePAR_AIN_6, (uint16_t*) &adc_val );

	adc_val = adc_get_raw( eADC_AIN_7 );
	par_set( ePAR_AIN_7, (uint16_t*) &adc_val );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       USB CDC plugged in event callback
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void usb_cdc_plugged_cb(void)
{
    led_blink_smooth( eLED_3, 0.20f, 0.50f, eLED_BLINK_CONTINUOUS );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       USB CDC unplugged event callback
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void usb_cdc_unplugged_cb(void)
{
    led_set_smooth( eLED_3, eLED_OFF );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       USB CDC Virtual COM port open event callback
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void usb_cdc_port_open_cb(void)
{
    led_set_smooth( eLED_3, eLED_ON );
}

////////////////////////////////////////////////////////////////////////////////
/**
*       USB CDC Virtual COM port close event callback
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void usb_cdc_port_close_cb(void)
{
    led_blink_smooth( eLED_3, 0.20f, 0.50f, eLED_BLINK_CONTINUOUS );
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
