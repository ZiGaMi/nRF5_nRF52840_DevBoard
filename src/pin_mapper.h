// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      pin_mapper.h
*@brief     Project pin mapping
*@author    Ziga Miklosic
*@date      22.10.2022
*@project   Base code for nRF5_nRF52840_DevBoard	
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup PIN_MAPPER
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __PIN_MAPPER_H
#define __PIN_MAPPER_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * 	PORT 0
 ******************************************************************************/

// P0.00
// Not used...

// P0.01
// Not used...

// P0.02
// Not used...

// P0.03
// Not used...

// P0.04
// Not used...

// P0.05
// Not used..

// P0.06
#define DBG_UART_TX__PORT   0
#define DBG_UART_TX__PIN    6

// P0.07
// Not used...

// P0.08
#define DBG_UART_RX__PORT   0
#define DBG_UART_RX__PIN    8

// P0.09
// Not used...

// P0.10
// Not used...

// P0.11
#define BTN_1__PORT         0
#define BTN_1__PIN          11

// P0.12
#define BTN_2__PORT         0
#define BTN_2__PIN          12

// P0.13
#define LED_1__PORT         0
#define LED_1__PIN          13

// P0.14
#define LED_2__PORT         0
#define LED_2__PIN          14

// P0.15
#define LED_3__PORT         0
#define LED_3__PIN          15

// P0.16
#define LED_4__PORT         0
#define LED_4__PIN          16

// P0.17
// Not used...

// P0.18
// Not used...

// P0.19
// Not used...

// P0.20
// Not used...

// P0.21
// Not used...

// P0.22
// Not used..

// P0.23
// Not used..

// P0.24
#define BTN_3__PORT         0
#define BTN_3__PIN          24

// P0.25
#define BTN_4__PORT         0
#define BTN_4__PIN          25


/*******************************************************************************
 * 	PORT 1
 ******************************************************************************/




#endif // __PIN_MAPPER_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
