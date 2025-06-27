/*
 * sc_system.c
 *
 * Smartcard Demo OS
 *
 * T. Music 05/2023
 *
 */

#include <stddef.h>

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>

#include "sc_system.h"

uint8_t channel_array[] = { 1, 1, ADC_CHANNEL_TEMP};

static void delay_cyc(uint32_t cycles) {
    // The loop translates to roughly two cycles,
    // so just div cycles by two and we good
    cycles = cycles >> 1;
    while (cycles > 0) {
        cycles--;
        __asm__("nop");
    }
}

/*
    Function sys_gpio_init()

    Configures all GPIOs on the card and puts them into a predefined state

*/
void sys_gpio_init() {
    // Enable the GPIO periphal clocks
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    // Configure the LEDs
    // Set the GPIOs (LEDs are inverted)
    // Red and green led share a GPIO port
    gpio_set(PORT_LED_R, PIN_LED_R | PIN_LED_G); 
    // Blue has its own GPIO port
    gpio_set(PORT_LED_B, PIN_LED_B);
    gpio_mode_setup(PORT_LED_R, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED_R | PIN_LED_G);
    gpio_set_output_options(PORT_LED_R, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, PIN_LED_R | PIN_LED_G);
    gpio_mode_setup(PORT_LED_B, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED_B);
    gpio_set_output_options(PORT_LED_B, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, PIN_LED_B);

    // 7816 Reset Disable Pin
    // By default this pin is strapped low on the pcb. Thus, if a card reader wants to
    // perform a reset, the STM32 will reset, too. In case you are debugging, you might not
    // want your card to be reset everytime the reader sees a timeout.
    gpio_clear(PORT_ISO7816_RST_DIS, PIN_ISO7816_RST_DIS);
    gpio_mode_setup(PORT_ISO7816_RST_DIS, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_ISO7816_RST_DIS);
    gpio_set_output_options(PORT_ISO7816_RST_DIS, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_ISO7816_RST_DIS);

    // Clock Source Select Pin CLKSEL
    // When low, the STM32 receives the card reader clock on its HSE pin
    // This means, when selecting HSE as clock source, the controller runs with the wonky reader clock
    // this may be favorable for transmitting the ATR and further communication, but unfavorable for
    // performing measurements. Thus, this signal is high by default.
    gpio_set(PORT_CLKSEL, PIN_CLKSEL);
    gpio_mode_setup(PORT_CLKSEL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_CLKSEL);
    gpio_set_output_options(PORT_CLKSEL, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIN_CLKSEL);

    // Trigger Pin
    gpio_clear(PORT_TRIGGER, PIN_TRIGGER);
    gpio_mode_setup(PORT_TRIGGER, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_TRIGGER);
    gpio_set_output_options(PORT_TRIGGER, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, PIN_TRIGGER);

}

void sys_set_reset_source(uint8_t src) {
    if (src != SYSTEM_RSTSRC_ISO7816 && src != SYSTEM_RSTSRC_DBG_ONLY) {
        // Invalid source has been selected -> exit
        return;
    }

    if (src == SYSTEM_RSTSRC_ISO7816)
        gpio_clear(PORT_ISO7816_RST_DIS, PIN_ISO7816_RST_DIS);
    else
        gpio_set(PORT_ISO7816_RST_DIS, PIN_ISO7816_RST_DIS);

    return;
}

void sys_rcc_init() {
    // Some general settings for the clock configuration

    // Also the card will never have clock speeds > 8 MHz, so keep the
    // flash wait states at zero
    flash_set_ws(FLASH_ACR_LATENCY_0WS);

    // At this time we are running from the HSI, so 8 MHz
    rcc_ahb_frequency  = 8000000;
    rcc_apb1_frequency = 8000000;
    rcc_apb2_frequency = 8000000;
}

