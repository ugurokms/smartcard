/*
 * Main.c
 *
 * Smartcard Demo OS
 *
 * T. Music 05/2023
 *
 */


// These includes may be useless for the demo os,
// but of use for you.
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "sc_system.h"
#include "adc.h"

// #include "aes_inv.c"
// #include "aes_inv_masked.c"
// #include "aes_inv_dummy.c"
// #include "aes_inv_shuffle.c"
#include "aes_inv_dummy_shuffle_mask.c"


int main(void) {

    uint8_t rxHeaderBuff[5];
    uint8_t rxDataBuff[16];


    sys_rcc_init();
    // Init the GPIOs so we can config
    // the clock source and ISO7816 reset behavior
    sys_gpio_init();
    sys_set_clksource(SYSTEM_CLKSOURCE_EXT_8MHZ);
    sys_set_reset_source(SYSTEM_RSTSRC_DBG_ONLY);

    sys_usart_init();
    
    adc_init();

    // gpio_clear(PORT_TRIGGER, PIN_TRIGGER);
    // sys_usart_send_atr();
    sys_usart_send((uint8_t[]){0x3B, 0x90, 0x11, 0x00}, 4);
    


    while(1) {

        // receive header
        sys_usart_receive(rxHeaderBuff, 5);
        // send instruction byte
        // sys_usart_send_insByte(rxHeaderBuff);
        sys_usart_send((uint8_t[]){rxHeaderBuff[1]}, 1);

        // receive encrypted data
        sys_usart_receive(rxDataBuff, 16);

        // decrypt data
        gpio_set(PORT_TRIGGER, PIN_TRIGGER);
        aes128_decrypt(rxDataBuff);
        gpio_clear(PORT_TRIGGER, PIN_TRIGGER);

        // send SW byte indicating decryption is done
        // sys_usart_send_SW_Byte();
        sys_usart_send((uint8_t[]){0x61, 0x10}, 2);

        // receive header
        sys_usart_receive(rxHeaderBuff, 5);
        
        // send instruction byte
        // sys_usart_send_insByte(rxHeaderBuff);
        sys_usart_send((uint8_t[]){rxHeaderBuff[1]}, 1);

        // send decrypted data
        sys_usart_send(rxDataBuff, 16);

        // send completion bytes
        sys_usart_send((uint8_t[]){0x90, 0x00}, 2);

    }

    // Never reached
    return 0;
}
