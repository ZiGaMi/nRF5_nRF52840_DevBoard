// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      uart_dbg.c
*@brief     Debug UART
*@author    Ziga Miklosic
*@date      29.10.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup UART_DBG
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "uart_dbg.h"
#include "pin_mapper.h"
#include "project_config.h"

#include "app_uart.h"
#include "nrf_uart.h"
#include "nrf_uarte.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *		Transmittion buffer size
 *
 *	Unit: byte
 */
#define UART_DBG_TX_BUF_SIZE		( 512 )                         

/**
 *		Reception buffer size
 *
 *	Unit: byte
 */
#define UART_DBG_RX_BUF_SIZE		( 512 )   

/**
 *		Baudrate
 *
 * @note	Select one of the "nrf_uart_baudrate_t" enumeration options inside
 *			"nrf_uart.h" file.
 */
#define UART_DBG_BAUDRATE			( NRF_UARTE_BAUDRATE_115200 )   

/**
 *		Debug UART asserts
 */
 #define UART_DBG_ASSERT_EN			( 1 )

 #if ( UART_DBG_ASSERT_EN )
	#define UART_DBG_ASSERT(x)		{ PROJECT_CONFIG_ASSERT(x) }
 #else
  #define UART_DBG_ASSERT(x)		{ ; }
 #endif


////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialization guards
 */
static bool gb_is_init = false;


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////



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
/**
*		Initialization of debug uart
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_dbg_status_t uart_dbg_init	(void)
{
	uart_dbg_status_t status = eUART_DBG_OK;
		uint32_t err_code;

	if ( false == gb_is_init ) 
	{
		const app_uart_comm_params_t comm_params =
		{
			DBG_UART_RX__PIN,
			DBG_UART_TX__PIN,
			DBG_UART_RTS__PIN,
			DBG_UART_CTS__PIN,
			true,
			false,
			UART_DBG_BAUDRATE
		};

		APP_UART_FIFO_INIT( &comm_params, UART_DBG_RX_BUF_SIZE, UART_DBG_TX_BUF_SIZE, uart_error_handle, APP_IRQ_PRIORITY_LOWEST, err_code );

		APP_ERROR_CHECK(err_code);

		// TODO: Check if init OK
		gb_is_init = true;
	}
	else
	{
		status = eUART_DBG_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		UART transmit
*
* @param[in] 	pc_string	- String to be sended over UART
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_dbg_status_t uart_dbg_write(const char* str)
{
	uart_dbg_status_t status = eUART_DBG_OK;

	UART_DBG_ASSERT( NULL != str );

	if	(	( true == gb_is_init ) 
		&&	( NULL != str ))
	{
		while( NULL != *str )
		{
			// TODO: Timeout event!
			// TODO: Check return code
			app_uart_put( *str );

			// Increment pointer
			str++;
		}
	}
	else
	{
		status = eUART_DBG_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive character from reception buffer
*
* @param[in] 	p_char	- Pointer to received character
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_dbg_status_t uart_dbg_get(char * const p_char)
{
	uart_dbg_status_t status = eUART_DBG_OK;

	UART_DBG_ASSERT( NULL != p_char );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_char ))
	{
		if ( NRF_SUCCESS != app_uart_get( p_char ))
		{
			status = eUART_DBG_ERROR;
		}
	}
	else
	{
		status = eUART_DBG_ERROR;
	}

	return status;
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
