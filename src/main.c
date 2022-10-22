/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

//#include "boards.h"


#include "pin_mapper.h"

#include "nrf_drv_systick.h"
#include "nrf_gpio.h"


static volatile uint32_t gu32_systick = 0;

void SysTick_Handler(void)
{
  gu32_systick++;

   nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_4__PORT, LED_4__PIN ) );
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Init systick
   // nrf_drv_systick_init();

    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_1__PORT, LED_1__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_2__PORT, LED_2__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_3__PORT, LED_3__PIN ) );
    nrf_gpio_cfg_output( NRF_GPIO_PIN_MAP( LED_4__PORT, LED_4__PIN ) );
    
  SysTick->LOAD  = (uint32_t)(( SystemCoreClock / 1000 ) - 1UL);                         /* set reload register --> 1ms */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */



    /* Configure board. */
   // bsp_board_init(BSP_INIT_LEDS);


   uint32_t cnt = gu32_systick;
   uint32_t cnt_prev = cnt;

  // NOTE: Systick counts down, therefore it would be beter to initiate systick IRQ and handle there

    /* Toggle LEDs. */
    while (true)
    {

      cnt = gu32_systick;

      if (((uint32_t)( cnt - cnt_prev )) >= 1000 )
      {
          cnt_prev = cnt;

          nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_1__PORT, LED_1__PIN ) );
          nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_2__PORT, LED_2__PIN ) );
          nrf_gpio_pin_toggle( NRF_GPIO_PIN_MAP( LED_3__PORT, LED_3__PIN ) );
         
      }






     // nrf_delay_ms( 1000 );

      /*
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            bsp_board_led_invert(i);
            nrf_delay_ms(1000);
        }
        */
    }
}

/**
 *@}
 **/
