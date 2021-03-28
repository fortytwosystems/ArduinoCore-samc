/*
 * board_definitions.h
 *
 * Created: 1/18/2021 10:27:01 PM
 *  Author: david
 */ 


#ifndef BOARD_DEFINITIONS_H_
#define BOARD_DEFINITIONS_H_

#include "sam.h"
#include "board_driver_serial.h"

#define SAM_BA_UART_ONLY				// Since there's no USB on SAMC21, just build the USART bootloader stuff.

/*
 * If BOOT_DOUBLE_TAP_ADDRESS is defined the bootloader is started by
 * quickly tapping two times on the reset button.
 * BOOT_DOUBLE_TAP_ADDRESS must point to a free SRAM cell that must not
 * be touched from the loaded application.
 */
#define BOOT_DOUBLE_TAP_ADDRESS				(0x20007FFCul)
#define BOOT_DOUBLE_TAP_DATA				(*((volatile uint32_t *) BOOT_DOUBLE_TAP_ADDRESS))

#define BOOT_USART_MODULE					SERCOM5
#define BOOT_USART_BUS_CLOCK_INDEX			MCLK_APBCMASK_SERCOM5
#define BOOT_USART_PER_CLOCK_INDEX			SERCOM5_GCLK_ID_CORE
#define BOOT_USART_PAD_SETTINGS				UART_RX_PAD3_TX_PAD2
#define BOOT_USART_PAD3						PINMUX_PB23D_SERCOM5_PAD3
#define BOOT_USART_PORT3					1
#define BOOT_USART_PAD2						PINMUX_PB22D_SERCOM5_PAD2
#define BOOT_USART_PORT2					1
#define BOOT_USART_PAD1						PINMUX_UNUSED
#define BOOT_USART_PORT1					1
#define BOOT_USART_PAD0						PINMUX_UNUSED
#define BOOT_USART_PORT0					1
#define BOOT_USART_IRQn						SERCOM5_IRQn

/* Master clock frequency */
#define CPU_FREQUENCY						(48000000ul)
#define VARIANT_MCK							CPU_FREQUENCY

/*
 * LEDs definitions - PA17
 */
#define BOARD_LED_PORT						0
#define BOARD_LED_PIN						17

#endif /* BOARD_DEFINITIONS_H_ */