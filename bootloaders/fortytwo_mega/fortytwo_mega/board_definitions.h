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

#define BOOT_USART_MODULE					SERCOM6
#define BOOT_USART_BUS_CLOCK_INDEX			MCLK_APBDMASK_SERCOM6
#define BOOT_USART_PER_CLOCK_INDEX			SERCOM6_GCLK_ID_CORE
#define BOOT_USART_PAD_SETTINGS				UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3						PINMUX_UNUSED
#define BOOT_USART_PORT3					2
#define BOOT_USART_PAD2						PINMUX_UNUSED
#define BOOT_USART_PORT2					2
#define BOOT_USART_PAD1						PINMUX_PC07C_SERCOM6_PAD1
#define BOOT_USART_PORT1					2
#define BOOT_USART_PAD0						PINMUX_PC06C_SERCOM6_PAD0
#define BOOT_USART_PORT0					2
#define BOOT_USART_IRQn						SERCOM6_IRQn

/* Master clock frequency */
#define CPU_FREQUENCY						(48000000ul)
#define VARIANT_MCK							CPU_FREQUENCY

/*
 * LEDs definitions - PB31
 */
#define BOARD_LED_PORT						1
#define BOARD_LED_PIN						31

#endif /* BOARD_DEFINITIONS_H_ */