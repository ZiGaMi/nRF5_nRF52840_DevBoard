// Copyright (c) 2022 Ziga Miklosic
// All Rights Reserved
// This software is under MIT licence (https://opensource.org/licenses/MIT)
////////////////////////////////////////////////////////////////////////////////
/**
*@file      app.c
*@author    Ziga Miklosic
*@date      22.10.2022
*@version   V1.0.0  (nRF5)
*/
////////////////////////////////////////////////////////////////////////////////
/**
*@addtogroup APP
* @{ <!-- BEGIN GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>

#include "nrf_drv_systick.h"
#include "systick.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/**
 *    Frequency of systick interrupt handler
 *
 *  Unit: Hz
 */
#define SYSTICK_PERIOD_HZ               ( 1000UL )    

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

/**
 *    Systick counter
 */
static volatile uint32_t gu32_systick_cnt = 0;

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/**
*       Systick ISR handler
*
* @return   void
*/
////////////////////////////////////////////////////////////////////////////////
void SysTick_Handler(void)
{
    gu32_systick_cnt++;
}

////////////////////////////////////////////////////////////////////////////////
/**
*       Initialize systick
*
* @return   status  - Status of operation
*/
////////////////////////////////////////////////////////////////////////////////
systick_status_t systick_init(void)
{
    systick_status_t status = eSYSTICK_OK;

    // Set load register
    SysTick->LOAD  = (uint32_t)(( SystemCoreClock / SYSTICK_PERIOD_HZ ) - 1UL);                      
    
    // Set interrupt priority
    NVIC_SetPriority ( SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL ); 
    
    // Clear counter
    SysTick->VAL = 0UL;                                            
   
    // Enable IRQ and start timer
    SysTick->CTRL = ( SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk );                        

    return status;
}

////////////////////////////////////////////////////////////////////////////////
/**
*       Get system tick counts
*
* @return   gu32_systick_cnt  - Miliseconds system tick counter
*/
////////////////////////////////////////////////////////////////////////////////
const uint32_t systick_get_ms(void)
{
    return (const uint32_t) gu32_systick_cnt;
}

////////////////////////////////////////////////////////////////////////////////
/**
* @} <!-- END GROUP -->
*/
////////////////////////////////////////////////////////////////////////////////
