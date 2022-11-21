// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/*!
*@file      version_cfg.h
*@brief     Configuration file for revision of software & hardware
*@author    Ziga Miklosic
*@date      27.05.2022
*@version	V1.2.0
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup VERSION_CFG
* @{ <!-- BEGIN GROUP -->
*
* 	Configuration for revision
*/
////////////////////////////////////////////////////////////////////////////////
#ifndef __VERSION_CFG_H_
#define __VERSION_CFG_H_

// USER CODE BEGIN...


/******************************************************************************
 * 	SET SOFTWARE VERSION
 *****************************************************************************/
#define VER_SW_MAJOR		0
#define VER_SW_MINOR		1
#define VER_SW_DEVELOP		1
#define VER_SW_TEST			0

/******************************************************************************
 * 	SET HARDWARE VERSION
 *****************************************************************************/
#define VER_HW_MAJOR		1
#define VER_HW_MINOR		0
#define VER_HW_DEVELOP		0
#define VER_HW_TEST			0


/******************************************************************************
 * 	APPLICAION HEADER SETTINGS
 *****************************************************************************/

/**
 * 	Valid app header signature
 *
 * 	@note	Must be 32-bit is size!
 */
#define VER_APP_HEAD_SIGN				((uint32_t)( 0xFACEC0DEUL ))

/**
 * 	Section name of app header
 */
#define VER_APP_HEAD_SECTION			( ".app_header" )


/******************************************************************************
 * 	PROJECT INFORMATIONS SETTINGS
 *****************************************************************************/

/**
 * 	Enable/Disable project informations
 */
#define VER_CFG_USE_PROJ_INFO_EN		( 0 )

/**
 * 	Project info max. size
 *
 * 	Unit: bytes
 */
#define VER_APP_PROJ_INFO_SIZE			( 2048 )

/**
 * 	Project info section
 */
#define VER_APP_PROJ_INFO_SECTION 		( ".proj_info" )


// USER CODE END...

#endif // __VERSION_CFG_H_

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
