// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      adc.c
*@brief     ADC low level driver
*@author    Ziga Miklosic
*@date      02.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup ADC
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"

#include "adc.h"
#include "pin_mapper.h"
#include "project_config.h"

#include "drivers/peripheral/gpio/gpio.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


/**
 *      ADC resolution
 *
 * @note    Select one of the "nrf_saadc_resolution_t" options inside "nrf_saadc.h"!
 *          
 *          Options: 
 *              - NRF_SAADC_RESOLUTION_8BIT, 
 *              - NRF_SAADC_RESOLUTION_10BIT, 
 *              - NRF_SAADC_RESOLUTION_12BIT, 
 *              - NRF_SAADC_RESOLUTION_14BIT
 */
 #define ADC_RESOLUTION                 ( NRF_SAADC_RESOLUTION_12BIT )

 /**
  *     ADC IRQ Priority
  */
 #define ADC_IRQ_PRIORITY               ( 3 )

 /**
  *     ADC Sample Rate
  *
  *  Unit: Hz
  *  Max: 2000 Hz
  *  Min: 1 Hz
  */
#define ADC_SAMPLE_RATE_HZ              ( 100 )


/**
 *		ADC asserts
 */
 #define ADC_ASSERT_EN                  ( 1 )

 #if ( ADC_ASSERT_EN )
	#define ADC_ASSERT(x)               { PROJECT_CONFIG_ASSERT(x) }
 #else
  #define ADC_ASSERTx)                  { ; }
 #endif


////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 *      Initialization guards
 */
static bool gb_is_init = false;

/**
 *      ADC Channel Configurations
 *
 * @note    Sequence of configurations dictates also place in sample buffer (gi16_adc_raw)! Meaning     
 *          first configurable channel will have raw value store inside "gi16_adc_raw[0]".
 */
