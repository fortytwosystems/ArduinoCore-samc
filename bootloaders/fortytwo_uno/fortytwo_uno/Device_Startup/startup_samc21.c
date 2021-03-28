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
#include <stdio.h>

/* RTOS Hooks */
extern void svcHook(void);
extern void pendSVHook(void);
extern int sysTickHook(void);

/* Default empty handler */
void Dummy_Handler(void) {
#if defined DEBUG
  __BKPT(3);
#endif
  for (;;) { }
}

/* Cortex-M0+ core handlers */
void NonMaskableInt_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void Reset_Handler (void);
void SVCall_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void SYSTEM_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* MCLK, OSC32KCTRL, OSCCTRL, PAC, PM, SUPC, TAL */
void WDT_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void RTC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EIC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void FREQM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TSENS_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DMAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

#ifdef SAMC21N
void INT9_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* SERCOM0, SERCOM6 */
void INT10_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM1, SERCOM7 */
#else
void SERCOM0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif

void SERCOM2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM5_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void CAN0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void CAN1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TCC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TCC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TCC2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

#ifdef SAMC21N
void INT20_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC0, TC5 */
void INT21_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC1, TC6 */
void INT22_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC2, TC7 */
#else 
void TC0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC0, TC5 */
void TC1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC1, TC6 */
void TC2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TC2, TC7 */
#endif

void TC3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void ADC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void ADC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void AC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SDADC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PTC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

/* Initialize segments */
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sfixed;
extern uint32_t _estack;

