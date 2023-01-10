// Copyright (c) 2023 Ziga Miklosic
// All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
/**
*@file      ble_p.c
*@brief     Bluetooth Peripheral drivers
*@author    Ziga Miklosic
*@date      10.01.2023
*@version   V1.0.0
*
*@note      This file shall be in following directory:
*           
*               /drivers/peripheral/ble
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup BLE_P
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include "project_config.h"

#include "ble_p.h"
#include "middleware/ring_buffer/src/ring_buffer.h"




#include "app_timer.h"

// For softdevice
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

// BLE queue manager to stack
#include "nrf_ble_qwr.h"

// GATT
#include "nrf_ble_gatt.h"

// For advertisement
#include "ble_advertising.h"
 
 // For connection parameters
#include "ble_conn_params.h"


// TODO: Remove LED FOR DEBUG
#include "drivers/hmi/led/led/src/led.h"




// Debug COM port
#include "middleware/cli/cli/src/cli.h"


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *      BLE Peripheral data
 */
typedef struct
{
    uint16_t conn_handle;       /**< Connection Handle on which event occurred */


} ble_p_data_t;


/**
 *		BLE Peripheral reception buffer size
 *
 *	Unit: byte
 */                     
#define BLE_P_RX_BUF_SIZE                   ( 512 )  

/**
 * 	Enable/Disable debug mode
 *
 * 	@note	Disable in release!
 */
#define BLE_P_DEBUG_EN						( 1 )

#ifndef DEBUG
    #undef BLE_P_DEBUG_EN
    #define BLE_P_DEBUG_EN 0
#endif

/**
 *  Enable/Disable assertion
 *
 *  @note   Disble in release!
 */
 #define BLE_P_ASSERT_EN                    ( 1 )

#ifndef DEBUG
    #undef PAR_CFG_ASSERT_EN
    #define PAR_CFG_ASSERT_EN 0
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == BLE_P_DEBUG_EN )
	#define BLE_P_DBG_PRINT( ... )			( cli_printf( __VA_ARGS__ ))
#else
	#define BLE_P_DBG_PRINT( ... )			{ ; }

#endif

/**
 *  Assertion definition
 */
 #if ( BLE_P_ASSERT_EN )
	#define BLE_P_ASSERT(x)                 { PROJECT_CONFIG_ASSERT(x) }
 #else
  #define BLE_P_ASSERT)                     { ; }
 #endif



















/**
 *      BLE Event priority
 */
#define BLE_EVENT_PRIORITY              3 

/**
 *      BLE connection tag
 */ 
#define BLE_CONN_CFG_TAG                ( 1 )


/******************************************************************
 *      GAP RELATED
 ******************************************************************/

/**
 *      Device name
 */
#define GAP_DEVICE_NAME                     ( "My BLE Device" )

/**
 *      Minimum connection interval
 *
 *  Unit: ms
 */
#define GAP_MIN_CONN_INTERVAL_MS            ( 100 ) 

/**
 *      Maximum connection interval
 *
 *  Unit: ms
 */
#define GAP_MAX_CONN_INTERVAL_MS            ( 200 ) 

/**
 *      Slave latency
 */
#define GAP_SLAVE_LATENCY                   ( 0 )

/**
 *      Suppervsision timeout
 *
 * @note    If device is not responding within that time the connection
 *          will be terminated!
 *
 *  Unit: ms
 */
#define GAP_SUPERVISION_TIMEOUT_MS          ( 4000 )



/******************************************************************
 *      ADVERTISEMENT
 ******************************************************************/

 /**
  *     Advertisement inteval
  * 
  * @note   In units of 0.625 ms. E.g. value of 300 corresponds to 187.5 ms.
  *
  * @note   Place time in ms as it will be re-calculated later into proper values!
  *
  *     Unit: ms
  */
#define GAP_ADV_INTERVAL_MS                 ( 200.0 )

/**
 *      Advertisement duration
 *
 * @note    In units of 10 milliseconds. Anyway place time in ms as it 
 *          will be re-calculated later into proper values!
 *
 * @note    Set to 0 if wanted continous advertisement!  
 *   
 *  Unit: ms
 */
#define GAP_ADV_DURATION_MS                 ( 0 )


