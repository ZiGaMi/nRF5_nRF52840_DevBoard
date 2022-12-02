// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      wdt_cfg.h
*@brief     Watchdog Configurations
*@author    Ziga Miklosic
*@date      02.09.2021
*@version   V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup WATCHDOG_CONFIGURATIONS
* @{ <!-- BEGIN GROUP -->
*
* 	Put code that is platform depended inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*/
////////////////////////////////////////////////////////////////////////////////
#ifndef __WDT_CFG_H
#define __WDT_CFG_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>

// USER CODE BEGIN...

#include "project_config.h"

// Debug communication port
#include "middleware/cli/cli/src/cli.h"

// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 * 		Watchdog protected task list
 * 
 * @brief	User shall list all protected task here! 
 */
typedef enum
{
	// USER CODE START...

	eWDT_TASK_MAIN = 0,

	// USER CODE END...

	eWDT_TASK_NUM_OF
} wdt_task_t;

/**
 * 	Watchdog kick period
 *
 * @note	This define must correspond to WDT timer open window settings!
 *
 * 			Period number must be whole number of time period of
 * 			main watchdog handler calls ( wdt_hndl ).
 *
 * @note    In case of using simple WDT configured timeout period shall
 *          be 10x higher that this value!
 *
 * 	Unit: ms
 */
#define WDT_CFG_KICK_PERIOD_TIME_MS				( 10.0f )

/**
 * 	Maximum timeout time for Watchdog timer
 *
 * 	@note 	This is being used only for configuration table checks!
 * 
 * 	Unit: ms
 */
#define WDT_CFG_MAX_TIMEOUT_TIME_MS				( 5000.0f )

/**
 * 	Minimum timeout time for Watchdog timer
 *
 * 	@note 	This is being used only for configuration table checks!
 * 
 * 	Unit: ms
 */
#define WDT_CFG_MIN_TIMEOUT_TIME_MS				( 0.01f )

/**
 * 	Enable/Disable debug mode
 */
#define WDT_CFG_DEBUG_EN						( 1 )

/**
 * 	Enable/Disable assertions
 */
#define WDT_CFG_ASSERT_EN						( 1 )

/**
 * 	Debug communication port macros
 */
#if ( 1 == WDT_CFG_DEBUG_EN )
	#define WDT_DBG_PRINT( ... )				( cli_printf( __VA_ARGS__ ))
#else
	#define WDT_DBG_PRINT( ... )				{ ; }

#endif

/**
 * 	 Assertion macros
 */
#if ( 1 == WDT_CFG_ASSERT_EN )
	#define WDT_ASSERT(x)						PROJECT_CONFIG_ASSERT(x)
#else
	#define WDT_ASSERT(x)						{ ; }
#endif

/**
 * 	Enable/Disable statistics
 * 
 * @note	WDT_CFG_DEBUG_EN macro must be enabled in order
 * 			to support statistics.
 */
#define WDT_CFG_STATS_EN						( 1 )

/**
 * 	Platform weak definition
 */
#define __WDT_WEAK_FNC__						__attribute__((weak))

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
const void * wdt_cfg_get_table(void);

#endif // __WDT_CFG_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
