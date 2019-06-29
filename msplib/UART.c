#include "UART.h"

void UCA3_Config(int baudRate, int parity, int isMSB, int nStops){
    UCA3CTLW0  = UCSWRST;

    UCA3CTLW0  = (isMSB?  UCMSB : 0)               |
                 (parity? UCPEN : 0)               |
                 (parity == PARITY_EVEN? UCPAR : 0)|
                 (nStops == 2? UCSPB : 0)          |
                  UCSSEL__SMCLK                    |
                  UCSWRST;

    uint32_t brDiv = (1000000<<3)/baudRate;
    UCA3BRW = brDiv>>3;
    UCA3MCTLW = (brDiv & 0x07)<<1;

    P6SEL0 |=  (BIT0|BIT1);
    P6SEL1 &= ~(BIT0|BIT1);

    UCA3CTLW0  &= ~UCSWRST;
}

void UCA3_Send(uint8_t data){
    while(!(UCA3IFG & UCTXIFG));
    UCA3TXBUF = data;
}

uint8_t UCA3_Get(){
    while(!(UCA3IFG & UCRXIFG));
    return UCA3RXBUF;
}

/*
void UART_BbSend(uint8_t data){
    // 9600 bps, no parity, LSB_first, 1 bit stop
    TA0CTL = TASSEL__SMCLK| TACLR|
             MC__UP;

    TA0CCR0 = 104 -1;
    TXDIR |= TXBIT;

    // Bit start
    TXOUT &= ~TXBIT;

    while(!(TA0CTL & TAIFG));
    TA0CTL &= ~TAIFG;

    // Transmit 8 bits
    uint8_t i = 8, mask = BIT0;

    while(i--){

        if(data & mask){
            TXOUT |= TXBIT;
        }

        else{
            TXOUT &= ~TXBIT;
        }

        while(!(TA0CTL & TAIFG));
        TA0CTL &= ~TAIFG;

        mask <<= 1;
    }

    // Bit stop

    TXOUT |= TXBIT;

    while(!(TA0CTL & TAIFG));
    TA0CTL &= ~TAIFG;

    TA0CTL &= ~MC__UP;
}

uint8_t UART_BbGet(){
    TA0CTL = TASSEL__SMCLK| TACLR|
             MC__STOP;

    TA0CCR0 = 104/2 -1;
    while(RXIN & RXBIT);
    TA0CTL |= MC__UP;

    while(!(TA0CTL & TAIFG));
    TA0CTL &= ~TAIFG;

    // Glitch catch

    if(RXIN & RXBIT){
        return 0;
    }

    TA0CCR0 = 104 -1;

    // 8 Bits receiver

    uint8_t i = 8, mask = BIT0, data = 0;

    while(i--){

        while(!(TA0CTL & TAIFG));
        TA0CTL &= ~TAIFG;

        if(RXIN & RXBIT){
            data |= mask;
        }

        mask <<= 1;
    }

    while(!(TA0CTL & TAIFG));
    TA0CTL &= ~TAIFG;

    return data;
}

*/


