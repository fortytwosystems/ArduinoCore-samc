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

#include "Arduino.h"
#include "wiring_private.h"

bool dacEnabled[] = {false, false};

// Wait for synchronization of registers between the clock domains
static __inline__ void syncDAC() __attribute__((always_inline, unused));
static void syncDAC() {
  while ( DAC->SYNCBUSY.reg & DAC_SYNCBUSY_MASK );
}

int pinPeripheral( uint32_t ulPin, EPioType ulPeripheral )
{
  // Prevent out of bounds access
  if (ulPin >= PINS_COUNT)
  {
    return -1 ;
  }

  uint8_t pinPort = GetPort(ulPin);
  uint8_t pinNum = GetPin(ulPin);
  uint8_t peripheralAttribute = g_APinDescription[ulPin].ulPeripheralAttribute;
  uint8_t pinType = g_APinDescription[ulPin].ulPinType;
  uint32_t pinAttribute = g_APinDescription[ulPin].ulPinAttribute;

  // Handle the case the pin isn't usable as PIO
  if ( pinPort == NOT_A_PORT )
  {
    return -1 ;
  }

  // If pinType is not PIO_MULTI or PIO_STARTUP in the pinDescription table, then it must match ulPeripheral
  if ( pinType != PIO_MULTI && pinType != PIO_STARTUP && pinType != ulPeripheral )
  {
    return -1 ;
  }

  // Make sure ulPeripheral is listed in pinAttribute
  if ( !(pinAttribute & (1UL << ulPeripheral)) && pinType != PIO_STARTUP )
  {
    return -1 ;
  }

  // Determine hardware peripheral to use
  EPioPeripheral peripheral = PER_PORT;
  switch ( ulPeripheral )
  {
    case PIO_EXTINT:

      if ( GetExtInt(ulPin) == NOT_AN_INTERRUPT )
      {
        return -1 ;
      }

      peripheral = PER_EXTINT;
      
      break;

    case PIO_ANALOG_ADC:

      if ( GetADC(ulPin) == No_ADC_Channel )
      {
        return -1 ;
      }
      peripheral = PER_ANALOG;
    
      break;

    case PIO_ANALOG_DAC:
    case PIO_ANALOG_REF:
      peripheral = PER_ANALOG;
    
      break;

    case PIO_TIMER_PWM:
    case PIO_TIMER_CAPTURE:
      if ( g_APinDescription[ulPin].ulTCChannel == NOT_ON_TIMER )
      {
        return -1 ;
      }

      if ( (peripheralAttribute & PER_ATTR_TIMER_MASK) == PER_ATTR_TIMER_STD )
      {
        peripheral = PER_TIMER;
      }
      else
      {
        peripheral = PER_TIMER_ALT;
      }

      break;

    case PIO_SERCOM:
      if ( (peripheralAttribute & PER_ATTR_SERCOM_MASK) == PER_ATTR_SERCOM_STD )
      {
        peripheral = PER_SERCOM;
      }
      else
      {
        peripheral = PER_SERCOM_ALT;
      }
      break;

    case PIO_COM:
    case PIO_CAN:
      peripheral = PER_COM;
      break;

    case PIO_AC_GCLK:
      peripheral = PER_AC_CLK;
      break;

    case PIO_NOT_A_PIN:
    case PIO_MULTI:
      return -1l;
      break;

    default:
      break;
  }

  // Disable DAC if setting pin to anything other than DAC. If analogWrite() was used previously, the DAC is enabled.
  // Note that on the L21, the DAC output would interfere with other peripherals if left enabled, even if the analog peripheral is not selected
  if ( ((pinPort == 0) && (pinNum == 2)) && ulPeripheral != PIO_ANALOG_DAC )
  {
    if (dacEnabled[0]) {
      dacEnabled[0] = false;
      //syncDAC();
      DAC->CTRLA.bit.ENABLE = 0x00; // Disable DAC
      //DAC->CTRLB.bit.EOEN = 0x00; // The DAC output is turned off.
      syncDAC();
    }
  }

  uint8_t pinCfg = (PORT->Group[pinPort].PINCFG[pinNum].reg & PORT_PINCFG_PULLEN);   // Preserve state of pullup/pulldown enable, clear the rest of the bits

  // INEN should be enabled for both input and output (but not analog)
  if ( ulPeripheral != PIO_ANALOG_ADC && ulPeripheral != PIO_ANALOG_DAC && ulPeripheral != PIO_ANALOG_REF )
  {
    pinCfg |= PORT_PINCFG_INEN;
  }

  // Set pin drive strength (DRVSTR), which is used with PIO_OUTPUT and PIO_SERCOM (UART, SPI, and I2C)
  if ( (peripheralAttribute & PER_ATTR_DRIVE_MASK) == PER_ATTR_DRIVE_STRONG )
  {
    pinCfg |= PORT_PINCFG_DRVSTR;
  }

  noInterrupts(); // Avoid possible invalid interim pin state

  if ( ulPeripheral == PIO_INPUT || ulPeripheral == PIO_STARTUP )
  {
    PORT->Group[pinPort].DIRCLR.reg = (uint32_t)(1<<pinNum) ;
    PORT->Group[pinPort].OUTSET.reg = (uint32_t)(1<<pinNum) ;	// set default pull direction to pullup (will not be enabled)
    pinCfg &= ~PORT_PINCFG_PULLEN;      // Disable pull resistor
  }
  else if ( ulPeripheral == PIO_INPUT_PULLUP || ulPeripheral == PIO_INPUT_PULLDOWN )
  {
    PORT->Group[pinPort].DIRCLR.reg = (uint32_t)(1<<pinNum) ;

    if ( ulPeripheral == PIO_INPUT_PULLDOWN ) {
      PORT->Group[pinPort].OUTCLR.reg = (uint32_t)(1<<pinNum) ;
    } else {
      PORT->Group[pinPort].OUTSET.reg = (uint32_t)(1<<pinNum) ;
    }

    pinCfg |= PORT_PINCFG_PULLEN;
  }
  else if ( ulPeripheral == PIO_OUTPUT )
  {
    PORT->Group[pinPort].DIRSET.reg = (uint32_t)(1<<pinNum) ;
    pinCfg &= ~PORT_PINCFG_PULLEN;      // Disable pull resistor
  }
  else	// pin will be controlled by a peripheral (PMUXEN will be set)
  {
    if ( pinNum & 1 ) // is pin odd?
    {
      // Get whole current setup for both odd and even pins and remove odd one, then set new muxing
      uint32_t temp = (PORT->Group[pinPort].PMUX[pinNum >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
      PORT->Group[pinPort].PMUX[pinNum >> 1].reg = temp|PORT_PMUX_PMUXO( peripheral ) ;
    }
    else // even pin
    {
      // Get whole current setup for both odd and even pins and remove even one, then set new muxing
      uint32_t temp = (PORT->Group[pinPort].PMUX[pinNum >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
      PORT->Group[pinPort].PMUX[pinNum >> 1].reg = temp|PORT_PMUX_PMUXE( peripheral ) ;
    }

    pinCfg |= PORT_PINCFG_PMUXEN; // Enable peripheral mux
  }

  // Set pin drive strength, enable/disable pull resistor, enable/disable INEN, and enable/disable the peripheral mux
  PORT->Group[pinPort].PINCFG[pinNum].reg = (uint8_t)pinCfg ;

  interrupts();
  return 0l ;
}

