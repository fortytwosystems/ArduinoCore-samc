/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#include <samc21.h>
#include "variant.h"

#include <stdio.h>

/**
 * \brief SystemInit() configures the needed clocks and according Flash Read Wait States.
 */

// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)
#define GENERIC_CLOCK_GENERATOR_OSC32K    (1u)
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /* Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)
#define GENERIC_CLOCK_GENERATOR_TIMERS    (5u)  /* Used by the timers for controlling PWM frequency. Can be up to 48MHz (up to 96MHz with the D51). */

// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_FDPLL   (0u)
#define GENERIC_CLOCK_MULTIPLEXER_FDPLL_32K (1u)

void waitForPLL( void )
{
  while ( OSCCTRL->DPLLSYNCBUSY.reg & OSCCTRL_DPLLSYNCBUSY_MASK );
}

void waitForSync( void )
{
	while ( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK );
}

void ClkSystemInit( void )
{
  /* Set 1 Flash Wait State for 48MHz (2 for the L21 and C21), cf tables 20.9 and 35.27 in SAMD21 Datasheet
	* The D51 runs with 5 wait states at 120MHz (automatic).
	* Disable automatic NVM write operations (errata reference 13134, applies to D21/D11/L21, but not C21 or D51)
	*/

	NVMCTRL->CTRLB.reg = (NVMCTRL_CTRLB_RWS_DUAL | NVMCTRL_CTRLB_MANW) ; // two wait states


	MCLK->APBAMASK.reg |= MCLK_APBAMASK_GCLK ;

	/* ----------------------------------------------------------------------------------------------
	* Software reset the GCLK module to ensure it is re-initialized correctly
	*/

	GCLK->CTRLA.reg = GCLK_CTRLA_SWRST ;

	while ( (GCLK->CTRLA.reg & GCLK_CTRLA_SWRST) && (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_MASK) );	/* Wait for reset to complete */


	/* ----------------------------------------------------------------------------------------------
	* Enable XOSC32K clock (External on-board 32.768Hz crystal oscillator)
	*/
	#define DPLLRATIO_LDR       2928u
	#define DPLLRATIO_LDRFRAC   11u

	OSC32KCTRL->XOSC32K.reg = (OSC32KCTRL_XOSC32K_STARTUP( 0x4u ) | OSC32KCTRL_XOSC32K_XTALEN | OSC32KCTRL_XOSC32K_EN32K | OSC32KCTRL_XOSC32K_EN1K);
	OSC32KCTRL->XOSC32K.bit.ENABLE = 1;

	while ( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 );       /* Wait for oscillator stabilization */

	OSCCTRL->DPLLRATIO.reg = ( OSCCTRL_DPLLRATIO_LDR(DPLLRATIO_LDR) | OSCCTRL_DPLLRATIO_LDRFRAC(DPLLRATIO_LDRFRAC) );  /* set PLL multiplier */
	waitForPLL();

	OSCCTRL->DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK(0);  /* select 32KHz crystal input */

	OSCCTRL->DPLLPRESC.reg = 0;
	waitForPLL();

	OSCCTRL->DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
	waitForPLL();

	while ( (OSCCTRL->DPLLSTATUS.reg & OSCCTRL_DPLLSTATUS_CLKRDY) != OSCCTRL_DPLLSTATUS_CLKRDY );


	MCLK->CPUDIV.reg  = MCLK_CPUDIV_CPUDIV_DIV1 ;

	/* Switch Generic Clock Generator 0 to 96MHz PLL output. The output is divided by two to obtain a 48MHz CPU clock. */
	GCLK->GENCTRL[0].reg = ( GCLK_GENCTRL_DIV(2) | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN );
	waitForSync();

	SystemCoreClock=VARIANT_MCK;

}