// Set the desired clock source for the system.
// Changing the system clock requires adjusting divider
// settings throughout the system for peripherals like USART
void sys_set_clksource(uint8_t src) {
    if (src != SYSTEM_CLKSOURCE_CARD_TERMINAL && src != SYSTEM_CLKSOURCE_EXT_8MHZ) {
        // Invalid source has been selected -> exit
        return;
    }

    // As both clock options are being received through the HSE pin,
    // hop on the internal oscillator for the time between changing
    rcc_osc_on(RCC_HSI);
    rcc_wait_for_osc_ready(RCC_HSI);
    rcc_set_sysclk_source(RCC_HSI);

    rcc_osc_off(RCC_HSE);
    // Bypass the oscillator -> we are supplied by a stable clock
    RCC_CR |= RCC_CR_HSEBYP;

    // The desired clock source is the card terminal
    if (src == SYSTEM_CLKSOURCE_CARD_TERMINAL) {
        // Set CLKSEL pin low to disable the pcb oscillator
        // and pass through the card terminal clock
        gpio_clear(PORT_CLKSEL, PIN_CLKSEL);
    } else {
        gpio_set(PORT_CLKSEL, PIN_CLKSEL);
    }

    // Re-enable the HSE
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);
    rcc_set_sysclk_source(RCC_HSE);

    // Turn off the HSI
    rcc_osc_off(RCC_HSI);

    return;
}

void adc_init(void)
{
	rcc_periph_clock_enable(RCC_ADC);
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

	adc_power_off(ADC1);
	adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
	adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_071DOT5);
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
	adc_disable_analog_watchdog(ADC1);
	adc_power_on(ADC1);

	/* Wait for ADC starting up. */
	// int i;
	// for (i = 0; i < 800000; i++) {    /* Wait a bit. */
	// 	__asm__("nop");
	// }

}

void sys_usart_init() {
    /*
        Note:
        Not using so many functions of the libopencm3 as, e.g., the usart
        functions for modesetting do only utilize USART_CR1, whereas some mode
        settings we need are in USART_CR3. To avoid any misdirections during
        config, write to the regs directly. Libopencm3 functions only utillized
        for very basics like baudrate and stoppbits.
    */
    rcc_periph_clock_enable(RCC_USART1); 
    gpio_mode_setup(PORT_USART1_TX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, PIN_USART1_TX);
    gpio_set_af(PORT_USART1_TX, GPIO_AF1, PIN_USART1_TX);

    // Set the clock source (this should be done outside of this function)
    // sys_set_clksource(SYSTEM_CLKSOURCE_EXT_8MHZ);


    // Set USART Baudrate
    usart_set_baudrate(USART1, 12800); // Set USARTDIV=625 for 12800 baud

    // Enable Smart Card Mode (properly configure USART control registers)

    // Set 1.5 stop bits
    usart_set_stopbits(USART1, USART_STOPBITS_1_5);

    // Configure communication settings with 8 data bits and parity
    usart_set_databits(USART1, 9); // 9 = 8 data + 1 parity bits
    // Set parity (odd or even, according to documentation)
    usart_set_parity(USART1, USART_PARITY_EVEN);

    // Enable USART for both transmission and reception
    usart_set_mode(USART1, USART_MODE_TX_RX);
    USART_CR3(USART1) |= USART_CR3_HDSEL;
    usart_enable(USART1);
    
    return;
}

uint8_t adc_read_random_byte(void) {
    // ADC1'deki belirli bir kanaldan okuma yap
    adc_start_conversion_regular(ADC1);
    while (!adc_eoc(ADC1)); // Dönüşümün bitmesini bekle

    uint16_t raw_adc_value = adc_read_regular(ADC1); // Ham ADC değerini oku

    // 12-bit ADC değerini 8-bit bir değere dönüştür
    return (uint8_t)(raw_adc_value & 0xFF);
}


void sys_usart_send(uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        usart_send_blocking(USART1, data[i]);
        (void)(uint8_t)usart_recv_blocking(USART1);
        delay_cyc(1);
    }
}

void sys_usart_receive(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (uint8_t)usart_recv_blocking(USART1);
    }
}

// void sys_usart_send_insByte(uint8_t *data) {

//     usart_send_blocking(USART1, data[1]);
//     (void)(uint8_t)usart_recv_blocking(USART1);

// }

// void sys_usart_send_SW_Byte() {
//     uint8_t data[] = {0x61, 0x10};

//     for (size_t i = 0; i < 2; i++) {
//         usart_send_blocking(USART1, data[i]);
//         (void)(uint8_t)usart_recv_blocking(USART1);
//         delay_cyc(5);
//     }

// }

// void sys_usart_send_atr() {

//     uint8_t atr[] = { 
//         // correct atr sequence
//         0x3B, // TS
//         0x90, // T0
//         0x11, // TA1
//         0x00 // TD1
//     };
//     size_t atr_len = sizeof(atr) / sizeof(atr[0]);

//     sys_usart_send(atr, atr_len);
//     return;
// }



