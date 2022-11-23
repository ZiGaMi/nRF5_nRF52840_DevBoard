// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      cli_cfg.h
*@brief     Command Line Interface Configurations
*@author    Ziga Miklosic
*@date      11.09.2022
*@version   V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup CLI_CFG
* @{ <!-- BEGIN GROUP -->
*
* 	@note	Change code only between "USER_CODE_BEGIN" and
* 			"USER_CODE_END" section!
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLI_CFG_H
#define __CLI_CFG_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>

// USER CODE BEGIN...

#include "project_config.h"

#include "revision/revision/src/version.h"

// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

// USER CODE BEGIN...

/**
 * 	Enable/Disable intro string
 *
 * 	@brief	Intro string will be transmitted at "cli_init()" function call,
 * 			if low level com driver are initialized OK.
 */
#define CLI_CFG_INTRO_STRING_EN					( 1 )

#if ( 1 == CLI_CFG_INTRO_STRING_EN )

	/**
	 * 	Intro strings
	 */
	#define CLI_CFG_INTRO_PROJECT_NAME			((const char*) "nRF52840 Dev Board Base Code" )
	#define CLI_CFG_INTRO_SW_VER				((const char*) version_get_sw_str())
	#define CLI_CFG_INTRO_HW_VER				((const char*) version_get_hw_str())
	#define CLI_CFG_INTRO_PROJ_INFO				((const char*) version_get_proj_info_str())
#endif

/**
 * 	Termination string
 *
 * 	@note	Each "cli_printf" will have termination string appended to
 * 			its back.
 *
 * 			If not used leave empty. E.g. ""
 */
#define CLI_CFG_TERMINATION_STRING				((const char*) ( "\r\n" ))

/**
 * 	Transmission buffer size in bytes
 */
#define CLI_CFG_TX_BUF_SIZE						( 512 )

/**
 * 	Reception buffer size in bytes
 */
#define CLI_CFG_RX_BUF_SIZE						( 512 )

/**
 * 	Maximum number of commands within a single table
 *
 * @note	Space for table command is allocated statically in compile time therefore
 * 			take special care defining that number. It is recommended to set
 * 			to a minimum as needed.
 *
 * 	@note 	Size of single table will increase with CLI_CFG_MAX_NUM_OF_COMMANDS
 * 			macro by following formula:
 *
 * 			size_of_cmd_table = ( 12 * CLI_CFG_MAX_NUM_OF_COMMANDS ) [bytes]
 */
#define CLI_CFG_MAX_NUM_OF_COMMANDS				( 10 )

/**
 * 	Maximum number of user defined command tables
 *
 * @note	Space for table command is allocated statically in compile time therefore
 * 			take special care defining that number. It is recommended to set
 * 			to a minimum as needed.
 *
 * 	@note 	Total size of all command table  will increase with CLI_CFG_MAX_NUM_OF_COMMANDS
 * 			and CLI_CFG_MAX_NUM_OF_USER_TABLES macro by following formula:
 *
 * 			tatal_size_of_all_cmd_tables = ( CLI_CFG_MAX_NUM_OF_USER_TABLES * (( 12 * CLI_CFG_MAX_NUM_OF_COMMANDS [bytes] ) + 4 )) [bytes]
 *
 * 	@note
 */
#define CLI_CFG_MAX_NUM_OF_USER_TABLES			( 8 )

/**
 * 	Enable/Disable multiple access protection
 */
#define CLI_CFG_MUTEX_EN						( 0 )

/**
 * 	Enable/Disable parameters usage
 *
 * 	@brief	Usage of device parameters.
 * 			Link to repository: https://github.com/GeneralEmbeddedCLibraries/parameters
 */
#define CLI_CFG_PAR_USE_EN						( 1 )

#if ( 1 == CLI_CFG_PAR_USE_EN )

	/**
	 *    	Time period of "cli_hndl()" function call
	 *
	 *    @note		Used for parameter streaming purposes!
	 *
	 *    Unit: ms
	 */
	#define CLI_CFG_HNDL_PERIOD_MS          	( 10UL )

#endif

/**
 * 	Enable/Disable debug mode
 */
#define CLI_CFG_DEBUG_EN						( 1 )

/**
 * 	Enable/Disable assertions
 */
#define CLI_CFG_ASSERT_EN						( 1 )

/**
 * 	 Assertion macros
 */
#if ( 1 == CLI_CFG_ASSERT_EN )
	#define CLI_ASSERT(x)						PROJECT_CONFIG_ASSERT(x)
#else
	#define CLI_ASSERT(x)						{ ; }
#endif

/**
 * 		List of communication channels
 *
 * @note	Warning and error communication channels must
 * 			always be present!
 *
 * 	@note	Change code only between "USER_CODE_BEGIN" and
 * 			"USER_CODE_END" section!
 */
typedef enum
{
	eCLI_CH_WAR = 0,		/**<Warning channel */
	eCLI_CH_ERR,			/**<Error channel */

	// USER_CODE_BEGIN

	eCLI_CH_APP,

	// USER_CODE_END

	eCLI_CH_NUM_OF			/**<Leave unchange - Must be last! */
} cli_ch_opt_t;

// USER CODE END...

/**
 * 	CLI Communication Channels
 */
typedef struct
{
	const char * 	name;	/**<Name of channel */
	bool			en;		/**<Enable flag */
} cli_cfg_ch_data_t;

/**
 * 	Invalid module configuration catcher
 */
#if ( CLI_CFG_TX_BUF_SIZE < 32 )
	#error "CLI Transmisson buffer size must be larger than 32 bytes! Adjust <CLI_CFG_TX_BUF_SIZE> macro inside cli_cfg.h file!"
#endif

#if ( CLI_CFG_RX_BUF_SIZE < 32 )
	#error "CLI Reception buffer size must be larger than 32 bytes! Adjust <CLI_CFG_RX_BUF_SIZE> macro inside cli_cfg.h file!"
#endif


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////
const char* cli_cfg_get_ch_name	(const cli_ch_opt_t ch);
const bool 	cli_cfg_get_ch_en	(const cli_ch_opt_t ch);
void 		cli_cfg_set_ch_en	(const cli_ch_opt_t ch, const bool en);

#if ( 1 == CLI_CFG_PAR_USE_EN )
	const char * cli_cfg_get_par_groupe_str(const uint32_t par_num);
#endif

#endif // __CLI_CFG_H

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
