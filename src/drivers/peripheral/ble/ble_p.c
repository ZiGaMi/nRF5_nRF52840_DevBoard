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
*
*@description
*
*   This code contains simple on BLE peripheral device connection with
*   two characteristics. Both for advertisement and connection LE UNCODED
*   1M PHY is used.
*
*   After connection is established advertising is automatically stopped.
*   After connection is lost advertising is automatically stared.
*
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
#include <assert.h>
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
 *      List of BLE services
 */
typedef enum
{
    eBLE_P_SERVICE_SERIAL = 0,  /**<Serial service */
    eBLE_P_SERVICE_DEV_IFO,      /**<Device info service */

    eBLE_P_SERVICE_NUM_OF,
} ble_p_service_opt_t;

/**
 *      Serial service characteristics
 */
typedef enum
{
    eBLE_P_SER_CHAR_TX = 0,     /**<Tx characteristics */
    eBLE_P_SER_CHAR_RX,         /**<Rx characteristics */
    
    eBLE_P_SER_CHAR_NUM_OF,
} ble_p_ser_char_opt_t;

 /**
  *     Device info characteristics
  */
typedef enum
{
    eBLE_P_DEV_CHAR_DEV_NAME = 0,   /**<Device name characteristics */
    eBLE_P_DEV_CHAR_FW_VER,         /**<FW version characteristics */
    eBLE_P_DEV_CHAR_HW_VER,         /**<HW version characteristics */
    eBLE_P_DEV_CHAR_SERIAL_NUM,     /**<Serial number characteristics */
    eBLE_P_DEV_CHAR_MAN_NAME,       /**<Manufacturer name characteristics */

    eBLE_P_DEV_CHAR_NUM_OF,
} ble_p_dev_char_opt_t;

/** 
 *      BLE Characteristics Property
 */
typedef enum
{
    eBLE_P_CHAR_PROP_READ     = 0x01,     /**<Read access */
    eBLE_P_CHAR_PROP_WRITE    = 0x02,     /**<Write access + here included Write Wihtout Response */
    eBLE_P_CHAR_PROP_NOTIFY   = 0x04,     /**<Notify */
} ble_p_char_prop_opt_t;

/**
 *      BLE Characteristics data
 */
typedef struct
{
    ble_uuid_t              uuid;        /**<16-bit service UUID */
    uint16_t                handle;         /**<Service handle */
    ble_p_char_prop_opt_t   property;       /**<Characteristics property */

} ble_p_char_t;

/**
 *      BLE Service data
 */
typedef struct
{
    ble_p_char_t *  p_char;         /**<Pointer to service characteristics */
    uint8_t         char_num_of;    /**<Number of characteristics */

    ble_uuid128_t   uuid_128;       /**<128-bit service UUID */
    ble_uuid_t      uuid_16;        /**<16-bit service UUID */
    uint16_t        handle;         /**<Service handle */
} ble_p_service_t;

/**
 *      BLE Peripheral data
 */
typedef struct
{
    ble_gatts_char_handles_t    tx_char;            /**<TX characteristic handle */
    ble_gatts_char_handles_t    rx_char;            /**<RX characteristic handle */
    uint16_t                    conn_handle;        /**<Connection Handle on which event occurred */
    
    struct
    {
        uint16_t            serial;     /**<Serial service handle */
        uint16_t            dev_info;   /**<Device service handle */
    } service_handle;
    
    bool                        is_adv;             /**<Advertisment active flag */ 

} ble_p_data_t;

/**
 *      BLE Peripheral connection tag
 *
 * @brief   Connection config tag is a unique key for keeping track of an 
 *          advertising configuration
 *
 *          Further details: https://devzone.nordicsemi.com/f/nordic-q-a/33504/what-does-app_ble_conn_cfg_tag-do 
 */ 
#define BLE_P_CONN_CFG_TAG                          ( 1 )

/**
 *      Main BLE Peripheral event priority
 */
#define BLE_P_EVENT_PRIORITY                        3       // TODO: Find out what that number means!!!

/**
 *		BLE Peripheral reception buffer size
 *
 *	Unit: byte
 */                     
#define BLE_P_RX_BUF_SIZE                           ( 512 )  

/**
 *      Device name
 */
#define BLE_P_DEVICE_NAME                           ( "MyBLE" )

/**
 *      Device BLE appearance
 *
 * @note    Shall have standard value!
 *
 *          Look at "ble_types.h" file for pre-defined values.
 */
#define BLE_P_DEVICE_APPEARANCE                     ( BLE_APPEARANCE_GENERIC_COMPUTER )

