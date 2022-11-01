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

#include "nrf_gpio.h"
#include "nrf_drv_uart.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *		UARTE0 buffer size
 *
 *	Unit: byte
 */
#define UART_0_TX_BUF_SIZE			( 1024 )                         
#define UART_0_RX_BUF_SIZE			( 512 )   

/**
 *		UARTE1 buffer size
 *
 *	Unit: byte
 */
#define UART_1_TX_BUF_SIZE			( 1024 )                         
#define UART_1_RX_BUF_SIZE			( 1024 )  


/**
 *		Baudrate
 *
 * @note	Select one of the "nrf_uart_baudrate_t" enumeration options inside
 *			"nrf_uart.h" file.
 */
#define UART_0_BAUDRATE				( NRF_UARTE_BAUDRATE_115200 )   
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
 static nrf_drv_uart_t gh_uart0_handler = NRF_DRV_UART_INSTANCE( 0 );
 static nrf_drv_uart_t gh_uart1_handler = NRF_DRV_UART_INSTANCE( 1 );

 /**
  *		Dummy reception buffers
  */
  static uint8_t gu8_uart0_rx_buf = 0;
  static uint8_t gu8_uart1_rx_buf = 0;

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static uart_status_t uart_1_init(void);


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////



static void uart_1_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    if (p_event->type == NRF_DRV_UART_EVT_RX_DONE)
    {
		if( p_event->data.rxtx.bytes > 0 )
		{
			// TODO: REceive here
		}

		(void) nrf_drv_uart_rx( &gh_uart1_handler, &gu8_uart1_rx_buf, 1 );
    }
    else if (p_event->type == NRF_DRV_UART_EVT_ERROR)
    {

    }
    else if (p_event->type == NRF_DRV_UART_EVT_TX_DONE)
    {
		// TODO: Transmit here
    }
}


static uart_status_t uart_1_init(void)
{
	uart_status_t status = eUART_OK;

	// Setup configuration
	nrf_drv_uart_config_t config = 
	{
		.pseltxd			=  NRF_GPIO_PIN_MAP( UART_1_TX__PORT, UART_1_TX__PIN ),          
		.pselrxd			=  NRF_GPIO_PIN_MAP( UART_1_RX__PORT, UART_1_RX__PIN ),        
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

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialization of uart
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_init	(void)
{
	uart_status_t status = eUART_OK;
	
	if ( false == gb_is_init )
	{
		// Init both uarts
		//status |= uart_0_init();
		status |= uart_1_init();

		if ( eUART_OK == status )
		{
			gb_is_init = false;
		}
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		UART transmit
*
* @param[in]	inst		- UART instance
* @param[in] 	pc_string	- String to be sended over UART
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_write(const uart_inst_t inst, const char* str)
{
	uart_status_t status = eUART_OK;

	UART_ASSERT( NULL != str );
	UART_ASSERT( inst < eUART_NUM_OF );

	if	(	( NULL != str )
		&&	( inst < eUART_NUM_OF ))
	{
	
	}
	else
	{
		status = eUART_ERROR;
	}

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive character from reception buffer
*
* @param[in]	inst	- UART instance
* @param[in] 	p_char	- Pointer to received character
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
uart_status_t uart_get(const uart_inst_t inst, char * const p_char)
{
	uart_status_t status = eUART_OK;

	UART_ASSERT( NULL != p_char );
	UART_ASSERT( inst < eUART_NUM_OF );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_char )
		&&	( inst < eUART_NUM_OF  ))
	{

		// TODO: ..
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
