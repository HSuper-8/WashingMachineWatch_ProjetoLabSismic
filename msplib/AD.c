#include "AD.h"

void config_adc(int mode){
    ADC12CTL0  &= ~ADC12ENC;        //Configuration's disable

    ADC12CTL0 = ADC12SHT0_3 |       //ADC sample time
                ADC12ON;            //ADC-on

    // Chooses AD sampling mode
    switch (mode){
    case SingleModeSC:
        ADC12CTL1 = ADC12SHS_0;     // ADCSC12
        break;

    case SingleTimer:
        ADC12CTL1 = ADC12SHS_1;     // Timer TA0.1
        break;
    default: break;

    }

    ADC12CTL1 = ADC12SHP         |  // Sample/Hold use stimer
                ADC12DIV_0       |  // Clock ADC Divisor = 1
                ADC12SSEL_0      |  // Clock ADC = MODOSC
                ADC12CONSEQ_1;      // Sequence mode

    ADC12CTL2 = ADC12RES_2;         // 12-bit resolution

    ADC12CTL3 = ADC12CSTARTADD_0;  // Sampling in ADC12MEM0

    ADC12MCTL0 &= ~ADC12DIF;
    ADC12MCTL0 = ADC12INCH_4;       //(P1.4) A4 = Channel 4

    ADC12MCTL1 &= ~ADC12DIF;
    ADC12MCTL1 = ADC12EOS    |      // End of sequence
                 ADC12INCH_5;       //(P1.5) A5 = Channel 5;

    ADC12IER0 = ADC12IE0;
    ADC12IER1 = ADC12IE1;

    P1SEL0 |= BIT4;
    P1SEL1 |= BIT4;

    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;

    ADC12CTL0 |= ADC12ENC;
}

void config_timer(void){
    TA0CTL = TACLR|TASSEL__ACLK | MC__UP;
    TA0CCR0 = 3000;                 // 1/8 seg
    TA0CCTL0 = CCIE;
}

