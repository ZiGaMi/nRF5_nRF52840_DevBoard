// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      usb_cdc.c
*@brief     USB CDC class
*@author    Ziga Miklosic
*@date      20.11.2022
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
#include "string.h"

#include "usb_cdc.h"
#include "project_config.h"
#include "middleware/ring_buffer/src/ring_buffer.h"

// Debug communication port
#include "middleware/cli/cli/src/cli.h"

#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"


// Debugging
#include "gpio.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


/**
 * 	Enable/Disable debug mode
 *
 * 	@note	Disable in release!
 */
#define USB_CDC_DEBUG_EN                    ( 0 )

#ifndef DEBUG
    #undef USB_CDC_DEBUG_EN
    #define USB_CDC_DEBUG_EN 0
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == USB_CDC_DEBUG_EN )
	#define USB_CDC_DBG_PRINT( ... )        ( cli_printf( __VA_ARGS__ ))
#else
	#define USB_CDC_DBG_PRINT( ... )        { ; }

#endif

/**
 *		USB CDC asserts
 *
 * 	@note	Disable in release!
 */
 #define USB_CDC_ASSERT_EN                  ( 1 )

 #if ( USB_CDC_ASSERT_EN )
	#define USB_CDC_ASSERT(x)               { PROJECT_CONFIG_ASSERT(x) }
 #else
  #define USB_CDC_ASSERT)                   { ; }
 #endif

 #ifndef DEBUG
    #undef USB_CDC_ASSERT_EN
    #define USB_CDC_ASSERT_EN 0
#endif

/**
 *		USB CDC buffer size
 *
 *	Unit: byte
 */                     
#define USB_CDC_RX_BUF_SIZE                 ( 512 )  





 



#define USBD_POWER_DETECTION true




#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1




////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialization guards
 */
static bool gb_is_init = false;

/**
 *		Dummy reception buffers
 */
static uint8_t gu8_usb_cdc_rx_buf = 0;

/**
 * 	USB CDC Tx/Rx buffer space
 */
static uint8_t gu8_usb_cdc_rx_buffer[USB_CDC_RX_BUF_SIZE] = {0};

/**
 * 	USB CDC Rx buffer
 */
static p_ring_buffer_t 		g_rx_buffer = NULL;
const ring_buffer_attr_t 	g_rx_buffer_attr  = { 	.name 		= "USB CDC Rx Buf",
													.item_size 	= 1,
													.override 	= false,
													.p_mem 		= &gu8_usb_cdc_rx_buffer };


static volatile bool gb_tx_in_progress = false;
static volatile bool gb_is_port_open = false;


////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static usb_cdc_status_t usb_cdc_init_buffers        (void);
static void             usb_cdc_event_cdc_hndl      (app_usbd_class_inst_t const * p_inst, app_usbd_cdc_acm_user_event_t event);
static void             usbd_user_ev_handler        (app_usbd_event_type_t event);


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////


/**
 * 	Create USB CDC Handler
 */
APP_USBD_CDC_ACM_GLOBAL_DEF(    gh_usb_cdc, 
                                usb_cdc_event_cdc_hndl,
                                CDC_ACM_COMM_INTERFACE,
                                CDC_ACM_DATA_INTERFACE,
                                CDC_ACM_COMM_EPIN,
                                CDC_ACM_DATA_EPIN,
                                CDC_ACM_DATA_EPOUT,
                                APP_USBD_CDC_COMM_PROTOCOL_AT_V250 );


////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize USB  buffers
*
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static usb_cdc_status_t usb_cdc_init_buffers(void)
{
	usb_cdc_status_t status = eUSB_CDC_OK;

	// Init Rx buffer
	if ( eRING_BUFFER_OK != ring_buffer_init( &g_rx_buffer, USB_CDC_RX_BUF_SIZE, &g_rx_buffer_attr ))
	{
		status = eUSB_CDC_ERROR;
	}

	return status;
}