/* Exception Table */
__attribute__((section(".vectors"))) 
const DeviceVectors exception_table = {

    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .pvStack                = (void*) (&_estack),

    .pfnReset_Handler       = (void*) Reset_Handler,
    .pfnNonMaskableInt_Handler = (void*) NonMaskableInt_Handler,
    .pfnHardFault_Handler   = (void*) HardFault_Handler,
    .pvReservedM12          = (void*) (0UL), /* Reserved */
    .pvReservedM11          = (void*) (0UL), /* Reserved */
    .pvReservedM10          = (void*) (0UL), /* Reserved */
    .pvReservedM9           = (void*) (0UL), /* Reserved */
    .pvReservedM8           = (void*) (0UL), /* Reserved */
    .pvReservedM7           = (void*) (0UL), /* Reserved */
    .pvReservedM6           = (void*) (0UL), /* Reserved */
    .pfnSVCall_Handler      = (void*) SVCall_Handler,
    .pvReservedM4           = (void*) (0UL), /* Reserved */
    .pvReservedM3           = (void*) (0UL), /* Reserved */
    .pfnPendSV_Handler      = (void*) PendSV_Handler,
    .pfnSysTick_Handler     = (void*) SysTick_Handler,

    /* Configurable interrupts */
    .pfnSYSTEM_Handler      = (void*) SYSTEM_Handler,         /*  0 Main Clock, 32k Oscillators Control, Oscillators Control, Peripheral Access Controller, Power Manager, Supply Controller, Trigger Allocator */
    .pfnWDT_Handler         = (void*) WDT_Handler,            /*  1 Watchdog Timer */
    .pfnRTC_Handler         = (void*) RTC_Handler,            /*  2 Real-Time Counter */
    .pfnEIC_Handler         = (void*) EIC_Handler,            /*  3 External Interrupt Controller */
    .pfnFREQM_Handler       = (void*) FREQM_Handler,          /*  4 Frequency Meter */
    .pfnTSENS_Handler       = (void*) TSENS_Handler,          /*  5 Temperature Sensor */
    .pfnNVMCTRL_Handler     = (void*) NVMCTRL_Handler,        /*  6 Non-Volatile Memory Controller */
    .pfnDMAC_Handler        = (void*) DMAC_Handler,           /*  7 Direct Memory Access Controller */
    .pfnEVSYS_Handler       = (void*) EVSYS_Handler,          /*  8 Event System Interface */

#ifdef SAMC21N
    .pfnINT9_Handler        = (void*) INT9_Handler,           /*  9 Serial Communication Interface 0, Serial Communication Interface 6 */
    .pfnINT10_Handler       = (void*) INT10_Handler,          /* 10 Serial Communication Interface 1, Serial Communication Interface 7 */
#else
    .pfnSERCOM0_Handler        = (void*) SERCOM0_Handler,           /*  9 Serial Communication Interface 0, Serial Communication Interface 6 */
    .pfnSERCOM1_Handler       = (void*) SERCOM1_Handler,          /* 10 Serial Communication Interface 1, Serial Communication Interface 7 */
#endif

    .pfnSERCOM2_Handler     = (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
    .pfnSERCOM3_Handler     = (void*) SERCOM3_Handler,        /* 12 Serial Communication Interface 3 */
    .pfnSERCOM4_Handler     = (void*) SERCOM4_Handler,        /* 13 Serial Communication Interface 4 */
    .pfnSERCOM5_Handler     = (void*) SERCOM5_Handler,        /* 14 Serial Communication Interface 5 */
    .pfnCAN0_Handler        = (void*) CAN0_Handler,           /* 15 Control Area Network 0 */
    .pfnCAN1_Handler        = (void*) CAN1_Handler,           /* 16 Control Area Network 1 */
    .pfnTCC0_Handler        = (void*) TCC0_Handler,           /* 17 Timer Counter Control 0 */
    .pfnTCC1_Handler        = (void*) TCC1_Handler,           /* 18 Timer Counter Control 1 */
    .pfnTCC2_Handler        = (void*) TCC2_Handler,           /* 19 Timer Counter Control 2 */

#ifdef SAMC21N
    .pfnINT20_Handler       = (void*) INT20_Handler,          /* 20 Basic Timer Counter 0, Basic Timer Counter 5 */
    .pfnINT21_Handler       = (void*) INT21_Handler,          /* 21 Basic Timer Counter 1, Basic Timer Counter 6 */
    .pfnINT22_Handler       = (void*) INT22_Handler,          /* 22 Basic Timer Counter 2, Basic Timer Counter 7 */
#else
    .pfnTC0_Handler         = (void*) TC0_Handler,            /* 23 Basic Timer Counter 3 */
    .pfnTC1_Handler         = (void*) TC1_Handler,            /* 24 Basic Timer Counter 4 */
    .pfnTC2_Handler         = (void*) TC2_Handler,            /* 23 Basic Timer Counter 3 */
#endif

    .pfnTC3_Handler         = (void*) TC3_Handler,            /* 23 Basic Timer Counter 3 */
    .pfnTC4_Handler         = (void*) TC4_Handler,            /* 24 Basic Timer Counter 4 */
    .pfnADC0_Handler        = (void*) ADC0_Handler,           /* 25 Analog Digital Converter 0 */
    .pfnADC1_Handler        = (void*) ADC1_Handler,           /* 26 Analog Digital Converter 1 */
    .pfnAC_Handler          = (void*) AC_Handler,             /* 27 Analog Comparators */
    .pfnDAC_Handler         = (void*) DAC_Handler,            /* 28 Digital Analog Converter */
    .pfnSDADC_Handler       = (void*) SDADC_Handler,          /* 29 Sigma-Delta Analog Digital Converter */
    .pfnPTC_Handler         = (void*) PTC_Handler             /* 30 Peripheral Touch Controller */
};

extern int main(void);

/* This is called on processor reset to initialize the device and call main() */
void Reset_Handler(void)
{
  uint32_t *pSrc, *pDest;

  /* Initialize the relocate segment */
  pSrc = &_etext;
  pDest = &_srelocate;

  if (pSrc != pDest) {
	  for (; pDest < &_erelocate;) {
		  *pDest++ = *pSrc++;
	  }
  }

  /* Clear the zero segment */
  for (pDest = &_szero; pDest < &_ezero;) {
	  *pDest++ = 0;
  }

  /* Set the vector table base address */
  pSrc = (uint32_t *) & _sfixed;
  SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

  main();

  while (1)
    ;
}

/**
 * Initial system clock frequency. The System RC Oscillator (RCSYS) provides
 *  the source for the main clock at chip startup.
 */
#define __SYSTEM_CLOCK (48000000)

uint32_t SystemCoreClock = __SYSTEM_CLOCK; /*!< System Clock Frequency (Core Clock)*/