/******************************************************************
 *      CONNECTIONS
 ******************************************************************/

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        3                                       /**< Number of attempts before giving up the connection parameter negotiation. */





////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 *      Initialization guards
 */
static bool gb_is_init = false;

/**
 *      Reception buffer space
 */
static uint8_t gu8_ble_rx_buffer[BLE_P_RX_BUF_SIZE] = {0};

/**
 *      UART Rx buffer
 */
static p_ring_buffer_t 		g_rx_buf = NULL;
const ring_buffer_attr_t 	g_rx_buf_attr = 
{ 	
    .name 		= "BLE P Rx Buf",
    .item_size 	= sizeof(uint8_t),
    .override 	= false,
    .p_mem 		= &gu8_ble_rx_buffer 
};

/**
 *      BLE Peripheral data
 */
static ble_p_data_t g_ble_p = 
{ 
    .conn_handle = BLE_CONN_HANDLE_INVALID,

};
    




// For single connected device
NRF_BLE_QWR_DEF( m_qwr );

// For multi connected devices
//NRF_BLE_QWRS_DEF()

// GATT instance
NRF_BLE_GATT_DEF( m_gatt );

// Advertising instance
BLE_ADVERTISING_DEF( m_advertising );


// TODO: Check if can be omited!
//static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;


#define ADV_DATA_SIZE       ( 15 )
static uint8_t my_adv_data[ADV_DATA_SIZE] = { 0 };


// Available services as it will be part of advertisement packet
static ble_uuid_t m_adv_uuids[] = 
{ 
    {   .uuid = BLE_UUID_HEALTH_THERMOMETER_SERVICE,    .type = BLE_UUID_TYPE_BLE  },   // SIG standard service
    {   .uuid = BLE_UUID_BATTERY_SERVICE,               .type = BLE_UUID_TYPE_BLE  },   // SIG standard service
};



////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////

static void ble_evt_hndl(ble_evt_t const * p_ble_evt, void * p_context);


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////


/**
 *      ERROR HANDLER
 */
static void error_handler(void)
{
    PROJECT_CONFIG_ASSERT(0);
}


// Init BLE
static ret_code_t ble_stack_init(void)
{
    ret_code_t status = NRF_SUCCESS;
    
    // Start SoftDevice
    if ( NRF_SUCCESS != nrf_sdh_enable_request())
    {
        // Further actions on error...
        error_handler();
    }

    // Application RAM start address
    uint32_t ram_start = 0;

    // Configure BLE stack
    if ( NRF_SUCCESS != nrf_sdh_ble_default_cfg_set( BLE_CONN_CFG_TAG, &ram_start ))
    {
        // Further actions on error... 
        error_handler();
    }

    // Enable BLE stack
    if ( NRF_SUCCESS != nrf_sdh_ble_enable( &ram_start ))
    {
        // Further actions on error... 
        error_handler();
    }
    
    // Register BLE event callback
    // ZIGA: An observer is essentially a piece of code that listens for events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, BLE_EVENT_PRIORITY, ble_evt_hndl, NULL);

    return status;
}



// Init GAP
static ret_code_t gap_init(void)
{
    ret_code_t                  status          = NRF_SUCCESS;
    ble_gap_conn_params_t       gap_conn_params = {0};
    ble_gap_conn_sec_mode_t     security_mode   = {0};

    // Use no security
    BLE_GAP_CONN_SEC_MODE_SET_OPEN( &security_mode );

    // Set the device name
    if ( NRF_SUCCESS != sd_ble_gap_device_name_set( &security_mode, (const uint8_t*) GAP_DEVICE_NAME, strlen(GAP_DEVICE_NAME)))
    {
        // Further actions on error... 
        error_handler();
    }

    // Set the apperance
    // BLE_APPEARANCE_GENERIC_COMPUTER
    if ( NRF_SUCCESS != sd_ble_gap_appearance_set( BLE_APPEARANCE_GENERIC_GLUCOSE_METER ))  
    {
        // Further actions on error... 
        error_handler();
    }

    // Set up GAP
    gap_conn_params.min_conn_interval   = MSEC_TO_UNITS( GAP_MIN_CONN_INTERVAL_MS, UNIT_1_25_MS );
    gap_conn_params.max_conn_interval   = MSEC_TO_UNITS( GAP_MAX_CONN_INTERVAL_MS, UNIT_1_25_MS );
    gap_conn_params.slave_latency       = GAP_SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout    = MSEC_TO_UNITS( GAP_SUPERVISION_TIMEOUT_MS, UNIT_10_MS );

    // Set the PPCP - Peripheral Prefered Connection Parameters
    if ( NRF_SUCCESS != sd_ble_gap_ppcp_set( &gap_conn_params ))
    {
        // Further actions on error... 
        error_handler();        
    }

    return status;
}


