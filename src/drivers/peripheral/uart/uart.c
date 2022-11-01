// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      uart.c
*@brief     UART driver
*@author    Ziga Miklosic
*@date      01.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup UART
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "uart.h"
#include "pin_mapper.h"
#include "project_config.h"
#include "middleware/ring_buffer/src/ring_buffer.h"

#include "nrf_gpio.h"
#include "nrf_drv_uart.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


/**
 *		UARTE1 buffer size
 *
 *	Unit: byte
 */
#define UART_1_TX_BUF_SIZE			( 512)                         
#define UART_1_RX_BUF_SIZE			( 512 )  


/**
 *		Baudrate
 *
 * @note	Select one of the "nrf_uart_baudrate_t" enumeration options inside
 *			"nrf_uart.h" file.
 */  
#define UART_1_BAUDRATE				( NRF_UARTE_BAUDRATE_115200 )   

/**
 *		UART asserts
 */
 #define UART_ASSERT_EN				( 1 )

 #if ( UART_ASSERT_EN )
	#define UART_ASSERT(x)			{ PROJECT_CONFIG_ASSERT(x) }
 #else
  #define UART_ASSERTx)				{ ; }
 #endif


////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialization guards
 */
static bool gb_is_init = false;

/**
 *	UARTE Handlers
 */	
 static nrf_drv_uart_t gh_uart1_handler = NRF_DRV_UART_INSTANCE( 1 );

 /**
  *		Dummy reception buffers
  */
  static uint8_t gu8_uart1_rx_buf = 0;

  /**
 * 	UART Rx/Tx buffer space
 */
static uint8_t gu8_uart1_tx_buffer[UART_1_TX_BUF_SIZE] = {0};
static uint8_t gu8_uart1_rx_buffer[UART_1_RX_BUF_SIZE] = {0};

/**
 * 	UART Rx buffer
 */
static p_ring_buffer_t 		g_rx_buffer1 = NULL;
const ring_buffer_attr_t 	g_rx_buffer1_attr  = { 	.name 		= "Uart1 Rx Buf",
													.item_size 	= 1,
													.override 	= false,
													.p_mem 		= &gu8_uart1_rx_buffer };
/**
 * 	UART Tx buffer
 */
