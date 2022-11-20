// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      cli_if.c
*@brief     Interface with Command Line Interface
*@author    Ziga Miklosic
*@date      11.09.2022
*@version   V1.0.0
*/
////////////////////////////////////////////////////////////////////////////////
/*!
* @addtogroup CLI_IF
* @{ <!-- BEGIN GROUP -->
*
*
* 	@note	Change code only between "USER_CODE_BEGIN" and
* 			"USER_CODE_END" section!
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "cli_if.h"
#include "cli_cfg.h"

// USER CODE BEGIN...


#include "drivers/peripheral/uart/uart_dbg.h"

#include "nrf_nvic.h"

#include "drivers/peripheral/usb_cdc/usb_cdc.h"

// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

// USER CODE BEGIN..


// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

// USER CODE BEGIN...


// USER CODE END...

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*		Initialize Command Line Interface communication port
*
* @return 		status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_init(void)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...

/*	if ( eUART_DBG_OK != uart_dbg_init())
	{
		status = eCLI_ERROR_INIT;
	}
*/
	if ( eUSB_CDC_OK != usb_cdc_init())
	{
		status = eCLI_ERROR_INIT;
	}


	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		De-initialize Command Line Interface communication port
*
* @return 		status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_deinit(void)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...


	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Receive data
*
* @param[out]	p_data	- Pointer to received data
* @return 		status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_receive(uint8_t * const p_data)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...

	/*if ( eUART_DBG_OK != uart_dbg_get((char*) p_data))
	{
		status = eCLI_ERROR;
	}*/

	if ( eUSB_CDC_OK != usb_cdc_get((char*) p_data))
	{
		status = eCLI_ERROR;
	}

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Transmit data
*
* @param[in]	p_data	- Pointer to transmit data
* @return 		status 	- Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_transmit(const uint8_t * const p_data)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...

	/*if ( eUART_DBG_OK != uart_dbg_write((const char*) p_data))
	{
		status = eCLI_ERROR;
	}*/

	if ( eUSB_CDC_OK != usb_cdc_write((const char*) p_data))
	{
		status = eCLI_ERROR;
	}

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Acquire mutex
*
* @note	User shall provide definition of that function based on used platform!
*
*		If not being used leave empty.
*
*		This function does not have an affect if "CLI_CFG_MUTEX_EN"
* 		is set to 0.
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_aquire_mutex(void)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...


	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Release mutex
*
* @note	User shall provide definition of that function based on used platform!
*
*		If not being used leave empty.
*
*		This function does not have an affect if "CLI_CFG_MUTEX_EN"
* 		is set to 0.
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_release_mutex(void)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...

	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*		Reset device
*
* @note	User shall provide definition of that function based on used platform!
*
*		If not being used leave empty.
*
* @return 		status - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
cli_status_t cli_if_device_reset(void)
{
	cli_status_t status = eCLI_OK;

	// USER CODE BEGIN...

	// Reset device
	NVIC_SystemReset();


	// USER CODE END...

	return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