// Init GATT
static ret_code_t gatt_init(void)
{
    ret_code_t status = NRF_SUCCESS;

    if ( NRF_SUCCESS != nrf_ble_gatt_init( &m_gatt, NULL ))
    { 
        // Further actions on error... 
        error_handler();   
    }

    return status;
}



// Advertisement handler
static void adv_evt_hndl(ble_adv_evt_t ble_adv_evt)
{

    switch (ble_adv_evt)
    {
        // Advertisement START
        case BLE_ADV_EVT_FAST:
           
            //bsp_board_led_on( BSP_BOARD_LED_0 );

            break;
        
        // Advertisement STOP
        case BLE_ADV_EVT_IDLE:

            //bsp_board_led_off( BSP_BOARD_LED_0 );

            break;

        default:
            break;
    }
}

// Init advertisment
static ret_code_t advertisement_init(void)
{
    ret_code_t              status   = NRF_SUCCESS;
    ble_advertising_init_t  adv_init = {0};

    // Set up advertising parameters
    adv_init.advdata.name_type              = BLE_ADVDATA_FULL_NAME;
    adv_init.advdata.include_appearance     = true;
    adv_init.advdata.flags                  = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    adv_init.config.ble_adv_fast_enabled    = true;
    adv_init.config.ble_adv_fast_interval   = (uint32_t) ( GAP_ADV_INTERVAL_MS / 0.625f );
    adv_init.config.ble_adv_fast_timeout    = (uint32_t) ( GAP_ADV_DURATION_MS / 10.0f );
    adv_init.evt_handler                    = adv_evt_hndl;


    
    /****************************************************************
     *  SHOWING TX POWER LEVEL
     */
    
    int8_t tx_level = 1;
    adv_init.advdata.p_tx_power_level = &tx_level;

    /****************************************************************
     *  SHOWING AVAILABLE SERVICES IN ADVERTISEMENT PACKETS
     */
    
    // Show also available services in advertisement packet
    adv_init.advdata.uuids_complete.p_uuids = (ble_uuid_t*) &m_adv_uuids;
    adv_init.advdata.uuids_complete.uuid_cnt = 2;



    /****************************************************************
    *  ADDING SERVICE DATA TO ADVERTISEMENT PACKET
    */
    #if 0
    uint8_t                        bat_level       = 12;
    ble_advdata_service_data_t     service_data    = {0};

    // Set up service data
    service_data.service_uuid      = BLE_UUID_BATTERY_SERVICE;
    service_data.data.size         = sizeof( bat_level);
    service_data.data.p_data       = (uint8_t*) &bat_level;

    // Add to advertisement data
    adv_init.advdata.p_service_data_array   = &service_data;
    adv_init.advdata.service_data_count     = 1;


    #endif


    /****************************************************************
     *  SETTING UP MANUFACTURER DATA INSIDE ADVERTISEMENT PACKET
     */
    
    #if 0
        ble_advdata_manuf_data_t man_data =
        {
            .company_identifier = 0x00ff,
            .data = 
            {
                .p_data = (uint8_t*) &my_adv_data,
                .size = ADV_DATA_SIZE,
            }
        };

        adv_init.advdata.p_manuf_specific_data = &man_data;
    #endif

        
        
        // Set Long Range Advertisement PHY
        //adv_init.config.ble_adv_primary_phy     = BLE_GAP_PHY_CODED;
        //adv_init.config.ble_adv_secondary_phy   = BLE_GAP_PHY_CODED;
        


    /****************************************************************
     *  SETTING UP CONNECTION INTERVAL RANGE
     */
    
    ble_advdata_conn_int_t slave_conn_int = 
    {
        .min_conn_interval = (uint16_t)( 100 / 1.25 ),        /**< Minimum connection interval, in units of 1.25 ms, range 6 to 3200 (7.5 ms to 4 s). */
        .max_conn_interval = (uint16_t)( 200 / 1.25 ),         /**< Maximum connection interval, in units of 1.25 ms, range 6 to 3200 (7.5 ms to 4 s). The value 0xFFFF indicates no specific maximum. */
    };

    adv_init.advdata.p_slave_conn_int = &slave_conn_int;






    // Init advertisement
    if ( NRF_SUCCESS != ble_advertising_init( &m_advertising, &adv_init ))
    {
        // Further actions on error... 
        error_handler(); 
    }
    
    // Setup advertising config tag
    ble_advertising_conn_cfg_tag_set( &m_advertising, BLE_CONN_CFG_TAG );

    return status;
}



