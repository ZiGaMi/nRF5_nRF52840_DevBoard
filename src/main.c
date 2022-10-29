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



#include "app_uart.h"
#include "nrf_uart.h"

/*
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif
*/

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}



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


	uint32_t err_code;


    // Init systick
    systick_init();

    // Init application
    app_init();



    const app_uart_comm_params_t comm_params =
      {
          DBG_UART_RX__PIN,
          DBG_UART_TX__PIN,
          DBG_UART_RTS__PIN,
          DBG_UART_CTS__PIN,
          true,
          false,
#if defined (UART_PRESENT)
          NRF_UART_BAUDRATE_115200
#else
          NRF_UARTE_BAUDRATE_115200
#endif
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);



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