/**
 *      Minimum connection interval
 *
 * @brief   This is part of Peripheral Prefered Connection Parameter (PPCP)
 *          values. It only suggest Central BLE device about connection
 *          timing. At the end Central BLE device is deciding if 
 *          respect these values or ignore them.
 *
 *          Valid value range: 7.5 ms - 4.0 sec
 *          Resolution:  1.25 ms
 *
 * @note    Min. connection interval shall not be larger than max.!
 *
 *  Unit: ms
 */
#define BLE_P_MIN_CONN_INTERVAL_MS                  ( 100 ) 

/**
 *      Maximum connection interval
 *
 * @brief   This is part of Peripheral Prefered Connection Parameter (PPCP)
 *          values. It only suggest Central BLE device about connection
 *          timing. At the end Central BLE device is deciding if 
 *          respect these values or ignore them.
 *
 *          Valid value range: 7.5 ms - 4.0 sec
 *          Resolution:  1.25 ms
 *
 * @note    Min. connection interval shall not be larger than max.!
 *
 *  Unit: ms
 */
#define BLE_P_MAX_CONN_INTERVAL_MS                  ( 200 ) 

/**
 *      Slave latency
 *
 * @brief   This is part of Peripheral Prefered Connection Parameter (PPCP)
 *          values. Slave latency parameter allows the peripheral to skip a number
 *          of consecutive connection Events without compromising the
 *          connection.
 *
 *          The slave latency value defines the number of connection
 *          events it can safely skip.
 *
 *          Valid value range: 0 - ((connSupervisionTimeout / connIntervalMax) - 1)
 *
 * @note    Value of 0 means that Peripheral BLE device needs to address
 *          every connection event triggered by Central BLE device.
 */
#define BLE_P_SLAVE_LATENCY                         ( 0 )

/**
 *      Supervsision timeout
 *
 * @brief   This is part of Peripheral Prefered Connection Parameter (PPCP)
 *          values. The Supervision Timeout is used to detect a loss in 
 *          communication. It is defined as maximum time between two received 
 *          data packets before the connection is considered lost.
 *
 *          Valid value range: 100 ms - 32 sec
 *
 * @note    Additional condition to check:
 *
 *              SupervisionTimeout > (( 1 + SlaveLatency ) * connInterval * 2 )
 *
 *  Unit: ms
 */
#define BLE_P_SUPERVISION_TIMEOUT_MS                ( 4000 )

 /**
  *     Advertisement inteval
  * 
  * @brief  Advertisement interval dictates time between two consequtive
  *         advertisement packets.
  *
  *         Valid value range: 20 ms - 10.24 sec
  *         Resolution: 0.625 ms
  *
  * @note   The advertising interval greatly impacts battery life. It is 
  *         recommended to choose the longest advertising interval that provides
  *         a balance between fast connectivity and reduced power consumption!
  *
  *     Unit: ms
  */
#define BLE_P_ADV_INTERVAL_MS                       ( 200.0 )

/**
 *      Advertisement duration
 *
 * @brief   Advertisement duration defines how long BLE peripheral
 *          device will advertise. After that time advertismeent
 *          will stop and Central BLE device won't be able to 
 *          scan it.
 *
 * @note    Value = 0 --> Continous advertisement!  
 *   
 *  Unit: ms
 */
#define BLE_P_ADV_DURATION_MS                       ( 60000 )

/**
 *      Enable/Disable start of addvertisement on disconnection event
 */
#define BLE_P_START_ADV_ON_DISCONNECT               ( 1 )

/**
 *      Company ID
 *
 * @brief   Company ID is part of Manufacturer Specific Data 
 *          in advertisement packet.
 *          
 *          Company ID values is defined by SIG BLE.
 *          
 *          Further details: https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
 *
 * @note    Company ID = 0x0059 for Nordic Semiconductor ASA
 */
#define BLE_P_ADV_MAN_DATA_COMPANY_ID               ( 0x0059 )

/**
 *      Time from initiating event (connect or start of notification) 
 *      to first time sd_ble_gap_conn_param_update is called
 *
 * @note    This setting is related to "ble_coon_params" library!
 *
 *  Unit: ms
 */
#define BLE_P_FIRST_CONN_PARAMS_UPDATE_DELAY_MS     ( 5000 )                   

/**
 *      Time between each call to sd_ble_gap_conn_param_update 
 *      after the first call
 *
 * @note    This setting is related to "ble_coon_params" library!
 *
 *  Unit: ms
 */
