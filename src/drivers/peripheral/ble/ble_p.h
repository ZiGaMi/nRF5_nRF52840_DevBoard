// Copyright (c) 2023 Ziga Miklosic
// All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
/**
*@file      ble_p.h
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
/**
*@addtogroup BLE_P_API
* @{ <!-- BEGIN GROUP -->
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __BLE_P_H
#define __BLE_P_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *  BLE Peripheral status
 */
typedef enum
{
    eBLE_P_OK       = 0x00,     /**<Normal operation */
    eBLE_P_ERROR    = 0x01,     /**<General error code */
} ble_p_status_t;

/**
 *  BLE Peripheral events
 */
typedef enum
{
    eBLE_P_EVT_CONNECT  = 0,    /**<Peer connected event */
    eBLE_P_EVT_DISCONNECT,      /**<Peer dis-connected event */
    eBLE_P_EVT_RX_DATA,         /**<Client writes to Rx characteristics */
    eBLE_P_EVT_ADV_START,       /**<Advertising started event */
    eBLE_P_EVT_ADV_END,         /**<Advertising ended event */
} ble_p_evt_t;

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
ble_p_status_t ble_p_init           (void);
ble_p_status_t ble_p_is_init        (bool * const p_is_init);

ble_p_status_t ble_p_set_adv_start      (void);
ble_p_status_t ble_p_set_adv_stop       (void);
ble_p_status_t ble_p_is_adv             (bool * const p_is_adv);

ble_p_status_t ble_p_is_connected   (bool * const p_is_conn);
ble_p_status_t ble_p_write          (const uint8_t * const p_data, const uint16_t len);
ble_p_status_t ble_p_get            (uint8_t * const p_data);

void ble_p_evt_cb(const ble_p_evt_t event);




// TODO: Omit this no need...
void ble_p_hndl       (void);
 


#endif // __BLE_P_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