static void advertisement_start(void)
{
    if ( NRF_SUCCESS != ble_advertising_start( &m_advertising, BLE_ADV_MODE_FAST ))
    {
        // Further actions on error... 
        error_handler();  
    }
}



static void nrf_qwr_error_hndl(uint32_t nrf_error)
{
    // Handler errors here...

}

// Init services
static ret_code_t services_init(void)
{
    ret_code_t          status      = NRF_SUCCESS;
    nrf_ble_qwr_init_t  qwr_init    = { 0 };

    // Register error hander
    qwr_init.error_handler = nrf_qwr_error_hndl;

    // Init QWR
    if ( NRF_SUCCESS != nrf_ble_qwr_init( &m_qwr, &qwr_init ))
    {
        // Further actions on error... 
        error_handler();    
    }

    return status;
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *
 * @note    All this function does is to disconnect. This could have been done by simply
 *          setting the disconnect_on_fail config parameter, but instead we use the event
 *          handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect( g_ble_p.conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE );
        
        // TODO: 
        //APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    // TODO: 
   // APP_ERROR_HANDLER(nrf_error);
}


// Init connection parameters
static ret_code_t conn_pars_init(void)
{
    ret_code_t              status  = NRF_SUCCESS;
    ble_conn_params_init_t  cp_init = {0};


    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    if ( NRF_SUCCESS != ble_conn_params_init( &cp_init ))
    {
        // Further actions on error... 
        error_handler();    
    }

    return status;
}



static ble_gatts_char_handles_t    char_handles    = {0};

#define THERMOSTAT_SERVICE_BASE                                                \
  {                                                                            \
    0x2A, 0x94, 0x01, 0x0a, 0x34, 0xc4, 0x3e, 0xbc, 0xe5, 0x4b, 0xb3, 0x8b,    \
        0x00, 0x00, 0x9d, 0xaf                                                 \
  }

#define THERMOSTAT_SERVICE_UUID   0x1000


#define BLE_THERMOSTAT_RX_CHARACTERISTIC   0x1001
#define BLE_THERMOSTAT_TX_CHARACTERISTIC   0x1002

static ble_gatts_char_handles_t    rx_char_handles    = {0};
static ble_gatts_char_handles_t    tx_char_handles    = {0};