#define BLE_P_NEXT_CONN_PARAMS_UPDATE_DELAY_MS      ( 30000 )      

/**
 *      Number of attempts before giving up connection parameter negotiation
 */         
#define BLE_P_MAX_CONN_PARAMS_UPDATE_COUNT          ( 3 )                          

/**
 *      Serial service UUID for simple UART emulation
 *
 *      Random generated custom UUID (with "https://www.uuidgenerator.net/"):
 *
 *      5EC0xxxx-BEEF-4FEB-842C-E90E79703DA7 
 */
#define BLE_P_SERVICE_SERIAL_UUID_BASE              { 0xA7, 0x3D, 0x70, 0x79, 0x0E, 0xE9, 0x2C, 0x84, 0xEB, 0x4F, 0xEF, 0xBE, 0x00, 0x00, 0xC0, 0x5E }
#define BLE_P_SERVICE_SERIAL_UUID_16                ( 0x0100 )

/**
 *      Transmit characteristics 16-bit UUID
 *
 *  @note   Part of "Serial" service
 */
#define BLE_P_CHAR_TX_UUID                          ( 0x0101 )

/**
 *      Receive characteristics 16-bit UUID
 *
 *  @note   Part of "Serial" service
 */
#define BLE_P_CHAR_RX_UUID                          ( 0x0102 )

/**
 *      Device information service UUID
 *
 * @note    This UUID is defined by Bluetooth SIG standard.
 *          Futher details: https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
 */
#define BLE_P_SERVICE_DEV_INFO_UUID                 ( 0x180A )

/**
 *      Device name characteristic UUID
 *
 * @note    Part of "Device info" service
 */
#define BLE_P_CHAR_DEV_NAME_UUID                    ( 0x2A00 )

/**
 *      Firmware revision string characteristic UUID
 *
 * @note    Part of "Device info" service
 */
#define BLE_P_CHAR_FW_VER_UUID                      ( 0x2A26 )

/**
 *      Hardware revision string characteristic UUID
 *
 * @note    Part of "Device info" service
 */
#define BLE_P_CHAR_HW_VER_UUID                      ( 0x2A27 )

/**
 *      Serial number string characteristic UUID
 *
 * @note    Part of "Device info" service
 */
#define BLE_P_CHAR_SER_NUM_UUID                     ( 0x2A25 )

/**
 *      Manufacturer name string characteristic UUID
 *
 * @note    Part of "Device info" service
 */
#define BLE_P_CHAR_MAN_NAME_UUID                    ( 0x2A29 )

/**
 * 	Enable/Disable debug mode
 *
 * 	@note	Disable in release!
 */
#define BLE_P_DEBUG_EN                              ( 1 )

#ifndef DEBUG
    #undef BLE_P_DEBUG_EN
    #define BLE_P_DEBUG_EN 0
#endif

/**
 *  Enable/Disable assertion
 *
 *  @note   Disble in release!
 */
 #define BLE_P_ASSERT_EN                            ( 1 )

#ifndef DEBUG
    #undef PAR_CFG_ASSERT_EN
    #define PAR_CFG_ASSERT_EN 0
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == BLE_P_DEBUG_EN )
	#define BLE_P_DBG_PRINT( ... )                  ( cli_printf( __VA_ARGS__ ))
#else
	#define BLE_P_DBG_PRINT( ... )                  { ; }

#endif

/**
 *  Assertion definition
 */
 #if ( BLE_P_ASSERT_EN )
	#define BLE_P_ASSERT(x)                         { PROJECT_CONFIG_ASSERT(x) }
 #else
  #define BLE_P_ASSERT)                             { ; }
 #endif

/**
 *  Invalid configuration catcher
 */

 // Min. connection interval must be smaller than max.
 _Static_assert( BLE_P_MIN_CONN_INTERVAL_MS < BLE_P_MAX_CONN_INTERVAL_MS );

// Slave lateny must be in valid range
_Static_assert(( BLE_P_SLAVE_LATENCY >= 0) && ( BLE_P_SLAVE_LATENCY < 500 ));

// Supervision timeout must be in valid range
_Static_assert(( BLE_P_SUPERVISION_TIMEOUT_MS >= 100) && ( BLE_P_SUPERVISION_TIMEOUT_MS <= 32000 ));

// Supervision timeout additional condition must be meet
_Static_assert( BLE_P_SUPERVISION_TIMEOUT_MS > (( 1 + BLE_P_SLAVE_LATENCY ) * 2 * BLE_P_MAX_CONN_INTERVAL_MS ));

