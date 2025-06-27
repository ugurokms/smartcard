// #include <libopencm3/stm32/rcc.h>
// #include <libopencm3/stm32/gpio.h>
// #include <libopencm3/stm32/adc.h>

// void adc_init(void) {
//     // RCC yapılandırması
//     rcc_periph_clock_enable(RCC_GPIOA); // GPIOA için saat sinyalini etkinleştir
//     rcc_periph_clock_enable(RCC_ADC);   // ADC için saat sinyalini etkinleştir

//     // GPIO yapılandırması
//     gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0); // PA0 pinini analog modda yapılandır

//     // ADC yapılandırması
//     adc_power_off(ADC1);
//     adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
//     adc_set_resolution(ADC1, ADC_RESOLUTION_12BIT);
//     adc_set_sample_time_on_all_channels(ADC1, ADC_SMPTIME_239DOT5);
//     adc_power_on(ADC1);

//     for (int i = 0; i < 8000; i++) {    /* Wait a bit. */
//         __asm__("nop");
//     }
// }

// uint8_t adc_read_random_byte(void) {
//     // ADC1'deki belirli bir kanaldan okuma yap
//     adc_start_conversion_regular(ADC1);
//     while (!adc_eoc(ADC1)); // Dönüşümün bitmesini bekle

//     uint16_t raw_adc_value = adc_read_regular(ADC1); // Ham ADC değerini oku

//     // 12-bit ADC değerini 8-bit bir değere dönüştür
//     return (uint8_t)(raw_adc_value & 0xFF);
// }