/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2015 Atmel Corporation/Thibaut VIARD.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <sam.h>
#include "sam_ba_monitor.h"
#include "sam_ba_serial.h"
#include "board_definitions.h"
#include "board_driver_led.h"

extern uint32_t __sketch_vectors_ptr; // Exported value from linker script
extern void ClkSystemInit();

volatile uint32_t* pulSketch_Start_Address;

#define APP_START_ADDRESS 0x2000

void delayUs (uint32_t delay)
{
  /* The SAMD21 has a default 1MHz clock @ reset.
   * SAMC21 clock has been set to 48MHz before this function is used.
   */
  
  for (uint32_t i=0; i < delay; i++)
    /* force compiler to not optimize this... */
    __asm__ __volatile__("");
}

/**
 * \brief Check the application startup condition
 *
 */
static void check_start_application(void)
{
	/*
	* Test sketch stack pointer @ &__sketch_vectors_ptr
	* Stay in SAM-BA if value @ (&__sketch_vectors_ptr) == 0xFFFFFFFF (Erased flash cell value)
	*/
	if (__sketch_vectors_ptr == 0xFFFFFFFF)
	{
		/* Stay in bootloader */
		return;
	}

	/*
	* Load the sketch Reset Handler address
	* __sketch_vectors_ptr is exported from linker script and point on first 32b word of sketch vector table
	* First 32b word is sketch stack
	* Second 32b word is sketch entry point: Reset_Handler()
	*/
	pulSketch_Start_Address = &__sketch_vectors_ptr ;
	pulSketch_Start_Address++ ;

	/*
	* Test vector table address of sketch @ &__sketch_vectors_ptr
	* Stay in SAM-BA if this function is not aligned enough, ie not valid
	*/
	if ( ((uint32_t)(&__sketch_vectors_ptr) & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
	{
		/* Stay in bootloader */
		return;
	}

	#define DOUBLE_TAP_MAGIC 0x07738135
	
	if (RSTC->RCAUSE.bit.POR)
	{
		/* On power-on initialize double-tap */
		BOOT_DOUBLE_TAP_DATA = 0;
	}
	else
	{
		if (BOOT_DOUBLE_TAP_DATA == DOUBLE_TAP_MAGIC)
		{
			/* Second tap, stay in bootloader */
			BOOT_DOUBLE_TAP_DATA = 0;
			return;
		}

		/* First tap */
		BOOT_DOUBLE_TAP_DATA = DOUBLE_TAP_MAGIC;

		/* Wait 0.5sec to see if the user tap reset again.
			* The loop value is based on SAMD21 default 1MHz clock @ reset.
			*/
		delayUs(500000UL);

		/* Timeout happened, continue boot... */
		BOOT_DOUBLE_TAP_DATA = 0;
	}

	uint32_t app_start_address;

	/* Load the Reset Handler address of the application */
	app_start_address = *(uint32_t *)(APP_START_ADDRESS + 4);

	/**
	 * Test reset vector of application @APP_START_ADDRESS+4
	 * Stay in SAM-BA if *(APP_START+0x4) == 0xFFFFFFFF
	 * Application erased condition
	 */
	if (app_start_address == 0xFFFFFFFF) {
		/* Stay in bootloader */
		return;
	}

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) APP_START_ADDRESS);

	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

	/* Jump to application Reset Handler in the application */
	asm("bx %0"::"r"(app_start_address));

}

/**
 *  \brief SAMD21 SAM-BA Main loop.
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Jump in application if condition is satisfied */
	check_start_application();

	/* We have determined we should stay in the monitor. */
	/* System initialization */
	
	__enable_irq();
  
	/* Let's change the clock to 48MHz so we have more time 
		for controlling the LED and communicating via serial */
	ClkSystemInit();
  
	/* UART is enabled in all cases */
	serial_open();

	/* Initialize LEDs */
	LED_init();
  
	/* For some reason modifying this value makes the serial stop working */
	SysTick_Config(1000);

	/* Wait for a complete enum on usb or a '#' char on serial line */
	while (1)
	{
		/* Check if a '#' has been received */
		if (serial_sharp_received())
		{
			sam_ba_monitor_init(SAM_BA_INTERFACE_USART);
			/* SAM-BA on Serial loop */
			while(1)
			{
				sam_ba_monitor_run();
			}
		}
	}
}

void SysTick_Handler(void)
{
	LED_pulse();
}
