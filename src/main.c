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

#include "nrf_gpio.h"
#include "nrf_drv_uart.h"

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


static void uart_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    if (p_event->type == NRF_DRV_UART_EVT_RX_DONE)
    {

    }
    else if (p_event->type == NRF_DRV_UART_EVT_ERROR)
    {

    }
    else if (p_event->type == NRF_DRV_UART_EVT_TX_DONE)
    {

    }
}


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

	uint32_t ret_code;
	nrf_drv_uart_t uart_driver_instance = NRF_DRV_UART_INSTANCE( 1 );
	
	//nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
	nrf_drv_uart_config_t config = 
	{
		.pseltxd			=  NRF_GPIO_PIN_MAP( 1, 2 ),          
		.pselrxd			=  NRF_GPIO_PIN_MAP( 1, 1 ),        
		.pselcts			= NRF_UART_PSEL_DISCONNECTED,
		.pselrts			= NRF_UART_PSEL_DISCONNECTED,
		.p_context			= NULL,
		.hwfc				= NRF_UART_HWFC_DISABLED,    
		.parity				= NRF_UART_PARITY_EXCLUDED,
		.baudrate			= NRF_UARTE_BAUDRATE_115200,
		.interrupt_priority	= 6,
		.use_easy_dma		= true,
	};
	

	ret_code = nrf_drv_uart_init( &uart_driver_instance, &config, uart_event_handler );

	static uint8_t rx_buffer[1];
	static uint8_t tx_buffer[2] = { 'a', 'b' };
	nrf_drv_uart_rx(&uart_driver_instance, rx_buffer,1);


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

			nrf_drv_uart_tx( &uart_driver_instance, &tx_buffer, 2 );
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

