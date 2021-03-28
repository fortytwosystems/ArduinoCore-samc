/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"

#define RAMSTART (HMCRAMC0_ADDR)
#define RAMSIZE  (HMCRAMC0_SIZE)
#define RAMEND   (RAMSTART + RAMSIZE - `1)

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// Include Atmel headers
#undef LITTLE_ENDIAN
#include <samc21.h>

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

#include "WVariant.h"

#ifdef __cplusplus
} // extern "C"
#endif

// Include board variant
#include "variant.h"

#define interrupts()    __enable_irq()
#define noInterrupts()  __disable_irq()

// Interrupts
#define digitalPinToInterrupt(P)   ( P )

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#define abs(x) ((x)>0?(x):-(x))

#ifndef ARDUINO_ARCH_SAMC
#define ARDUINO_ARCH_SAMC
#endif

#define TIMER_732Hz

/*
 * \brief Analog reference selection.
 * For eAnalogReference values <= 5, the value is written to the REFSEL register.
 * For values > 5 (SAMD51, SAML and SAMC only), 0 is written into the REFSEL register,
 * and the SUPC_VREF_SEL (supply controller) register value is: (eAnalogReference - 6).
 * With the D51, INTVCC1 and INTVCC2 as used in Arduino are actually INTVCC0 and
 * INTVCC1 in the datasheet.
 */
typedef enum _eAnalogReference
{
  AR_INTREF = 0,              // This has the same effect as AR_INTREF_1V024
  AR_INTERNAL_INTVCC0 = 1,    // Vcc*(1/1.6)
  AR_INTERNAL_INTVCC1 = 2,    // Vcc/2
  AR_EXTERNAL_REFA = 3,
  AR_EXTERNAL_DAC = 4,
  AR_INTERNAL_INTVCC2 = 5,    // Vcc
  AR_INTREF_1V024 = 6,
  AR_INTREF_2V048 = 7,
  AR_INTREF_4V096 = 8,
  AR_DEFAULT = AR_INTERNAL_INTVCC2,
  AR_INTERNAL1V0 = AR_INTREF,
  AR_INTERNAL = AR_INTERNAL_INTVCC0,        // 2.06V only when Vcc = 3.3V and only with the L21/C21
  AR_INTERNAL2V06 = AR_INTERNAL_INTVCC0,        // 2.06V only when Vcc = 3.3V and only with the L21/C21
  AR_INTERNAL1V65 = AR_INTERNAL_INTVCC1,	// 1.65V only when Vcc = 3.3V
  AR_EXTERNAL = AR_EXTERNAL_REFA,
} eAnalogReference ;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
 *
 * \param res
 */
extern void analogReadResolution(int res);

/*
 * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
 *
 * \param res
 */
extern void analogWriteResolution(int res);

extern void analogOutputInit( void ) ;

#ifdef __cplusplus
}
#endif

// ARM toolchain doesn't provide itoa etc, provide them
#include "api/itoa.h"

#endif // Arduino_h
