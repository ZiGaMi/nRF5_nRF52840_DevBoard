// Copyright (c) 2021 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      par_cfg.c
*@brief     Configuration for device parameters
*@author    Ziga Miklosic
*@date      22.05.2021
*@version	V1.3.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup PAR_CFG
* @{ <!-- BEGIN GROUP -->
*
* 	Configuration for device parameters
*
* 	User shall put code inside inside code block start with
* 	"USER_CODE_BEGIN" and with end of "USER_CODE_END".
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "par_cfg.h"
#include "parameters/src/par.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 *	Parameters definitions
 *
 *	@brief
 *
 *	Each defined parameter has following properties:
 *
 *		i) 		Parameter ID: 	Unique parameter identification number. ID shall not be duplicated.
 *		ii) 	Name:			Parameter name. Max. length of 32 chars.
 *		iii)	Min:			Parameter minimum value. Min value must be less than max value.
 *		iv)		Max:			Parameter maximum value. Max value must be more than min value.
 *		v)		Def:			Parameter default value. Default value must lie between interval: [min, max]
 *		vi)		Unit:			In case parameter shows physical value. Max. length of 32 chars.
 *		vii)	Data type:		Parameter data type. Supported types: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t and float32_t
 *		viii)	Access:			Access type visible from external device such as PC. Either ReadWrite or ReadOnly.
 *		ix)		Persistence:	Tells if parameter value is being written into NVM.
 *
 *
 *	@note	User shall fill up wanted parameter definitions!
 *
 *	@note 	Version V1.0.1 parameters store to NVM based on ID numbers (fixed addresses)!
 *
 *			For this configuration of nvm size=1024 max persistent par ID must be less than 128
 */
static const par_cfg_t g_par_table[ePAR_NUM_OF] =
{

	// USER CODE BEGIN...

	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//							ID			Name						Min 				Max 					Def 					Unit				Data type				PC Access					Persistent				Description
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	[ePAR_BTN_1]		= 	{	.id = 0, 	.name = "User button 1",	.min.u8 = 0 ,		.max.u8 = 1,			.def.u8 = 0	,			.unit = NULL,		.type = ePAR_TYPE_U8,	.access = ePAR_ACCESS_RO, 	.persistant = false,	.desc = "State of user button 1. 0-idle | 1-pressed"		},
	[ePAR_BTN_2]		= 	{	.id = 1, 	.name = "User button 2",	.min.u8 = 0 ,		.max.u8 = 1,			.def.u8 = 0	,			.unit = NULL,		.type = ePAR_TYPE_U8,	.access = ePAR_ACCESS_RO, 	.persistant = false,	.desc = "State of user button 2. 0-idle | 1-pressed"		},
	[ePAR_BTN_3]		= 	{	.id = 2, 	.name = "User button 3",	.min.u8 = 0 ,		.max.u8 = 1,			.def.u8 = 0	,			.unit = NULL,		.type = ePAR_TYPE_U8,	.access = ePAR_ACCESS_RO, 	.persistant = false,	.desc = "State of user button 3. 0-idle | 1-pressed"		},
	[ePAR_BTN_4]		= 	{	.id = 3, 	.name = "User button 4",	.min.u8 = 0 ,		.max.u8 = 1,			.def.u8 = 0	,			.unit = NULL,		.type = ePAR_TYPE_U8,	.access = ePAR_ACCESS_RO, 	.persistant = false,	.desc = "State of user button 4. 0-idle | 1-pressed"		},

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


	// USER CODE END...
};

/**
 * 	Table size in bytes
 */
static const uint32_t gu32_par_table_size = sizeof( g_par_table );

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Get parameter configuration table
*
* @return		pointer to configuration table
*/
////////////////////////////////////////////////////////////////////////////////
const void * par_cfg_get_table(void)
{
	return (const par_cfg_t*) &g_par_table;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Get configuration table size in bytes
*
* @return	gu32_par_table_size	- Size of table in bytes
*/
////////////////////////////////////////////////////////////////////////////////
uint32_t par_cfg_get_table_size(void)
{
	return gu32_par_table_size;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
