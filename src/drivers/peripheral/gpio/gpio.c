// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      gpio.c
*@brief     GPIO
*@author    Ziga Miklosic
*@date      10.22.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup GPIO
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "nrf_gpio.h"

#include "gpio.h"
#include "pin_mapper.h"
#include "project_config.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 	GPIO configuration structure
 */
typedef struct
{
    const uint32_t        port;		/**<Port */
    const uint32_t        pin;		/**<Pin */
    nrf_gpio_pin_dir_t    dir;		/**<Direction */
    nrf_gpio_pin_pull_t   pull;		/**<Pull device mode */
    const gpio_state_t    init_state;	/**<Initial state */
} gpio_cfg_table_t;


////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialization guards
 */
static bool gb_is_init = false;

/**
 * 	GPIO CONFIGURATION TABLE
 */
static const gpio_cfg_table_t g_gpio_cfg_table[ eGPIO_NUM_OF ] =
{
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // OUTPUTS		PORT                      PIN                   DIRECTION                           PULL DEVICE                       INIT STATE
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    [eGPIO_LED_1]   = {   .port = LED_1__PORT,    .pin = LED_1__PIN,    .dir = NRF_GPIO_PIN_DIR_OUTPUT,     .pull = NRF_GPIO_PIN_NOPULL,      .init_state = eGPIO_HIGH        },
    [eGPIO_LED_2]   = {   .port = LED_2__PORT,    .pin = LED_2__PIN,    .dir = NRF_GPIO_PIN_DIR_OUTPUT,     .pull = NRF_GPIO_PIN_NOPULL,      .init_state = eGPIO_HIGH        },
    [eGPIO_LED_3]   = {   .port = LED_3__PORT,    .pin = LED_3__PIN,    .dir = NRF_GPIO_PIN_DIR_OUTPUT,     .pull = NRF_GPIO_PIN_NOPULL,      .init_state = eGPIO_LOW        },
    [eGPIO_LED_4]   = {   .port = LED_4__PORT,    .pin = LED_4__PIN,    .dir = NRF_GPIO_PIN_DIR_OUTPUT,     .pull = NRF_GPIO_PIN_NOPULL,      .init_state = eGPIO_LOW        },
        
        
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // INPUTS		PORT                      PIN                   DIRECTION                           PULL DEVICE                       INIT STATE
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    [eGPIO_BTN_1]   = {   .port = BTN_1__PORT,    .pin = BTN_1__PIN,    .dir = NRF_GPIO_PIN_DIR_INPUT,      .pull = NRF_GPIO_PIN_PULLUP,      .init_state = 0                 },
    [eGPIO_BTN_2]   = {   .port = BTN_2__PORT,    .pin = BTN_2__PIN,    .dir = NRF_GPIO_PIN_DIR_INPUT,      .pull = NRF_GPIO_PIN_PULLUP,      .init_state = 0                 },
    [eGPIO_BTN_3]   = {   .port = BTN_3__PORT,    .pin = BTN_3__PIN,    .dir = NRF_GPIO_PIN_DIR_INPUT,      .pull = NRF_GPIO_PIN_PULLUP,      .init_state = 0                 },
    [eGPIO_BTN_4]   = {   .port = BTN_4__PORT,    .pin = BTN_4__PIN,    .dir = NRF_GPIO_PIN_DIR_INPUT,      .pull = NRF_GPIO_PIN_PULLUP,      .init_state = 0                 },

};

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*	GPIO initialization
*
* @return 	status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
gpio_status_t gpio_init(void)
{
    gpio_status_t status = eGPIO_OK;

    if ( false == gb_is_init )
    {
        // Init all GPIOs
        for ( uint32_t pin = 0; pin < eGPIO_NUM_OF; pin++ )
        {
            // Set output state
            if ( NRF_GPIO_PIN_DIR_OUTPUT == g_gpio_cfg_table[pin].dir )
            {
                if ( eGPIO_HIGH == g_gpio_cfg_table[pin].init_state )
                {
                    nrf_gpio_pin_write( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ), 1 );
                }
                else if ( eGPIO_LOW == g_gpio_cfg_table[pin].init_state  )
                {
                    nrf_gpio_pin_write( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ), 0 );
                }
                else
                {
                        // No actions...
                }

                // Configure as output
                nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ));
            }
            else
            {
                // Configure as input
                nrf_gpio_cfg_input( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ), g_gpio_cfg_table[pin].pull );
            }
        }

        // Alles gut - init succeed
        if ( eGPIO_OK == status )
        {
            gb_is_init = true;
        }
    }
    else
    {
        status = eGPIO_ERROR;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*	Get GPIO init flag
*
* @param[out] 	p_is_init	- Pointer to GPIO init flag
* @return 	state		- Current state of pin
*/
////////////////////////////////////////////////////////////////////////////////
gpio_status_t gpio_is_init(bool * const p_is_init)
{
    gpio_status_t status = eGPIO_OK;

    if ( NULL != p_is_init )
    {
            *p_is_init = gb_is_init;
    }
    else
    {
            status = eGPIO_ERROR;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*         Get GPIO state
*
* @param[in] 	pin	- GPIO pin
* @return 	state	- Current state of pin
*/
////////////////////////////////////////////////////////////////////////////////
gpio_state_t gpio_get(const gpio_pins_t pin)
{
    gpio_state_t pin_state = eGPIO_UKNOWN;

    PROJECT_CONFIG_ASSERT( pin < eGPIO_NUM_OF );

    if ( true == gb_is_init )
    {
        if ( 1 == nrf_gpio_pin_read( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin )))
        {
            pin_state = eGPIO_HIGH;
        }
        else
        {
            pin_state = eGPIO_LOW;
        }
    }

    return pin_state;
}

////////////////////////////////////////////////////////////////////////////////
/**
*       Set GPIO state
*
* @param[in] 	pin	- GPIO pin
* @param[in] 	state	- Current state of pin
* @return	void
*/
////////////////////////////////////////////////////////////////////////////////
void gpio_set(const gpio_pins_t pin, const gpio_state_t state)
{
    PROJECT_CONFIG_ASSERT( pin < eGPIO_NUM_OF );

    if ( true == gb_is_init )
    {
        if ( eGPIO_HIGH == state )
        {
            nrf_gpio_pin_write( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ), 1 );
        }
        else if ( eGPIO_LOW == state )
        {
            nrf_gpio_pin_write( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port, g_gpio_cfg_table[pin].pin ), 0 );
        }
        else
        {
                // No actions...
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*	Toggle GPIO state
*
* @param[in] 	pin   - GPIO pin
* @return	void
*/
////////////////////////////////////////////////////////////////////////////////
void gpio_toggle(const gpio_pins_t pin)
{
    PROJECT_CONFIG_ASSERT( pin < eGPIO_NUM_OF );

    if ( true == gb_is_init )
    {
       nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP(  g_gpio_cfg_table[pin].port,  g_gpio_cfg_table[pin].pin ));
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