static p_ring_buffer_t 		g_tx_buffer1 = NULL;
const ring_buffer_attr_t 	g_tx_buffer1_attr  = { 	.name 		= "Uart1 Tx Buf",
													.item_size 	= 1,
													.override 	= false,
													.p_mem 		= &gu8_uart1_tx_buffer };

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static uart_status_t uart_1_init_buffers(void);


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/**
*		UART1 Event handler from interrupt
*
* @param[in]	p_event		- Event details
* @param[in]	p_context	- Context of event
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static void uart_1_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
	uint8_t u8_data = 0;

	// Reception buffer not empty
    if ( p_event->type == NRF_DRV_UART_EVT_RX_DONE )
    {
		if( p_event->data.rxtx.bytes > 0 )
		{
			// Get received char
			u8_data = p_event->data.rxtx.p_data[0];

			// Store rx char to buffer
			if ( eRING_BUFFER_OK == ring_buffer_add( g_rx_buffer1, &u8_data ))
			{
				// No actions...
			}
			else
			{
				// TODO: handle error
			}
		}

		// Dummy read
		(void) nrf_drv_uart_rx( &gh_uart1_handler, &gu8_uart1_rx_buf, 1 );
    }

	// Transmission buffer empty
    else if ( p_event->type == NRF_DRV_UART_EVT_TX_DONE )
    {
		// Send next char from tx buffer
		if ( eRING_BUFFER_OK == ring_buffer_get( g_tx_buffer1, &u8_data ))
		{
			nrf_drv_uart_tx( &gh_uart1_handler, &u8_data, 1 );
		}
    }

	// TODO: Check out what kind of error are being process here...
	else if (p_event->type == NRF_DRV_UART_EVT_ERROR)
    {

    }

	else
	{
		// No actions...
	}
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize UART1 Tx/Rx buffer
*
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static uart_status_t uart_1_init_buffers(void)
{
	uart_status_t status = eUART_OK;

	// Init Tx buffer
	if ( eRING_BUFFER_OK != ring_buffer_init( &g_tx_buffer1, UART_1_TX_BUF_SIZE, &g_tx_buffer1_attr ))
	{
		status = eUART_ERROR;
	}

	// Init Rx buffer
	if ( eRING_BUFFER_OK != ring_buffer_init( &g_rx_buffer1, UART_1_RX_BUF_SIZE, &g_rx_buffer1_attr ))
	{
		status = eUART_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialization of UART1
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_1_init(void)
{
	uart_status_t status = eUART_OK;
	
	if ( false == gb_is_init )
	{
		// Init buffers
		status |= uart_1_init_buffers();

		// Setup configuration
		nrf_drv_uart_config_t config = 
		{
			.pseltxd			= NRF_GPIO_PIN_MAP( UART_1_TX__PORT, UART_1_TX__PIN ),          
			.pselrxd			= NRF_GPIO_PIN_MAP( UART_1_RX__PORT, UART_1_RX__PIN ),        
			.pselcts			= NRF_UART_PSEL_DISCONNECTED,
			.pselrts			= NRF_UART_PSEL_DISCONNECTED,
			.p_context			= NULL,
			.hwfc				= NRF_UART_HWFC_DISABLED,    
			.parity				= NRF_UART_PARITY_EXCLUDED,
			.baudrate			= UART_1_BAUDRATE,
			.interrupt_priority	= 6,
			.use_easy_dma		= true,
		};
	
		// Init
		if ( NRF_SUCCESS != nrf_drv_uart_init( &gh_uart1_handler, &config, uart_1_event_handler ))
		{
			status = eUART_ERROR;
		}

		// Dummy read
		(void) nrf_drv_uart_rx( &gh_uart1_handler, &gu8_uart1_rx_buf, 1 );

		if ( eUART_OK == status )
		{
			gb_is_init = true;
		}
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		UART1 transmit
*	
* @note This function is non-blocking
*
* @param[in] 	pc_string	- String to be sended over UART
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_1_write(const char* str)
{
	uart_status_t status = eUART_OK;
	uint8_t u8_data = 0;

	UART_ASSERT( true == gb_is_init );
	UART_ASSERT( NULL != str );

	if	(	( true == gb_is_init ) 
		&&	( NULL != str ))
	{
		// Get lenght of string
		const uint32_t str_len = strlen( str );

		// Put all to fifo
		for ( uint32_t ch = 0; ch < str_len; ch++ )
		{
			// Put char to tx buffer
			if ( eRING_BUFFER_OK != ring_buffer_add( g_tx_buffer1, (const char*) str ))
			{
				// TODO: handle error if not all data added
				break;
			}

			// Move thru string
			str++;
		}

        // The new byte has been added to FIFO. It will be picked up from there
        // (in 'uart_event_handler') when all preceding bytes are transmitted.
        // But if UART is not transmitting anything at the moment, we must start
        // a new transmission here.
        if ( false == nrf_drv_uart_tx_in_progress( &gh_uart1_handler ))
        {
            // This operation should be almost always successful, since we've
            // just added a byte to FIFO, but if some bigger delay occurred
            // (some heavy interrupt handler routine has been executed) since
            // that time, FIFO might be empty already.
            
			// Start trasmission by sending first char from fifo
			if ( eRING_BUFFER_OK == ring_buffer_get( g_tx_buffer1, &u8_data ))
			{
				nrf_drv_uart_tx( &gh_uart1_handler, &u8_data, 1 );
			}
        }	
	}
	else
	{
		status = eUART_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive UART1 character from reception buffer
*
* @note This function is non-blocking
*
* @param[in] 	p_char	- Pointer to received character
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_1_get(char * const p_char)
{
	uart_status_t status = eUART_OK;

	UART_ASSERT( true == gb_is_init );
	UART_ASSERT( NULL != p_char );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_char ))
	{
		// Get from buffer
		if ( eRING_BUFFER_OK != ring_buffer_get( g_rx_buffer1, p_char ))
		{
			status = eUART_ERROR;
		}
	}
	else
	{
		status = eUART_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