// Advertisement interval must be in valid range
_Static_assert(( BLE_P_ADV_INTERVAL_MS >= 20) && ( BLE_P_ADV_INTERVAL_MS <= 10240 ));


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
    .name 		= "BLE Peripheral Rx Buf",
    .item_size 	= sizeof(uint8_t),
    .override 	= false,
    .p_mem 		= &gu8_ble_rx_buffer 
};

/**
 *      BLE Peripheral data
 */
static ble_p_data_t g_ble_p = 
{ 
    .conn_handle    = BLE_CONN_HANDLE_INVALID,
    .is_adv         = false,


};  
    
/**
 *      Define GATT instance
 *
 *  @note   Calling this macro also register observer in background!
 */
NRF_BLE_GATT_DEF( g_gatt_instance );

/**
 *      Define Advertising instance
 *
 *  @note   Calling this macro also register observer in background!
 */ 
BLE_ADVERTISING_DEF( g_adv_instance );

/**
 *      Declare Queued Write instance
 *
 *  @brief  The Queued Writes module provides an implementation of 
 *          Queued Writes, using the Generic Attribute Profile (GATT) 
 *          Server interface of the SoftDevice. Add this module to your 
 *          GATT server implementation to enable support for Queued Writes 
 *          for some or all of the characteristics.
 *
 *  @note   Calling this macro also register observer in background!
 *
 *  @note   Leave this as it might come handy in future....
 */
//NRF_BLE_QWR_DEF( g_qwr_instance );




/**
 *      Serial characteristics
 */
static ble_p_char_t g_ble_p_serial_chars[eBLE_P_SER_CHAR_NUM_OF] = 
{
    [eBLE_P_SER_CHAR_TX] = { .handle = 0,   .uuid = BLE_P_CHAR_TX_UUID, .property = ( eBLE_P_CHAR_PROP_NOTIFY | eBLE_P_CHAR_PROP_READ )     },
    [eBLE_P_SER_CHAR_RX] = { .handle = 0,   .uuid = BLE_P_CHAR_RX_UUID, .property = ( eBLE_P_CHAR_PROP_WRITE )                              },
    
};

/**
 *      BLE Services
 */
static ble_p_service_t ble_p_service[ eBLE_P_SERVICE_NUM_OF ] =
{
    [ eBLE_P_SERVICE_SERIAL ]   = { .handle = 0, .uuid_128 = BLE_P_SERVICE_SERIAL_UUID_BASE, .uuid_16 = BLE_P_SERVICE_SERIAL_UUID_16, .p_char = &g_ble_p_serial_chars, .char_num_of = 2 },
    [ eBLE_P_SERVICE_DEV_IFO ]  = {  },
};









// Available services as it will be part of advertisement packet
static ble_uuid_t m_adv_uuids[] = 
{ 
    {   .uuid = BLE_UUID_HEALTH_THERMOMETER_SERVICE,    .type = BLE_UUID_TYPE_BLE  },   // SIG standard service
    {   .uuid = BLE_UUID_BATTERY_SERVICE,               .type = BLE_UUID_TYPE_BLE  },   // SIG standard service
};



//static ble_gatts_char_handles_t    char_handles    = {0};

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




////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t   ble_p_stack_init            (void);
static void             ble_p_evt_hndl              (ble_evt_t const * p_ble_evt, void * p_context);
static inline void      ble_p_evt_on_connect        (ble_evt_t const * p_ble_evt);
static inline void      ble_p_evt_on_disconnect     (ble_evt_t const * p_ble_evt);
static inline void      ble_p_evt_on_write          (ble_evt_t const * p_ble_evt);
static inline void      ble_p_evt_on_update_phy     (ble_evt_t const * p_ble_evt);
static inline void      ble_p_evt_on_missing_attr   (ble_evt_t const * p_ble_evt);
static ble_p_status_t   ble_p_gap_init              (void);
static ble_p_status_t   ble_p_gatt_init             (void);
static void             ble_p_adv_evt_hndl          (ble_adv_evt_t ble_adv_evt);
static ble_p_status_t   ble_p_adv_init              (void);
static ble_p_status_t   ble_p_conn_init             (void);
static void             ble_p_on_conn_pars_evt_hndl (ble_conn_params_evt_t * p_evt);
static ble_p_status_t   ble_p_service_init          (void);