static void custom_service_init(void)
{
    ble_uuid_t  hrs_uuid    = {0};
    uint16_t    hrs_handle  = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    // Assing UUID 
    //BLE_UUID_BLE_ASSIGN( hrs_uuid, BLE_UUID_HEART_RATE_SERVICE );
    
    // Or EQUIVALENT
    hrs_uuid.type = BLE_UUID_TYPE_BLE;
    hrs_uuid.uuid = BLE_UUID_HEART_RATE_SERVICE;
    ///////////////////////////////////////////////////////////////////////////////////


    // Register service
    if ( NRF_SUCCESS != sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY, &hrs_uuid, &hrs_handle ))
    {
        // Further actions on error... 
        error_handler();  
    }

    
    ///////////////////////////////////////////////////////////////////////////////////
    // Add characteristics to service
    ble_add_char_params_t       char_params     = {0};


    uint8_t init_data[5] = { 0, 1, 2, 3, 4 };


    char_params.uuid                = BLE_UUID_HEART_RATE_MEASUREMENT_CHAR;
    char_params.max_len             = 20;
    char_params.init_len            = 5;
    char_params.is_var_len          = true;
    char_params.char_props.notify   = 1;
    char_params.cccd_write_access   = SEC_OPEN;
    char_params.p_init_value        = (uint8_t*) &init_data;



    // Register characteristics
    if ( NRF_SUCCESS != characteristic_add( hrs_handle, &char_params, &char_handles ))
    {
        // Further actions on error... 
        error_handler();  
    }





    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //          HOW TO MAKE CUSTOM SERVICE AND CHARACTERISTICS ???????????????

    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = THERMOSTAT_SERVICE_BASE;
    uint16_t            custom_service_handle  = 0;


    char_uuid.uuid      = THERMOSTAT_SERVICE_UUID;

    if ( NRF_SUCCESS != sd_ble_uuid_vs_add( &base_uuid, &char_uuid.type ))
    {
        // Further actions on error... 
        error_handler();  
    }

    // Register service
    if ( NRF_SUCCESS != sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY, &char_uuid, &custom_service_handle ))
    {
        // Further actions on error... 
        error_handler();  
    }


    // Add RX characteristics
    ble_add_char_params_t rx_char_params = {0};

    rx_char_params.uuid                     = BLE_THERMOSTAT_RX_CHARACTERISTIC;
    rx_char_params.uuid_type                = char_uuid.type;
    rx_char_params.init_len                 = sizeof(uint8_t);
    rx_char_params.max_len                  = 250;
    rx_char_params.is_var_len               = true;
    rx_char_params.char_props.write         = 1;
    rx_char_params.char_props.write_wo_resp = 1;
    rx_char_params.read_access              = SEC_OPEN;
    rx_char_params.write_access             = SEC_OPEN;
    rx_char_params.cccd_write_access        = SEC_OPEN;

    // Register characteristics
    if ( NRF_SUCCESS != characteristic_add( custom_service_handle, &rx_char_params, &rx_char_handles ))
    {
        // Further actions on error... 
        error_handler();  
    }

    
    // Add Tx characteristics
    ble_add_char_params_t tx_char_params = {0};

    tx_char_params.uuid                     = BLE_THERMOSTAT_TX_CHARACTERISTIC;
    tx_char_params.uuid_type                = char_uuid.type;
    tx_char_params.init_len                 = sizeof(uint8_t);
    tx_char_params.max_len                  = 250;
    tx_char_params.is_var_len               = true;
    tx_char_params.char_props.write         = 0;
    tx_char_params.char_props.write_wo_resp = 0;
    tx_char_params.char_props.read          = 0;
    tx_char_params.char_props.notify        = 1;
    tx_char_params.read_access              = SEC_OPEN;
    tx_char_params.write_access             = SEC_OPEN;
    tx_char_params.cccd_write_access        = SEC_OPEN;

    // Register characteristics
    if ( NRF_SUCCESS != characteristic_add( custom_service_handle, &tx_char_params, &tx_char_handles))
    {
        // Further actions on error... 
        error_handler();  
    }

}





/**
 *      BLE HANDLER
 */
