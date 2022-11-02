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
#include "drivers/peripheral/adc/adc.h"

// Middleware
#include "middleware/cli/cli/src/cli.h"
#include "middleware/parameters/parameters/src/par.h"

#include "uart.h"


#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"



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

/*


#define SAMPLES_IN_BUFFER   3

static int16_t gi16_adc_raw[SAMPLES_IN_BUFFER] = {0};
static uint32_t gu32_adc_event_cnt = 0;



static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE( 1 );
static nrf_ppi_channel_t m_ppi_channel;


 void timer_handle(nrf_timer_event_t event_type, void * p_context)
{
    // No actions...
}

void timer_with_ppi_init(void)
{
   ret_code_t err_code; // a variable to hold the error code

    // Initialize the PPI (make sure its initialized only once in your code)
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code); // check for errors

    // Create a config struct which will hold the timer configurations.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG; // configure the default settings
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // change the timer's width to 32- bit to hold large values for ticks 

    // Initialize the timer with timer handle, timer configurations, and timer handler
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handle);
    APP_ERROR_CHECK(err_code); // check for errors


    // A variable to hold the number of ticks which are calculated in this function below
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 50);

    // Initialize the channel 0 along with configurations and pass the Tick value for the interrupt event 
    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // Enable the timer - it starts ticking as soon as this function is called 
    nrf_drv_timer_enable(&m_timer);



    // Save the address of compare event so that it can be connected to ppi module
    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);

    // Save the task address to a variable so that it can be connected to ppi module for automatic triggering
    uint32_t saadc_sample_task_addr = nrf_drv_saadc_sample_task_get();

    // Allocate the ppi channel by passing it the struct created at the start
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code); // check for errors


    // attach the addresses to the allocated ppi channel so that its ready to trigger tasks on events
    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel, timer_compare_event_addr, saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code); // check for errors
}




void saadc_callback(nrf_drv_saadc_evt_t const *p_event) 
{
  
    switch ( p_event->type )
    {
        // Event generated when the buffer is filled with samples
        case NRF_DRV_SAADC_EVT_DONE:

           if ( NRF_SUCCESS == nrf_drv_saadc_buffer_convert( p_event->data.done.p_buffer, SAMPLES_IN_BUFFER ))
            {
                for ( uint8_t i = 0; i < SAMPLES_IN_BUFFER; i++ )
                {
                    gi16_adc_raw[i] = p_event->data.done.p_buffer[i];
                }

                gu32_adc_event_cnt++;
            }


            //nrf_drv_saadc_sample();

            gpio_toggle( eGPIO_TP_1 );


            break;

        // Event generated after one of the limits is reached
        case NRFX_SAADC_EVT_LIMIT:

            // TBD ...

            break;

        // Event generated when the calibration is complete
        case NRFX_SAADC_EVT_CALIBRATEDONE:

            // TBD ...

            break;

        default:
            // No actions...
            break;
    }
}




*/





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
        // Hearthbeat
        led_blink( eLED_1, 1.0f, 2.0f, eLED_BLINK_CONTINUOUS );
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


	if ( eUART_OK != uart_1_init())
	{
        cli_printf_ch( eCLI_CH_APP, "UART1 init error!" );
		PROJECT_CONFIG_ASSERT( 0 );
	}




/*
    const nrf_drv_saadc_config_t m_saadc_config = 
    {
        .resolution         = NRF_SAADC_RESOLUTION_14BIT,
        .oversample         = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = 3,
        .low_power_mode     = true
    };

    // Init SAR ADC
    if ( NRF_SUCCESS != nrf_drv_saadc_init( &m_saadc_config, saadc_callback ))
    { 
        PROJECT_CONFIG_ASSERT( 0 );
    }


    // General configuration between all ADC channels
    nrf_saadc_channel_config_t adc_channel_cfg = 
    {
        .resistor_p     = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n     = NRF_SAADC_RESISTOR_DISABLED,
        .gain           = NRF_SAADC_GAIN1_4,            // Gain = 1/4
        .reference      = NRF_SAADC_REFERENCE_VDD4,     // Vref = Vdd/4
        .acq_time       = NRF_SAADC_ACQTIME_40US,
        .mode           = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst          = NRF_SAADC_BURST_DISABLED,
        .pin_n          = (nrf_saadc_input_t)(NRF_SAADC_INPUT_DISABLED),
    };
    
    // Analog input 1
    adc_channel_cfg.pin_p = (nrf_saadc_input_t)( NRF_SAADC_INPUT_AIN1 );

    // Init channels
    if ( NRF_SUCCESS != nrf_drv_saadc_channel_init( 0, &adc_channel_cfg ))
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }

    // Analog input 2
    adc_channel_cfg.pin_p = (nrf_saadc_input_t)( NRF_SAADC_INPUT_AIN2 );

    // Init channels
    if ( NRF_SUCCESS != nrf_drv_saadc_channel_init( 1, &adc_channel_cfg ))
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }

    // Analog input 3
    adc_channel_cfg.pin_p = (nrf_saadc_input_t)( NRF_SAADC_INPUT_AIN4 );

    // Init channels
    if ( NRF_SUCCESS != nrf_drv_saadc_channel_init( 2, &adc_channel_cfg ))
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }


    if ( NRF_SUCCESS != nrf_drv_saadc_buffer_convert((int16_t*) &gi16_adc_raw, SAMPLES_IN_BUFFER ))
    {
        PROJECT_CONFIG_ASSERT( 0 );
    }


   timer_with_ppi_init();

    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);



*/

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

    cli_printf_ch( eCLI_CH_APP, "%05d, %05d, %05d", adc_get_raw( eADC_AIN_1), adc_get_raw( eADC_AIN_2), adc_get_raw( eADC_AIN_4) );


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
