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


#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"

#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


#define USBD_POWER_DETECTION true



static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);

#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1


/**
 * @brief CDC_ACM class instance
 * */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250
);




////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static void usbd_user_ev_handler(app_usbd_event_type_t event);


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
            //NRF_LOG_INFO("USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;

        case APP_USBD_EVT_POWER_REMOVED:
            //NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;

        case APP_USBD_EVT_POWER_READY:
           // NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;

        default:
            break;
    }
}


/**
 * @brief User event handler @ref app_usbd_cdc_acm_user_ev_handler_t (headphones)
 * */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
		
           // bsp_board_led_on(LED_CDC_ACM_OPEN);

            /*Setup first transfer*/
            //ret_code_t ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
             //                                      m_rx_buffer,
              //                                     READ_SIZE);
            //UNUSED_VARIABLE(ret);

		
            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
            //bsp_board_led_off(LED_CDC_ACM_OPEN);
            break;

        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
            //bsp_board_led_invert(LED_CDC_ACM_TX);
            break;

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
           
		   #if 0
		    ret_code_t ret;
            NRF_LOG_INFO("Bytes waiting: %d", app_usbd_cdc_acm_bytes_stored(p_cdc_acm));
            do
            {
                /*Get amount of data transfered*/
                size_t size = app_usbd_cdc_acm_rx_size(p_cdc_acm);
                NRF_LOG_INFO("RX: size: %lu char: %c", size, m_rx_buffer[0]);

                /* Fetch data until internal buffer is empty */
                ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
                                            m_rx_buffer,
                                            READ_SIZE);
            } while (ret == NRF_SUCCESS);

            bsp_board_led_invert(LED_CDC_ACM_RX);
			#endif 


            break;
        }
        default:
            break;
    }
}






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

	// ____________________________________________________________________________________________________
    
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


    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get( &m_app_cdc_acm );

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


	// ____________________________________________________________________________________________________


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

			while (app_usbd_event_queue_process())
			{
				/* Nothing to do */
			}
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

			app_usbd_cdc_acm_write( &m_app_cdc_acm, "Hello World\r", 12);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