static const nrf_saadc_channel_config_t g_adc_channel[eADC_NUM_OF] =
{
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //               Positive Pin                   Negative Pin                        Pull Up Resistor                          Pull Down Resistor                            Input Gain                  ADC Reference                           Sample Time                         Mode                                Burst Mode
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // USER CODE BEGIN...
    
    [eADC_AIN_1] = { .pin_p = NRF_SAADC_INPUT_AIN1, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    [eADC_AIN_2] = { .pin_p = NRF_SAADC_INPUT_AIN2, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    [eADC_AIN_4] = { .pin_p = NRF_SAADC_INPUT_AIN4, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    [eADC_AIN_5] = { .pin_p = NRF_SAADC_INPUT_AIN5, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    [eADC_AIN_6] = { .pin_p = NRF_SAADC_INPUT_AIN6, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    [eADC_AIN_7] = { .pin_p = NRF_SAADC_INPUT_AIN7, .pin_n = NRF_SAADC_INPUT_DISABLED, .resistor_p = NRF_SAADC_RESISTOR_DISABLED, .resistor_n = NRF_SAADC_RESISTOR_DISABLED,    .gain = NRF_SAADC_GAIN1_4,  .reference = NRF_SAADC_REFERENCE_VDD4,  .acq_time = NRF_SAADC_ACQTIME_40US, .mode = NRF_SAADC_MODE_SINGLE_ENDED, .burst = NRF_SAADC_BURST_DISABLED  },
    
    // USER CODE END...

};

/**
 *      ADC Raw Samples buffer
 */
static int16_t gi16_adc_raw[eADC_NUM_OF] = {0};




static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE( 1 );
static nrf_ppi_channel_t m_ppi_channel;



////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static adc_status_t adc_init_channels   (void);
static adc_status_t adc_init_timer      (void);



////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialization of ADC Channels
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static adc_status_t adc_init_channels(void)
{
    adc_status_t status = eADC_OK;

    for ( uint32_t ch = 0; ch < eADC_NUM_OF; ch++ )
    {
        // Init channels
        if ( NRF_SUCCESS != nrf_drv_saadc_channel_init( ch, &g_adc_channel[ch] ))
        {
            status = eADC_ERROR;
            break;
        }
    }
        
    return status;
}



 void timer_handle(nrf_timer_event_t event_type, void * p_context)
{
    // No actions...
}

static adc_status_t adc_init_timer(void)
{
   adc_status_t status = eADC_OK;

    // Initialize the PPI (make sure its initialized only once in your code)
    if ( NRF_SUCCESS != nrf_drv_ppi_init())
    {
        status |= eADC_ERROR;
    }

    // Create a config struct which will hold the timer configurations.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG; // configure the default settings
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32; // change the timer's width to 32- bit to hold large values for ticks 

    // Initialize the timer with timer handle, timer configurations, and timer handler
    if ( NRF_SUCCESS != nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handle))
    {
        status |= eADC_ERROR;
    }


    // A variable to hold the number of ticks which are calculated in this function below
    //uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 50);
    uint32_t ticks = nrf_drv_timer_us_to_ticks( &m_timer, (uint32_t)( 1000000UL / ADC_SAMPLE_RATE_HZ ));

    // Initialize the channel 0 along with configurations and pass the Tick value for the interrupt event 
    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // Enable the timer - it starts ticking as soon as this function is called 
    nrf_drv_timer_enable(&m_timer);



    // Save the address of compare event so that it can be connected to ppi module
    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);

    // Save the task address to a variable so that it can be connected to ppi module for automatic triggering
    uint32_t saadc_sample_task_addr = nrf_drv_saadc_sample_task_get();

    // Allocate the ppi channel by passing it the struct created at the start
    if ( NRF_SUCCESS != nrf_drv_ppi_channel_alloc(&m_ppi_channel))
    {
        status |= eADC_ERROR;
    }

    // attach the addresses to the allocated ppi channel so that its ready to trigger tasks on events
    if ( NRF_SUCCESS != nrf_drv_ppi_channel_assign(m_ppi_channel, timer_compare_event_addr, saadc_sample_task_addr))
    {
        status |= eADC_ERROR;
    }

    return status;
}




void adc_event_hndl(nrf_drv_saadc_evt_t const *p_event) 
{
    // Debugging 
    gpio_toggle( eGPIO_TP_1 );

    // Check event
    switch ( p_event->type )
    {
        // Event generated when the buffer is filled with samples
        case NRF_DRV_SAADC_EVT_DONE:

           // Get raw samples
            if ( NRF_SUCCESS == nrf_drv_saadc_buffer_convert( p_event->data.done.p_buffer, eADC_NUM_OF ))
            {
                for ( uint32_t ch = 0; ch < eADC_NUM_OF; ch++ )
                {
                    gi16_adc_raw[ ch ] = p_event->data.done.p_buffer[ ch ];
                }
            }

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







////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup ADC_API
* @{ <!-- BEGIN GROUP -->
*
* 	Following function are part or ADC API.
*/
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/**
*		Initialization of ADC
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
adc_status_t adc_init(void)
{
    adc_status_t status = eADC_OK;

    if ( false == gb_is_init )
    {
        // Configure ADC
        const nrf_drv_saadc_config_t adc_cfg = 
        {
            .resolution         = ADC_RESOLUTION,
            .oversample         = NRF_SAADC_OVERSAMPLE_DISABLED,
            .interrupt_priority = ADC_IRQ_PRIORITY,
            .low_power_mode     = true
        };

        // Init SAR ADC
        if ( NRF_SUCCESS != nrf_drv_saadc_init( &adc_cfg, adc_event_hndl ))
        { 
             status = eADC_ERROR;
        }

        // Init ADC channels
        status = adc_init_channels();

        // Start conversion
        if ( NRF_SUCCESS != nrf_drv_saadc_buffer_convert((int16_t*) &gi16_adc_raw, eADC_NUM_OF ))
        {
            PROJECT_CONFIG_ASSERT( 0 );
        }

        // Init ADC triggering timer
        status |= adc_init_timer();
        
        // Init PPI
        if ( NRF_SUCCESS != nrf_drv_ppi_channel_enable( m_ppi_channel ))
        {
            status = eADC_ERROR;
        }

        // Init success
        if ( eADC_OK == status )
        {
            gb_is_init = true;
        }
    }
    else
    {
        status = eADC_ERROR;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*       Get raw ADC value
*
* @param[in] 	pin		- ADC pin
* @return 		raw_val	- Raw converted value
*/
////////////////////////////////////////////////////////////////////////////////
uint16_t adc_get_raw(const adc_pins_t pin)
{
    uint16_t raw = 0;

    ADC_ASSERT( true == gb_is_init );
    ADC_ASSERT( pin < eADC_NUM_OF );

    if  (   ( true == gb_is_init )
        &&  ( pin < eADC_NUM_OF ))
    {
        if ( gi16_adc_raw[ pin ] > 0 )
        {
            raw = gi16_adc_raw[ pin ];
        }
        else
        {
            raw = 0U;
        }
    }

    return raw;
}

////////////////////////////////////////////////////////////////////////////////
/**
*	G   et real ADC value
*
*	Conversion from raw and real is part of configuration table
*
* @param[in] 	pin			- ADC pin
* @return 		real_val	- Real (physical) converted value in V
*/
////////////////////////////////////////////////////////////////////////////////
float32_t adc_get_real(const adc_pins_t pin)
{
    float32_t real = 0.0f;

    ADC_ASSERT( true == gb_is_init );
    ADC_ASSERT( pin < eADC_NUM_OF );

    if  (   ( true == gb_is_init )
        &&  ( pin < eADC_NUM_OF ))
    {   
           // TODO: ...
    }

    return real;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
