#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init(void);
uint8_t adc_read_random_byte(void);

#endif // ADC_H
