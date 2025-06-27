/*
 * sc_system.h
 *
 * Smartcard Demo OS
 *
 * T. Music 05/2023
 *
 */


#ifndef __SC_SYSTEM_H__
#define __SC_SYSTEM_H__

#include <stddef.h>

#include <libopencm3/stm32/gpio.h>

#define SYSTEM_CLKSOURCE_CARD_TERMINAL 0
#define SYSTEM_CLKSOURCE_EXT_8MHZ      1

#define SYSTEM_RSTSRC_ISO7816  0
#define SYSTEM_RSTSRC_DBG_ONLY 1

/* System definitions for the Smartcard V3 Hardware (STM32F051) */
#if defined(SMARTCARD_HW_V3)

#define PORT_LED_R   GPIOA
#define PORT_LED_G   GPIOA
#define PORT_LED_B   GPIOB

#define PIN_LED_R    GPIO6
#define PIN_LED_G    GPIO7
#define PIN_LED_B    GPIO0

#define PORT_TRIGGER GPIOA
#define PIN_TRIGGER  GPIO4

#define PORT_ISO7816_RST_DIS GPIOB
#define PIN_ISO7816_RST_DIS  GPIO1

#define PORT_ISO7816_RST_SNS GPIOB
#define PIN_ISO7816_RST_SNS  GPIO2

#define PORT_CLKSEL GPIOA
#define PIN_CLKSEL  GPIO15

#define PORT_USART1_TX GPIOA
#define PIN_USART1_TX  GPIO9

#else
#error "Hardware not defined! Please define either SMARTCARD_HW_V2_0 or SMARTCARD_HW_V3_0"
#endif

void sys_rcc_init(void);
void sys_gpio_init(void);
void sys_usart_init(void);
void adc_init(void);

void sys_usart_send(uint8_t *data, size_t len);
void sys_usart_receive(uint8_t *buffer, size_t length);
// void sys_usart_send_atr(void);
// void sys_usart_send_insByte(uint8_t *data);
// void sys_usart_send_SW_Byte(void);
void sys_set_clksource(uint8_t src);

void sys_set_reset_source(uint8_t src);
uint8_t adc_read_random_byte(void);

// void delay_cyc(uint32_t cycles);

#endif
