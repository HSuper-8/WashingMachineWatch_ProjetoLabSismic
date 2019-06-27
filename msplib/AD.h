#ifndef     AD_H
#define     AD_H

#include <msp430.h>

#define ADC12SSEL__SMCLK    ADC12SSEL_3
#define SingleModeSC          0
#define SingleTimer           1

void config_adc(int mode);
void config_timer(void);

#endif

