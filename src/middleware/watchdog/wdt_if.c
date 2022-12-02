// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      wdt_if.c
*@brief     Watchdog Platform Interface
*@author    Ziga Miklosic
*@date      02.09.2021
*@version   V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup WATCHDOG CONFIGURATIONS
* @{ <!-- BEGIN GROUP -->
*
* 	Put code that is platform depended inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include "wdt_if.h"
#include "wdt_cfg.h"

// USER INCLUDE START...

// System tick
#include "peripheral/systick/systick.h"

// WDT periphery
#include "nrf_drv_wdt.h"

// USER INCLUDE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

// USER DEFINITIONS BEGIN...

// USER DEFINITIONS END...

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// USER VARIABLES BEGIN...

/**
 *  Wdt channel
 */
static nrf_drv_wdt_channel_id g_wdt_ch_id = 0;

// USER VARIABLES END...

////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Watchdog platform initilization
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
* @return		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
wdt_status_t wdt_if_init(void)
{
    wdt_status_t status = eWDT_OK;

    // USER CODE START...
    
    //Configure WDT.   
    const nrf_drv_wdt_config_t config = 
    {
        .behaviour          = NRF_WDT_BEHAVIOUR_RUN_SLEEP,                          /**<WDT behaviour when CPU in sleep/halt mode. Look at "nrf_wdt_behaviour_t" definition */
        .reload_value       = (uint32_t) ( 10UL * WDT_CFG_KICK_PERIOD_TIME_MS ),    /**<WDT reload value in ms. NOTE: Timeout is 10x bigger that kick period! */
        .interrupt_priority = NRFX_WDT_CONFIG_IRQ_PRIORITY,
    };

    // Init WDT
    if ( NRF_SUCCESS != nrf_drv_wdt_init( &config, wdt_event_handler ))
    {
        status |= eWDT_ERROR_INIT;
    }

    // Init wdt channel
    if ( NRF_SUCCESS !=nrf_drv_wdt_channel_alloc( &g_wdt_ch_id ))
    {
        status |= eWDT_ERROR_INIT;
    }

    // USER CODE END...

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Watchdog platform WDT timer start
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
* @return		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
wdt_status_t wdt_if_start(void)
{
    wdt_status_t status = eWDT_OK;

    // USER CODE START...

    // Enable WDT
    nrf_drv_wdt_enable();

    // USER CODE END...

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Watchdog platform WDT timer kick
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
* @return		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
wdt_status_t wdt_if_kick(void)
{
    wdt_status_t status = eWDT_OK;

    // USER CODE START...

    // Kick wdt
    nrf_drv_wdt_channel_feed( g_wdt_ch_id );

    // USER CODE END...

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Get platform system ticks
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
* @return		systick - System time ticks 
*/
////////////////////////////////////////////////////////////////////////////////
uint32_t wdt_if_get_systick(void)
{
    uint32_t systick = 0;

    // USER CODE START...

    systick = systick_get_ms();

    // USER CODE END...

    return systick;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Get watchdog mutex
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
*           Return eWDT_OK if successfully acquire otherwise
*           return eWDT_ERROR
*
* @return		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
wdt_status_t wdt_if_aquire_mutex(void)
{
    wdt_status_t status = eWDT_OK;

    // USER CODE START...

    // USER CODE END...

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Release watchdog mutex
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
*           Return /ref eWDT_OK if successfully acquire otherwise 
*           return /ref eWDT_ERROR
*
* @return		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
wdt_status_t wdt_if_release_mutex(void)
{
    wdt_status_t status = eWDT_OK;

    // USER CODE START...

    // USER CODE END...

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Watchdog pre-reset ISR 
*
* @note     User shall provide definition of that function based 
*           on used platform!
*
*           Also put here /ref wdt_pre_reset_isr_callback if needed!
*
*
* @code     // Example code: TODO:
*
* @endcode
*
* @return		void
*/
////////////////////////////////////////////////////////////////////////////////
// TODO: ...

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