static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            //bsp_board_led_off(LED_USB_RESUME);
            break;

        case APP_USBD_EVT_DRV_RESUME:
            //bsp_board_led_on(LED_USB_RESUME);
            break;

        case APP_USBD_EVT_STARTED:
            break;

        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
           // bsp_board_leds_off();
            break;

        case APP_USBD_EVT_POWER_DETECTED:
            USB_CDC_DBG_PRINT("USB_CDC: USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;

        case APP_USBD_EVT_POWER_REMOVED:
            USB_CDC_DBG_PRINT("USB_CDC: USB power removed");
            app_usbd_stop();

            gb_is_port_open = false;
            break;

        case APP_USBD_EVT_POWER_READY:
            USB_CDC_DBG_PRINT("USB_CDC: USB ready");
            app_usbd_start();
            break;

        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*		USB CDC class event handler @ref app_usbd_cdc_acm_user_ev_handler_t
*
* @param[in]    p_inst  - USB Device class instance
* @param[in]    event   - Event that raise handler
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static void usb_cdc_event_cdc_hndl(app_usbd_class_inst_t const * p_inst, app_usbd_cdc_acm_user_event_t event)
{
    switch ( event )
    {
        // User event on port open
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
			// Dummy read
			app_usbd_cdc_acm_read( &gh_usb_cdc, &gu8_usb_cdc_rx_buf, 1 );

            // Raise port open flag
			gb_is_port_open = true;

            // Raise callback
            usb_cdc_port_open_cb();

            // Debug
            USB_CDC_DBG_PRINT("USB_CDC: USB port open!");
		
            break;
        }

        // User event on port close
        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:

            // Clear port open flag
			gb_is_port_open = false;

            // Raise callback
            usb_cdc_port_close_cb();

            // Debug
            USB_CDC_DBG_PRINT("USB_CDC: USB port closed!");

            break;

        // User event on transmission complete
        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
	
            // Clear tx in progress flag
			gb_tx_in_progress = false;
		
			break;

        // User event on reception complete
        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        			
			// Take all bytes from reception buffer
			do
			{
				// Store rx char to buffer
				if ( eRING_BUFFER_OK == ring_buffer_add( g_rx_buffer, &gu8_usb_cdc_rx_buf ))
				{
					// No actions...
				}
				else
				{
					// TODO: handle error
				}

			} while ( NRF_SUCCESS == app_usbd_cdc_acm_read( &gh_usb_cdc, &gu8_usb_cdc_rx_buf, 1 ));

            break;
        

        default:
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
*@addtogroup UART_API
* @{ <!-- BEGIN GROUP -->
*
* 	Following function are part or uart API.
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialization of USB CDC 
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
usb_cdc_status_t usb_cdc_init(void)
{
	usb_cdc_status_t status = eUSB_CDC_OK;

	if ( false == gb_is_init )
	{
	
		// Init Rx/Tx buffers
		status |= usb_cdc_init_buffers();



		uint32_t ret;
		ret = nrf_drv_clock_init();
	    APP_ERROR_CHECK(ret);
    
	    nrf_drv_clock_lfclk_request(NULL);

	    while(!nrf_drv_clock_lfclk_is_running())
	    {
	        /* Just waiting */
	    }

	
		static const app_usbd_config_t usbd_config = {
	        .ev_state_proc = usbd_user_ev_handler
	    };


		app_usbd_serial_num_generate();
	    app_usbd_init(&usbd_config);


	    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get( &gh_usb_cdc );

	    app_usbd_class_append(class_cdc_acm);


	
	    if (USBD_POWER_DETECTION)
	    {
	        app_usbd_power_events_enable();
	    }
	    else
	    {
	        app_usbd_enable();
	        app_usbd_start();
	    }


		

		// Init success
		if ( eUSB_CDC_OK == status )
		{
			gb_is_init = true;
		}
	}
	else
	{
		status = eUSB_CDC_ERROR;
	}

	return status;
}



usb_cdc_status_t usb_cdc_hndl(void)
{
	usb_cdc_status_t status = eUSB_CDC_OK;

	if ( true == gb_is_init )
	{
		while (app_usbd_event_queue_process())
		{
		/* Nothing to do */
		}
	}
	return status;
}



////////////////////////////////////////////////////////////////////////////////
/**
*		Transmit over USB CDC 
*	
* @note This function is blocking
*
* @param[in] 	str     - String to be send
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
usb_cdc_status_t usb_cdc_write(const char* str)
{
	usb_cdc_status_t status = eUSB_CDC_OK;
    
	USB_CDC_ASSERT( true == gb_is_init );
	USB_CDC_ASSERT( NULL != str );

    if  (   ( true == gb_is_init )
        &&  ( NULL != str ))
    {
        // Is VCP open?
    	if ( true == gb_is_port_open )
    	{
            // Raise tx in progress flag
    		gb_tx_in_progress = true;

            // Start transmission
    		app_usbd_cdc_acm_write( &gh_usb_cdc, str, strlen(str));

            /*
             *  Wait until transmission is over, where "APP_USBD_CDC_ACM_USER_EVT_TX_DONE" event is
             *  raised.
             * 
             *  Check also if port is still open as it might happend that during transmission USB 
             *  cable is removed!
             */
    		while ( true == gb_tx_in_progress && true == gb_is_port_open )
    		{
    			// TODO: Implement timeout...

    			usb_cdc_hndl();
    		}
    	}
    }
    else
    {
        status = eUSB_CDC_ERROR;
    }

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive character from USB CDC
*
* @note This function is non-blocking
*
* @param[in] 	p_char	- Pointer to received character
* @return 		status	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
usb_cdc_status_t usb_cdc_get(char * const p_char)
{
	usb_cdc_status_t status = eUSB_CDC_OK;

	USB_CDC_ASSERT( true == gb_is_init );
	USB_CDC_ASSERT( NULL != p_char );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_char ))
	{
		// Get from buffer
		if ( eRING_BUFFER_OK != ring_buffer_get( g_rx_buffer, p_char ))
		{
			status = eUSB_CDC_ERROR;
		}
	}
	else
	{
		status = eUSB_CDC_ERROR;
	}
	
	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		USB cable plugged event callback 
*
* @return 	void
*/
////////////////////////////////////////////////////////////////////////////////
__attribute__((weak)) void usb_cdc_plugged_cb(void)
{
	/**
	 * 	Leave empty for user application purposes...
	 */
}

////////////////////////////////////////////////////////////////////////////////
/**
*		USB cable un-plugged event callback 
*
* @return 	void
*/
////////////////////////////////////////////////////////////////////////////////
__attribute__((weak)) void usb_cdc_unplugged_cb(void)
{
	/**
	 * 	Leave empty for user application purposes...
	 */
}

////////////////////////////////////////////////////////////////////////////////
/**
*		USB port open event callback 
*
* @return 	void
*/
////////////////////////////////////////////////////////////////////////////////
__attribute__((weak)) void usb_cdc_port_open_cb(void)
{
	/**
	 * 	Leave empty for user application purposes...
	 */
}

////////////////////////////////////////////////////////////////////////////////
/**
*		USB port close event callback 
*
* @return 	void
*/
////////////////////////////////////////////////////////////////////////////////
__attribute__((weak)) void usb_cdc_port_close_cb(void)
{
	/**
	 * 	Leave empty for user application purposes...
	 */
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
