// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      usb_cdc.h
*@brief     USB CDC class
*@author    Ziga Miklosic
*@date      20.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup USB_CDC
* @{ <!-- BEGIN GROUP -->
*
* 	Debug USB_CDC API
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __USC_CDC_H
#define __USC_CDC_H

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
 * 	USB CDC status
 */
typedef enum
{
    eUSB_CDC_OK = 0,	/**<Normal operation */
    eUSB_CDC_ERROR,		/**<General error code */
} usb_cdc_status_t;


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
usb_cdc_status_t usb_cdc_init	(void);
usb_cdc_status_t usb_cdc_hndl	(void);
usb_cdc_status_t usb_cdc_write	(const char* str);
usb_cdc_status_t usb_cdc_get	(char * const p_char);

void usb_cdc_plugged_cb         (void);
void usb_cdc_unplugged_cb       (void);
void usb_cdc_port_open_cb       (void);
void usb_cdc_port_close_cb      (void);

#endif // __USC_CDC_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
