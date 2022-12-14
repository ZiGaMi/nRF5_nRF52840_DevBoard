// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
* @file     button_cfg.h
* @brief    Button configurations
* @author   Ziga Miklosic
* @date     04.11.2022
* @version	V1.1.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
* @addtogroup BUTTON_CFG
* @{ <!-- BEGIN GROUP -->
*
*
* 	Configuration for Button
*
* 	User shall put code inside inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __BUTTON_CFG_H_
#define __BUTTON_CFG_H_

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

// USER CODE BEGIN...

//#include "project_config.h"

// Debug communication port
//#include "middleware/cli/cli/src/cli.h"

// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 	List of Buttons
 *
 * @note 	User shall provide Button name here as it would be using
 * 			later inside code.
 *
 * 			Shall always start with 0.
 *
 * @note 	User shall change code only inside section of "USER_CODE_BEGIN"
 * 			ans "USER_CODE_END".
 */
typedef enum
{
	// USER CODE START...

    eBUTTON_1 = 0,
    eBUTTON_2,
    eBUTTON_3,
    eBUTTON_4,

	// USER CODE END...

	eBUTTON_NUM_OF
} button_num_t;


// USER CODE BEGIN...

/**
 * 	Main button handler period
 * 	Unit: sec
 */
#define BUTTON_CFG_HNDL_PERIOD_S				( 0.01f )

/**
 * 	Enable/Disable usage of Filter module. It is being used for debouncing.
 */
#define BUTTON_CFG_FILTER_EN					( 1 )

/**
 * 	Enable/Disable debug mode
 *
 * 	@note Disabled in release!
 */
#define BUTTON_CFG_DEBUG_EN						( 0 )
#ifndef DEBUG
	#undef BUTTON_CFG_DEBUG_EN
	#define BUTTON_CFG_DEBUG_EN	( 0 )
#endif

/**
 * 	Enable/Disable assertions
 *
 * 	@note Disabled in release!
 */
#define BUTTON_CFG_ASSERT_EN					( 0 )
#ifndef DEBUG
	#undef BUTTON_CFG_ASSERT_EN
	#define BUTTON_CFG_ASSERT_EN	( 0 )
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == BUTTON_CFG_DEBUG_EN )
	#define BUTTON_PRINT( ... )				( cli_printf( __VA_ARGS__ ))
#else
	#define BUTTON_PRINT( ... )				{ ; }

#endif

/**
 * 	 Assertion macros
 */
#if ( 1 == BUTTON_CFG_ASSERT_EN )
	#define BUTTON_ASSERT(x)					PROJECT_CONFIG_ASSERT(x)
#else
	#define BUTTON_ASSERT(x)					{ ; }
#endif

// USER CODE END...


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
const void * button_cfg_get_table(void);

#endif // __BUTTON_CFG_H_
////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