static void ble_evt_hndl(ble_evt_t const * p_ble_evt, void * p_context)
{
    // Handle BLE events
    switch( p_ble_evt->header.evt_id )
    {
        /**
         *      Disconnected from peer event
         */
        case BLE_GAP_EVT_DISCONNECTED:

            // Connection lost
            g_ble_p.conn_handle = BLE_CONN_HANDLE_INVALID;
        
            // Status
           // bsp_board_led_off( BSP_BOARD_LED_1 );
            led_set( eLED_2, eLED_OFF );

            // TODO: Raise callback on disconnect


            break;

        /**
         *      Connected to peer event
         */
        case BLE_GAP_EVT_CONNECTED:

            // Assing connection info to handle
            g_ble_p.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            
            // TODO: Check if needed this call!
            if ( NRF_SUCCESS != nrf_ble_qwr_conn_handle_assign( &m_qwr, g_ble_p.conn_handle ))
            {
                error_handler();
            }

            led_set( eLED_2, eLED_ON );


            // TODO: Raise callback on connect

            break;


        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            
            const ble_gap_phys_t phys = 
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };

            if ( NRF_SUCCESS != sd_ble_gap_phy_update( p_ble_evt->evt.gap_evt.conn_handle, &phys))
            {
                error_handler();
            }

            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            ret_code_t err_code = sd_ble_gatts_sys_attr_set( g_ble_p.conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;


        case BLE_GATTS_EVT_WRITE:



            // Client write to RX characteristics?
            if ( p_ble_evt->evt.gatts_evt.params.write.handle == rx_char_handles.value_handle )
            {
               
                // Get data and lenght
                const uint8_t * p_data  = p_ble_evt->evt.gatts_evt.params.write.data;
                const uint16_t len      = p_ble_evt->evt.gatts_evt.params.write.len;

                // Debug message
                BLE_P_DBG_PRINT( "BLE_P: Rx event! (len: %d)", len );

                // Add to RX fifo
                for (uint16_t idx = 0; idx < len; idx++)
                {
                    if ( eRING_BUFFER_OK != ring_buffer_add( g_rx_buf, (uint8_t*) &p_data[idx] ))
                    {
                        // Buffer overflow!
                        BLE_P_DBG_PRINT( "BLE_P: Rx buffer overflow! Increse buffer size via \"BLE_P_RX_BUF_SIZE\" macro!");
                        
                        break;
                    }
                }
            }

        break;

        default:
            // No actions...
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
*@addtogroup BLE_P_API
* @{ <!-- BEGIN GROUP -->
*
* 	Following function are part of BLE Peripheral API.
*/
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral Initialize
*
* @brief    BLE peripheral module creates custom service and with two 
*           characteristics:
*
*               1. TX Characteristics: for pushing data to client
*               2. RX Characteristics: for receiveing data from client
*
*           Basic client-server model:
*
*                           write to RX characteristic
*               CLIENT -----------------------------------> SERVER
*        (central BLE device)                        (peripheral BLE device)
*
*
*                        notification to TX characteristic
*               CLIENT <----------------------------------- SERVER
*        (central BLE device)                        (peripheral BLE device)
*
*
*           Implemented communication sheme is emulating simple UART interface.
*
*       
* @note     TX Characteristics is defined as notification, therefore client
*           must first enable CCCD (Client Characteristics Configuration Descriptor)
*           before it can receive notifications!
*
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_init(void)
{
    ble_p_status_t status = eBLE_P_OK;

    // Init Rx buffer
	if ( eRING_BUFFER_OK != ring_buffer_init( &g_rx_buf, BLE_P_RX_BUF_SIZE, &g_rx_buf_attr ))
	{
		status = eBLE_P_ERROR;
	}


    // Init BLE stuff
    ble_stack_init();
    gap_init();
    gatt_init();
    
    advertisement_init();
    //advertisement_2_init();
    
    services_init();
    conn_pars_init(); 


    custom_service_init();


    // Start advertising     
    advertisement_start();


    // Init success
    if ( eBLE_P_OK == status )
    {
        gb_is_init = true;
    }



    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral get init status
*
* @param[out] 	p_is_conn   - Connection status, true if initiated
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_is_init (bool * const p_is_init)
{
    ble_p_status_t status = eBLE_P_OK;

    if ( NULL != p_is_init )
    {
        *p_is_init = gb_is_init;
    }
    else
    {
        status = eBLE_P_ERROR;
    }

    return status;  
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral get connection status
*
* @param[out] 	p_is_conn   - Connection status, true if connected
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_is_connected(bool * const p_is_conn)
{
    ble_p_status_t status = eBLE_P_OK;

    if ( NULL != p_is_conn )
    {
        if ( g_ble_p.conn_handle == BLE_CONN_HANDLE_INVALID )
        {
            *p_is_conn = false;
        }
        else
        {
            *p_is_conn = true; 
        }
    }
    else
    {
        status = eBLE_P_ERROR;
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral write 
*	
* @note     This function push data to GATTS Tx characteristics.
*
* @note     Tx characteristics is defined as server-initiated update with
*           "Notification" type. 
*
* @note     This function shall not be called if connection is not established!
*
* @param[in] 	p_data      - Pointer to data for transmit
* @param[in] 	len         - Lenght of data in bytes
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_write(const uint8_t * const p_data, const uint16_t len)
{
    ble_p_status_t status = eBLE_P_OK;
    bool            is_connected    = false;

    BLE_P_ASSERT( true == gb_is_init );
	BLE_P_ASSERT( NULL != p_data );
	BLE_P_ASSERT( len > 0 );

    // Get connection status
    (void) ble_p_is_connected( &is_connected );

    BLE_P_ASSERT( true == is_connected );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_data )
        &&  ( len > 0 )
        &&  ( true == is_connected ))
	{
        // Prepare notification data
        const ble_gatts_hvx_params_t hvx_params_tx = 
        {
            .handle     = tx_char_handles.value_handle,
            .p_data     = (uint8_t*) p_data,
            .p_len      = (uint16_t*) &len,
            .type       = BLE_GATT_HVX_NOTIFICATION
        };

        // Push data to client via Notify
        if ( NRF_SUCCESS != sd_ble_gatts_hvx( g_ble_p.conn_handle, &hvx_params_tx ))
        {
            status = eBLE_P_ERROR;
        }
	}
	else
	{
		status = eBLE_P_ERROR;
	}

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral read
*	
* @note     This function retrives data from GATTS Rx characteristics.
*
* @note     Rx characteristics is defined as read-only, therefore only client
*           can write to characteristics value.
*
* @note     This function shall not be called if connection is not established!
*
* @note     Returned data is taken from local FIFO buffer storage. In case reception
*           FIFO is empty it return ERROR code, otherwise OK.
*
* @param[out] 	p_data      - Pointer to data for transmit
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_get(uint8_t * const p_data)
{
    ble_p_status_t  status          = eBLE_P_OK;
    bool            is_connected    = false;

    BLE_P_ASSERT( true == gb_is_init );
	BLE_P_ASSERT( NULL != p_data );

    // Get connection status
    (void) ble_p_is_connected( &is_connected );

    BLE_P_ASSERT( true == is_connected );

	if	(	( true == gb_is_init ) 
		&&	( NULL != p_data )
        &&  ( true == is_connected ))
	{
		// Get from buffer
		if ( eRING_BUFFER_OK != ring_buffer_get( g_rx_buf, p_data ))
		{
			status = eBLE_P_ERROR;
		}
	}
	else
	{
		status = eBLE_P_ERROR;
	}

    return status;
}














void ble_p_hndl(void)
{
    
    static uint8_t                encoded_hrm[5] = { 0, 1, 2, 3};
    uint16_t               len;
    uint16_t               hvx_len;
    ble_gatts_hvx_params_t hvx_params = {0};

    len     = 5;
    hvx_len = len;


    // To HRV characteristics
    hvx_params.handle = char_handles.value_handle;   
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &hvx_len;
    hvx_params.p_data = encoded_hrm;



    static uint8_t data[250] = {0};
    uint16_t len2 = 250;

    // ZIGA (10.01.2023): MTU is set to 250, effectively can exchange up to 244 bytes!
    data[0]++;
    data[243]++;

    ble_gatts_hvx_params_t hvx_params_tx = 
    {
        .handle     = tx_char_handles.value_handle,
        .p_data     = (uint8_t*) &data,
        .p_len      = &len2,
        .type       = BLE_GATT_HVX_NOTIFICATION
    };



    // Send value if connected and notifying
    if ( g_ble_p.conn_handle != BLE_CONN_HANDLE_INVALID )
    {

        encoded_hrm[4]++;
        encoded_hrm[0]--;

        // Send data
        ret_code_t status = NRF_SUCCESS;
        
        // Send
        status = sd_ble_gatts_hvx( g_ble_p.conn_handle, &hvx_params );

        //if ( NRF_SUCCESS != sd_ble_gatts_hvx( m_conn_handle, &hvx_params ))
        if ( NRF_SUCCESS != status )
        {
            // Further actions on error... 
            //error_handler();  

            //err_cnt++;
        }
        

        
        status = sd_ble_gatts_hvx( g_ble_p.conn_handle, &hvx_params_tx );

        if ( NRF_SUCCESS != status )
        {
            // Further actions on error... 
            //error_handler();  

            //err_cnt++;
        }
        

    }


    led_toggle( eLED_3 );
}





////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
