// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      par_cfg.h
*@brief    	Configuration for device parameters
*@author    Ziga Miklosic
*@date      22.05.2021
*@version	V1.3.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup PAR_CFG
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _PAR_CFG_H_
#define _PAR_CFG_H_

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdlib.h>

// USER CODE BEGIN...

#include "project_config.h"

// Debug communication port
#include "middleware/cli/cli/src/cli.h"

// NVM
//#include "middleware/nvm/nvm_cfg.h"

// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////


/**
 * 	List of device parameters
 *
 * @note 	User shall provide parameter name here as it would be using
 * 			later inside code.
 *
 * @note 	User shall change code only inside section of "USER_CODE_BEGIN"
 * 			ans "USER_CODE_END".
 *
 * 			Must be started with 0!
 */
typedef enum
{
	// USER CODE START...

	ePAR_BTN_1	= 0,
	ePAR_BTN_2,
	ePAR_BTN_3,
	ePAR_BTN_4,

	ePAR_AIN_1,
	ePAR_AIN_2,
	ePAR_AIN_4,
	ePAR_AIN_5,
	ePAR_AIN_6,
	ePAR_AIN_7,

	// USER CODE END...

	ePAR_NUM_OF
} par_num_t;


// USER CODE BEGIN...

/**
 * 	Enable/Disable multiple access protection
 */
#define PAR_CFG_MUTEX_EN						( 0 )

/**
 * 	Enable/Disable storing persistent parameters to NVM
 */
#define PAR_CFG_NVM_EN							( 0 )

#if ( 1 == PAR_CFG_NVM_EN )
	/**
	 * 	NVM parameter region option
	 *
	 * 	@note 	User shall select region based on nvm_cfg.h region
	 * 			definitions "nvm_region_name_t"
	 *
	 * 			Don't care if "PAR_CFG_NVM_EN" set to 0
	 */
	#define PAR_CFG_NVM_REGION						( eNVM_REGION_EEPROM_RUN_PAR )

	/**
	 * 	Enable/Disable parameter table unique ID checking
	 *
	 * @note	Base on hash unique ID is being calculated with
	 * 			purpose to detect device and stored parameter table
	 * 			difference.
	 *
	 * 			Must be disabled once the device is release in order
	 * 			to prevent loss of calibrated data stored in NVM.
	 *
	 * 	@pre	"PAR_CFG_NVM_EN" must be enabled otherwise does
	 * 			not make sense to calculating ID at all.
	 */
	#define PAR_CFG_TABLE_ID_CHECK_EN				( 0 )

	#ifndef DEBUG
	#undef PAR_CFG_TABLE_ID_CHECK_EN
	#define PAR_CFG_TABLE_ID_CHECK_EN 0
	#endif
#endif

/**
 * 	Enable/Disable debug mode
 *
 * 	@note	Disable in release!
 */
#define PAR_CFG_DEBUG_EN						( 1 )

#ifndef DEBUG
#undef PAR_CFG_DEBUG_EN
#define PAR_CFG_DEBUG_EN 0
#endif

/**
 * 	Enable/Disable assertions
 *
 * 	@note	Disable in release!
 */
#define PAR_CFG_ASSERT_EN						( 0 )

#ifndef DEBUG
#undef PAR_CFG_ASSERT_EN
#define PAR_CFG_ASSERT_EN 0
#endif

/**
 * 	Debug communication port macros
 */
#if ( 1 == PAR_CFG_DEBUG_EN )
	#define PAR_DBG_PRINT( ... )				( cli_printf( __VA_ARGS__ ))
#else
	#define PAR_DBG_PRINT( ... )				{ ; }

#endif

/**
 * 	 Assertion macros
 */
#if ( 1 == PAR_CFG_ASSERT_EN )
	#define PAR_ASSERT(x)						PROJECT_CONFIG_ASSERT(x)
#else
	#define PAR_ASSERT(x)						{ ; }
#endif

// USER CODE END...

/**
 * 	Invalid configuration catcher
 *
 * 	@note	Shall be intact by end user!
 */
#if ( 0 == PAR_CFG_NVM_EN ) && ( 1 == PAR_CFG_TABLE_ID_CHECK_EN )
	#error "Parameter settings invalid: Disable table ID checking (PAR_CFG_TABLE_ID_CHECK_EN)!"
#endif

////////////////////////////////////////////////////////////////////////////////
// Functions Prototypes
////////////////////////////////////////////////////////////////////////////////
const void * 	par_cfg_get_table		(void);
uint32_t	 	par_cfg_get_table_size	(void);

#endif // _PAR_CFG_H_
