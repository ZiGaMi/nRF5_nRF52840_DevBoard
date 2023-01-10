// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      project_config.h
*@brief     Project configurations
*@author    Ziga Miklosic
*@date      01.08.2022
*@project	Base code for nRF5_nRF52840_DevBoard
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup PROJECT_CONFIGURATIONS
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
#ifndef __PROJECT_CONFIG_H
#define __PROJECT_CONFIG_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *  Enable/Disable WDT
 */
#define PROJECT_CONFIG_WDT_EN           ( 1 )

// Float definition
typedef float float32_t;

/**
 * 	My floating PI
 */
#define MY_PI           ((float32_t)( 3.1415926 ))
#define MY_TWOPI        ((float32_t)( 2.0 * MY_PI ))
#define MY_PI_OVER_FOUR	((float32_t)( MY_PI / 4.0 ))

/**
 * 	Common goods
 */
#define RAD_TO_DEG      ((float32_t) ( 180.0 / MY_PI ))	// Leave double
#define DEG_TO_RAD      ((float32_t) ( MY_PI / 180.0 ))	// Leave double


/**
 * 	Project debug mode
 */
#ifdef DEBUG
    #define PROJECT_CONFIG_DEBUG_EN		( 1 )
#else
    #define PROJECT_CONFIG_DEBUG_EN		( 0 )
#endif

/**
 * 	Project assertion
 */
#if ( PROJECT_CONFIG_DEBUG_EN )
    void project_config_assert_fail(void);
    #define PROJECT_CONFIG_ASSERT(x)			if( !( x )) { project_config_assert_fail(); }
#else
    #define PROJECT_CONFIG_ASSERT(x)			{ ; }
#endif

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////


#endif // __PROJECT_CONFIG_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
