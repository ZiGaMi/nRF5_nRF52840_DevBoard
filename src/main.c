// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      main.c
*@author    Ziga Miklosic
*@date      22.10.2022
*@project   Base code for nRF5_nRF52840_DevBoard
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup MAIN
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdint.h>

// Project configurations
#include "project_config.h"
#include "pin_mapper.h"

// Application
#include "app.h"

// Periphery
#include "systick.h"

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
*	Main entry function
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	uint32_t cnt           = systick_get_ms();
	uint32_t cnt_p_10ms    = cnt;
	uint32_t cnt_p_100ms   = cnt;
	uint32_t cnt_p_1000ms  = cnt;

    // Init systick
    systick_init();

    // Init application
    app_init();

    // Main loop
    while ( 1 )
    {
        // Get current systick
        cnt = systick_get_ms();

        // 10ms loop
        if (((uint32_t)( cnt - cnt_p_10ms )) >= 10UL )
        {
            cnt_p_10ms = cnt;

            app_hndl_10ms();
        }

        // 100ms loop
        if (((uint32_t)( cnt - cnt_p_100ms )) >= 100UL )
        {
            cnt_p_100ms = cnt;

            app_hndl_100ms();
        }

        // 1000ms loop
        if (((uint32_t)( cnt - cnt_p_1000ms )) >= 1000UL )
        {
            cnt_p_1000ms = cnt;

            app_hndl_1000ms();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

