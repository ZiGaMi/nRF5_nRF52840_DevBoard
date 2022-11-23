// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      gpio.h
*@brief     GPIO
*@author    Ziga Miklosic
*@date      22.10.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup GPIO
* @{ <!-- BEGIN GROUP -->
*
* 	GPIO manipulations
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __GPIO_H
#define __GPIO_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 	GPIO status
 */
typedef enum
{
    eGPIO_OK = 0,	/**<Normal operation */
    eGPIO_ERROR,	/**<General error code */
} gpio_status_t;

/**
 *  GPIO pins
 */
typedef enum
{
    // Outputs
    eGPIO_LED_1 = 0,
    eGPIO_LED_2,
    eGPIO_LED_3,
    eGPIO_LED_4,

    //eGPIO_TP_1,
    //eGPIO_TP_2,
    //eGPIO_TP_3,

    // Inputs
    eGPIO_BTN_1,
    eGPIO_BTN_2,
    eGPIO_BTN_3,
    eGPIO_BTN_4,

    eGPIO_NUM_OF
} gpio_pins_t;

/**
 * 	GPIO state
 */
typedef enum
{
    eGPIO_LOW = 0,      /**<Logic low state */
    eGPIO_HIGH,		/**<Logic high state */
    eGPIO_UKNOWN,	/**<Uknown state - during startup time */
} gpio_state_t;

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
gpio_status_t 	gpio_init	(void);
gpio_status_t	gpio_is_init	(bool * const p_is_init);
gpio_state_t	gpio_get	(const gpio_pins_t pin);
void		gpio_set	(const gpio_pins_t pin, const gpio_state_t state);
void		gpio_toggle	(const gpio_pins_t pin);


#endif // __GPIO_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
