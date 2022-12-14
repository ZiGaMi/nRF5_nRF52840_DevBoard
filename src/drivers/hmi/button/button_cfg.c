// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
* @file     button_cfg.c
* @brief    Button configurations
* @author   Ziga Miklosic
* @date     04.11.2022
* @version	V1.1.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
 * @addtogroup BUTTON_CFG
 * @{ <!-- BEGIN GROUP -->
 */
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "button_cfg.h"
#include "button/src/button.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 * 	Button configuration table
 *
 * 	@brief	LPF is RC 1st order IIR filter. Output is being compared between
 * 			5% and 95% (3 Tao).
 *
 * 			E.g. LPF with fc=1Hz will result in button detection time delay
 * 			of:
 *
 * 				fc = 1 / ( 2*pi*Tao ) ---> Tao = 1 / ( 2*pi*fc )
 *
 * 				dt = 3 Tao = 3 * 1 / ( 2*pi*fc )
 *
 * 				fc = 1.0 Hz ---> dt = 0.477 sec
 *
 *
 * 	@note 	Low level gpio code must be compatible!
 */
static const button_cfg_t g_button_cfg[ eBUTTON_NUM_OF ] =
{

	// USER CODE BEGIN...

	// -------------------------------------------------------------------------------------------------------------------------
	//	Gpio pin						Button polarity							LPF enable			Default LPF fc
	// -------------------------------------------------------------------------------------------------------------------------
    {     .gpio_pin = eGPIO_BTN_1,    .polarity = eBUTTON_POL_ACTIVE_LOW,    .lpf_en = true,    .lpf_fc = 10.0f          },
    {     .gpio_pin = eGPIO_BTN_2,    .polarity = eBUTTON_POL_ACTIVE_LOW,    .lpf_en = true,    .lpf_fc = 10.0f          },
    {     .gpio_pin = eGPIO_BTN_3,    .polarity = eBUTTON_POL_ACTIVE_LOW,    .lpf_en = true,    .lpf_fc = 10.0f          },
    {     .gpio_pin = eGPIO_BTN_4,    .polarity = eBUTTON_POL_ACTIVE_LOW,    .lpf_en = true,    .lpf_fc = 10.0f          },


	// USER CODE END...

};

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Get button configuration table
*
* @return		pointer to configuration table
*/
////////////////////////////////////////////////////////////////////////////////
const void * button_cfg_get_table(void)
{
	return (button_cfg_t*) &g_button_cfg;
}

////////////////////////////////////////////////////////////////////////////////
/*!
 * @} <!-- END GROUP -->
 */
////////////////////////////////////////////////////////////////////////////////
