// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      adc.h
*@brief     ADC low level driver
*@author    Ziga Miklosic
*@date      02.11.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup ADC
* @{ <!-- BEGIN GROUP -->
*
* 	ADC low level driver
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __ADC_H
#define __ADC_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "project_config.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 	ADC status
 */
typedef enum
{
	eADC_OK = 0,	/**<Normal operation */
	eADC_ERROR,		/**<General error code */
} adc_status_t;

/**
 *  ADC (analog) pins
 */
typedef enum
{
    // USER CODE BEGIN...

	eADC_AIN_1 = 0,     /**< P0.03 on nRF52840 DK */
	eADC_AIN_2,         /**< P0.04 on nRF52840 DK */
	eADC_AIN_4,         /**< P0.28 on nRF52840 DK */
	eADC_AIN_5,         /**< P0.29 on nRF52840 DK */
	eADC_AIN_6,         /**< P0.30 on nRF52840 DK */
	eADC_AIN_7,         /**< P0.31 on nRF52840 DK */

    // USER CODE END...

	eADC_NUM_OF
} adc_pins_t;

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
adc_status_t 	adc_init		(void);
uint16_t		adc_get_raw		(const adc_pins_t pin);
float32_t		adc_get_real	(const adc_pins_t pin);

#endif // __ADC_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