////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize BLE peripheral stack
*
* @return 		status - Status of initialization
*/
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t ble_p_stack_init(void)
{
    ble_p_status_t  status      = eBLE_P_OK;
    uint32_t        ram_start   = 0;  // Application RAM start address

    // Start SoftDevice
    if ( NRF_SUCCESS != nrf_sdh_enable_request())
    {
        status = eBLE_P_ERROR;
        
        BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable request error!" );
    }

    // Configure BLE stack
    if ( NRF_SUCCESS != nrf_sdh_ble_default_cfg_set( BLE_P_CONN_CFG_TAG, &ram_start ))
    {
        status = eBLE_P_ERROR;
        
        BLE_P_DBG_PRINT( "BLE_P: SoftDevice default config set error!" );
    }

    // Enable BLE stack
    if ( NRF_SUCCESS != nrf_sdh_ble_enable( &ram_start ))
    {
        status = eBLE_P_ERROR;
        
        BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable error!" );
    }
    
    // Register BLE event callback
    // An observer is essentially a piece of code that listens for events.
    NRF_SDH_BLE_OBSERVER( m_ble_observer, BLE_P_EVENT_PRIORITY, ble_p_evt_hndl, NULL );

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Main BLE event handler
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @param[in] 	p_context   - Pointer to context
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static void ble_p_evt_hndl(ble_evt_t const * p_ble_evt, void * p_context)
{
    // Handle BLE events
    switch( p_ble_evt->header.evt_id )
    {
        /**
         *      Disconnected from peer event
         */
        case BLE_GAP_EVT_DISCONNECTED:
            ble_p_evt_on_disconnect( p_ble_evt );
            break;

        /**
         *      Connected to peer event
         */
        case BLE_GAP_EVT_CONNECTED:
            ble_p_evt_on_connect( p_ble_evt );
            break;

        /**
         *      PHY Update Procedure is complete
         *
         * @note    This event must be handler in order not to lose connection 
         *          with peer. If this event is not handled the connection
         *          will timeout.
         *
         *          Further details: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.s132.api.v7.2.0%2Fgroup___b_l_e___g_a_p___p_e_r_i_p_h_e_r_a_l___p_h_y___u_p_d_a_t_e.html&cp=4_7_3_1_2_1_5_7_1
         */
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            ble_p_evt_on_update_phy( p_ble_evt );
            break;

        /**
         *      A persistent system attribute access is pending
         *
         * @note    This event is triggered if the peer request a read on any of the
         *          system attributes
         *
         *          In the case where you send indications or notifications before the peer requests 
         *          such a read, for instance you want to perform a "sd_ble_gatts_service_changed()", 
         *          you need to do "the sd_ble_gatts_sys_attr_set()" first.
         *
         *          Furhter details: https://devzone.nordicsemi.com/f/nordic-q-a/54039/why-don-t-i-get-a-ble_gatts_evt_sys_attr_missing-event
         */
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            ble_p_evt_on_missing_attr( p_ble_evt );
            break;

        /**
         *      Write operation performed
         */
        case BLE_GATTS_EVT_WRITE:
            ble_p_evt_on_write( p_ble_evt );
            break;

        default:
            // No actions...
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE on connect event handler 
*
* @note     This function is being executed in main BLE stack callback!
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static inline void ble_p_evt_on_connect(ble_evt_t const * p_ble_evt)
{
    // Assing connection info to handle
    g_ble_p.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    // Stop advertisement
    // NOTE: This is done automatically by BLE SDK library!
    //(void) ble_p_adv_stop();

    // Raise callback
    ble_p_evt_cb( eBLE_P_EVT_CONNECT );

    BLE_P_DBG_PRINT( "BLE_P: Connected!" );
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE on disconnect event handler 
*
* @note     This function is being executed in main BLE stack callback!
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static inline void ble_p_evt_on_disconnect(ble_evt_t const * p_ble_evt)
{
    // Connection lost
    g_ble_p.conn_handle = BLE_CONN_HANDLE_INVALID;

    // Raise callback
    ble_p_evt_cb( eBLE_P_EVT_DISCONNECT );

    BLE_P_DBG_PRINT( "BLE_P: Disconnected!" );

    #if ( 1 == BLE_P_START_ADV_ON_DISCONNECT )

        // Start advertisement
        (void) ble_p_adv_start();

    #endif  
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE on write event handler 
*
* @note     This function is being executed in main BLE stack callback!
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static inline void ble_p_evt_on_write(ble_evt_t const * p_ble_evt)
{
    // Client write to RX characteristics?
    // TODO: Check for this...
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

        // Raise callback
        ble_p_evt_cb( eBLE_P_EVT_RX_DATA ); 
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE on update of PHY event handler 
*
* @note     This function is being executed in main BLE stack callback!
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static inline void ble_p_evt_on_update_phy(ble_evt_t const * p_ble_evt)
{
    // Set to auto option for PHY
    const ble_gap_phys_t phys = 
    {
        .rx_phys = BLE_GAP_PHY_AUTO,
        .tx_phys = BLE_GAP_PHY_AUTO,
    };

    // Update PHY
    if ( NRF_SUCCESS != sd_ble_gap_phy_update( p_ble_evt->evt.gap_evt.conn_handle, &phys))
    {
        // TODO: How to handle this error???

        BLE_P_DBG_PRINT( "BLE_P: PHY update error!" );
        BLE_P_ASSERT( 0 );
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE on missing system attributes event handler 
*
* @note     This function is being executed in main BLE stack callback!
*
* @param[in] 	p_ble_evt   - Pointer to BLE event informations
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static inline void ble_p_evt_on_missing_attr(ble_evt_t const * p_ble_evt)
{
    // Update persistent system attribute information
    if ( NRF_SUCCESS != sd_ble_gatts_sys_attr_set( g_ble_p.conn_handle, NULL, 0, 0))
    {
        // TODO: How to handle this error???

        BLE_P_DBG_PRINT( "BLE_P: Setting SYSTEM_ATTRIBUTES error!" );
        BLE_P_ASSERT( 0 );
    }    
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize BLE Peripheral GAP
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t ble_p_gap_init(void)
{
    ble_p_status_t              status          = eBLE_P_OK;
    ble_gap_conn_params_t       gap_conn_params = {0};
    ble_gap_conn_sec_mode_t     security_mode   = {0};

    // Use no security
    BLE_GAP_CONN_SEC_MODE_SET_OPEN( &security_mode );

    // Set the device name
    if ( NRF_SUCCESS != sd_ble_gap_device_name_set( &security_mode, (const uint8_t*) BLE_P_DEVICE_NAME, strlen( BLE_P_DEVICE_NAME )))
    {
        status = eBLE_P_ERROR;

        BLE_P_DBG_PRINT( "BLE_P: Setting device name error (GAP)!" );
    }

    // Set the apperance
    if ( NRF_SUCCESS != sd_ble_gap_appearance_set( BLE_P_DEVICE_APPEARANCE ))  
    {
        status = eBLE_P_ERROR;

        BLE_P_DBG_PRINT( "BLE_P: Setting device appearance error (GAP)!" );
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize BLE Peripheral GATT
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t ble_p_gatt_init(void)
{
    ble_p_status_t status = eBLE_P_OK;

    // Initialize GATT
    if ( NRF_SUCCESS != nrf_ble_gatt_init( &g_gatt_instance, NULL ))
    { 
        status = eBLE_P_ERROR;

        BLE_P_DBG_PRINT( "BLE_P: GATT init error!" );  
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize BLE Peripheral advertisement
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t ble_p_adv_init(void)
{
    ble_p_status_t              status   = eBLE_P_OK;
    ble_advertising_init_t      adv_init = {0};
    ble_advdata_manuf_data_t    man_data = {0};

    // Set up advertising parameters
    adv_init.advdata.name_type              = BLE_ADVDATA_FULL_NAME;
    adv_init.advdata.include_appearance     = true;
    adv_init.advdata.flags                  = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    adv_init.config.ble_adv_fast_enabled    = true;
    adv_init.config.ble_adv_fast_interval   = (uint32_t) ( BLE_P_ADV_INTERVAL_MS / 0.625f );
    adv_init.config.ble_adv_fast_timeout    = (uint32_t) ( BLE_P_ADV_DURATION_MS / 10.0f );
    adv_init.evt_handler                    = ble_p_adv_evt_hndl;
    adv_init.advdata.p_manuf_specific_data  = &man_data;
    
    // TODO: Find out how to add here custom service UUID
    #if 0
        // Show also available services in advertisement packet
        adv_init.advdata.uuids_complete.p_uuids = (ble_uuid_t*) &m_adv_uuids;
        adv_init.advdata.uuids_complete.uuid_cnt = 2;
    #endif

    // Set up manufacturer data
    man_data.company_identifier = BLE_P_ADV_MAN_DATA_COMPANY_ID;
    man_data.data.p_data        = "Vendor Data";
    man_data.data.size          = strlen( man_data.data.p_data );

    // Init advertisement
    if ( NRF_SUCCESS != ble_advertising_init( &g_adv_instance, &adv_init ))
    {
        // TODO: Change error status...
        BLE_P_DBG_PRINT( "BLE_P: BLE Adveritising library init error!" );
        BLE_P_ASSERT( 0 );
    }
    
    // Setup advertising config tag
    ble_advertising_conn_cfg_tag_set( &g_adv_instance, BLE_P_CONN_CFG_TAG );

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral Advertisement event handler
*
* @param[in]    ble_adv_evt     - Advertisement event
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
static void ble_p_adv_evt_hndl(ble_adv_evt_t ble_adv_evt)
{
    // Based on advertisemnt event
    switch (ble_adv_evt)
    {
        /**
         *      Fast advertising mode has started
         */
        case BLE_ADV_EVT_FAST:

            // Set adv flag
            g_ble_p.is_adv = true;

            // Raise callback
            ble_p_evt_cb( eBLE_P_EVT_ADV_START );

            BLE_P_DBG_PRINT( "BLE_P: Advertisement started!" );

            break;
        
        /**
         *      Idle; no connectable advertising is ongoing
         */
        case BLE_ADV_EVT_IDLE:

            // Clear adv flag
            g_ble_p.is_adv = false;

            // Raise callback
            ble_p_evt_cb( eBLE_P_EVT_ADV_END );

            BLE_P_DBG_PRINT( "BLE_P: Advertisement stoped!" );

            break;

        default:
            // No actions...
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize BLE connection library
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
static ble_p_status_t ble_p_conn_init(void)
{
    ble_p_status_t          status          = eBLE_P_OK;
    ble_conn_params_init_t  conn_par_init   = {0};
    ble_gap_conn_params_t   ppcp_cfg        = {0};


    conn_par_init.p_conn_params                  = NULL;
    conn_par_init.first_conn_params_update_delay = APP_TIMER_TICKS( BLE_P_FIRST_CONN_PARAMS_UPDATE_DELAY_MS );
    conn_par_init.next_conn_params_update_delay  = APP_TIMER_TICKS( BLE_P_NEXT_CONN_PARAMS_UPDATE_DELAY_MS );
    conn_par_init.max_conn_params_update_count   = BLE_P_MAX_CONN_PARAMS_UPDATE_COUNT;
    conn_par_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    conn_par_init.disconnect_on_fail             = false;
    conn_par_init.evt_handler                    = ble_p_on_conn_pars_evt_hndl;

    // Setup PPCP - Peripheral Prefered Connection Parameters
    ppcp_cfg.min_conn_interval   = MSEC_TO_UNITS( BLE_P_MIN_CONN_INTERVAL_MS, UNIT_1_25_MS );
    ppcp_cfg.max_conn_interval   = MSEC_TO_UNITS( BLE_P_MAX_CONN_INTERVAL_MS, UNIT_1_25_MS );
    ppcp_cfg.slave_latency       = BLE_P_SLAVE_LATENCY;
    ppcp_cfg.conn_sup_timeout    = MSEC_TO_UNITS( BLE_P_SUPERVISION_TIMEOUT_MS, UNIT_10_MS );
    conn_par_init.p_conn_params  = &ppcp_cfg;

    // Init BLE connection library
    if ( NRF_SUCCESS != ble_conn_params_init( &conn_par_init ))
    {
        status = eBLE_P_ERROR;

        BLE_P_DBG_PRINT( "BLE_P: BLE connection parameters library init error!" );
        BLE_P_ASSERT( 0 );
    }

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function for handling the Connection Parameters Module.
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
 ////////////////////////////////////////////////////////////////////////////////
static void ble_p_on_conn_pars_evt_hndl(ble_conn_params_evt_t * p_evt)
{
    // Negotiation procedure failed
    if ( BLE_CONN_PARAMS_EVT_FAILED == p_evt->evt_type )
    {
        if ( NRF_SUCCESS != sd_ble_gap_disconnect( g_ble_p.conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE ))
        {
            // TODO: How to handle this error?

            BLE_P_DBG_PRINT( "BLE_P: Disconnection error! (sd_ble_gap_disconnect)" );
            BLE_P_ASSERT( 0 );
        }
    }
}




static ble_p_status_t ble_p_service_init(void)
{
    ble_p_status_t      status          = eBLE_P_OK;
    ble_uuid_t          ble_uuid        = {0};
    ble_uuid128_t       base_uuid       = BLE_P_SERVICE_SERIAL_UUID_BASE;


    ble_uuid.uuid      = BLE_P_SERVICE_SERIAL_UUID_16;

    if ( NRF_SUCCESS != sd_ble_uuid_vs_add( &base_uuid, &ble_uuid.type ))
    {
        // TODO: Change error status...
        BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable error!" ); 
    }

    // Register service
    if ( NRF_SUCCESS != sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &custom_service_handle ))
    {
        // TODO: Change error status...
        BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable error!" ); 
    }


    #if 0
        // Add RX characteristics
        ble_add_char_params_t rx_char_params = {0};

        rx_char_params.uuid                     = BLE_P_CHAR_RX_UUID;
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
            // TODO: Change error status...
            BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable error!" ); 
        }

    
        // Add Tx characteristics
        ble_add_char_params_t tx_char_params = {0};

        tx_char_params.uuid                     = BLE_P_CHAR_TX_UUID;
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
            // TODO: Change error status...
            BLE_P_DBG_PRINT( "BLE_P: SoftDevice enable error!" );
        }
    #endif

    return status;
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

    // Init BLE stack
    status |= ble_p_stack_init();

    // Init GAP (connection parameters)
    status |= ble_p_gap_init();
    
    // Init GATT
    status |= ble_p_gatt_init();
    
    // Init advertisement
    status |= ble_p_adv_init();

    // Init BLE connection library
    status |= ble_p_conn_init();

    // Init services
    status |= ble_p_service_init();

    // Init success
    if ( eBLE_P_OK == status )
    {
        gb_is_init = true;

        // Start advertising     
        status = ble_p_adv_start();
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
*		BLE Peripheral start advertising
*
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_adv_start(void)
{
    ble_p_status_t  status          = eBLE_P_OK;
    bool            is_connected    = false;

    BLE_P_ASSERT( true == gb_is_init );

    // Get connection flag
    ble_p_is_connected( &is_connected );

    if (    ( true == gb_is_init )
        &&  ( false == g_ble_p.is_adv )
        &&  ( false == is_connected ))
    {
        // Start advertisement
        if ( NRF_SUCCESS != ble_advertising_start( &g_adv_instance, BLE_ADV_MODE_FAST ))
        {
            status = eBLE_P_ERROR;
    
            BLE_P_DBG_PRINT( "BLE_P: Advertisement start error!" ); 
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
*		BLE Peripheral stop advertising
*
* @note     It is recommended to setup advertise duration rather that
*           calling that function as BLE Advertisement library don't 
*           really support advertisement stop.
*
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_adv_stop(void)
{
    ble_p_status_t  status          = eBLE_P_OK;
    bool            is_connected    = false;

    BLE_P_ASSERT( true == gb_is_init );

    // Get connection flag
    ble_p_is_connected( &is_connected );

    if (    ( true == gb_is_init )
        &&  ( true == g_ble_p.is_adv )
        &&  ( false == is_connected ))
    {
        // Stop advertisement library
        if ( NRF_SUCCESS != ble_advertising_start( &g_adv_instance, BLE_ADV_MODE_IDLE ))
        {
            status = eBLE_P_ERROR;
        
            BLE_P_DBG_PRINT( "BLE_P: BLE adv stop error!" ); 
        }

        // Stop advertisement diretly via SoftDevice
        if ( NRF_SUCCESS != sd_ble_gap_adv_stop( g_adv_instance.adv_handle ))
        {
            status = eBLE_P_ERROR;
        
            BLE_P_DBG_PRINT( "BLE_P: SD adv stop error!" ); 
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
*		Is BLE Peripheral advertising
*
* @param[out]   p_is_adv    - Pointer to advertisement status
* @return 		status		- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_is_adv(bool * const p_is_adv)
{
    ble_p_status_t status = eBLE_P_OK;

    if ( NULL != p_is_adv )
    {
        *p_is_adv = g_ble_p.is_adv;
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
    ble_p_status_t  status          = eBLE_P_OK;
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

////////////////////////////////////////////////////////////////////////////////
/**
*		BLE Peripheral events
*
* @param[in]	event	- Event type
* @return 		void
*/
////////////////////////////////////////////////////////////////////////////////
__attribute__((weak)) void ble_p_evt_cb(const ble_p_evt_t event)
{
    /**
     * 	Leave empty for user application purposes...
     */
}











void ble_p_hndl(void)
{
    #if 0    
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
    #endif

    
    static uint8_t cnt[200] = {0};
    bool is_connected = false;

    (void) ble_p_is_connected( &is_connected );

    if ( true == is_connected )
    {

        cnt[1]++;
        cnt[199]++;

        ble_p_write((const uint8_t*) &cnt, 200 );
    }

    led_toggle( eLED_3 );
}





////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
