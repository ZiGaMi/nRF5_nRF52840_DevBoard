// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
* @file     led_cfg.h
* @brief    LED configurations
* @author   Ziga Miklosic
* @date     27.09.2021
* @version	V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
* @addtogroup LED_CFG
* @{ <!-- BEGIN GROUP -->
*
*
* 	Configuration for LED
*
* 	User shall put code inside inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __LED_CFG_H_
#define __LED_CFG_H_

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

// USER CODE BEGIN...

#include "project_config.h"

// Debug communication port
#include "middleware/cli/cli/src/cli.h"

// USER CODE END...


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


/**
 * 	List of LEDs
 *
 * @note 	User shall provide LED name here as it would be using
 * 			later inside code.
 *
 * @note 	User shall change code only inside section of "USER_CODE_BEGIN"
 * 			ans "USER_CODE_END".
 */
typedef enum
{
    // USER CODE START...

    eLED_1 = 0,
    eLED_2,
    eLED_3,
    eLED_4,

    // USER CODE END...

    eLED_NUM_OF
} led_num_t;


// USER CODE BEGIN...

/**
 * 	Main LED handler period
 * 	Unit: sec
 */
#define LED_CFG_HNDL_PERIOD_S					( 0.01f )

/**
 * 	Using timer for driving LED
 */
#define LED_CFG_TIMER_USE_EN					( 0 )

/**
 * 	Using GPIO for driving LED
 */
#define LED_CFG_GPIO_USE_EN						( 1 )

/**
 * 	Enable/Disable debug mode
 *
 * 	@note Disabled in release!
 */
#define LED_CFG_DEBUG_EN						( 0 )
#ifndef DEBUG
	#undef LED_CFG_DEBUG_EN
	#define LED_CFG_DEBUG_EN	( 0 )
#endif

/**
 * 	Enable/Disable assertions
 *
 * 	@note Disabled in release!
 */
#define LED_CFG_ASSERT_EN						( 1 )
#ifndef DEBUG
	#undef LED_CFG_ASSERT_EN
	#define LED_CFG_ASSERT_EN	( 0 )
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == LED_CFG_DEBUG_EN )
	#define LED_DBG_PRINT( ... )				( cli_printf( __VA_ARGS__ ))
#else
	#define LED_DBG_PRINT( ... )				{ ; }

#endif

/**
 * 	 Assertion macros
 */
#if ( 1 == LED_CFG_ASSERT_EN )
	#define LED_ASSERT(x)						PROJECT_CONFIG_ASSERT(x)
#else
	#define LED_ASSERT(x)						{ ; }
#endif

// USER CODE END...

/**
 * 	Faulty configurations check
 */
#if (( 0 == LED_CFG_TIMER_USE_EN ) && ( 0 == LED_CFG_GPIO_USE_EN ))
	#error "Select either GPIO or TIMER PWM LED driver!"
#endif

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
const void * led_cfg_get_table(void);

#endif // __LED_CFG_H_

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
