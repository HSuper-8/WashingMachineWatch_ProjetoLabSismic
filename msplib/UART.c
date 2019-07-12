#include "UART.h"

void UCA3_Config(uint32_t baudRate, int parity, int isMSB, int nStops){
    UCA3CTLW0   = UCSWRST;

    UCA3CTLW0  |= UCSSEL__SMCLK;

    uint32_t brDiv = (1000000<<8)/baudRate;
    UCA3BRW = brDiv>>8;
    UCA3MCTLW = (brDiv & 0xFF)<<8;

    P6SEL0 |=  (BIT0|BIT1);
    P6SEL1 &= ~(BIT0|BIT1);

    UCA3CTLW0  &= ~UCSWRST;

    UCA3IE |= UCRXIE;
    __enable_interrupt();
}

void UCA3_Send(unsigned char data){
    while(!(UCA3IFG & UCTXIFG));  // Waits for TXBUF to be empty
    UCA3TXBUF = data;             // Sends data
}

void UCA3_SendStr(char* str){
    int i;
    int size = strlen(str);
    for(i = 0; i < size; i++){
        UCA3_Send(str[i]);
    }
}

char UCA3_Get(){
    while(!(UCA3IFG & UCRXIFG));
    return UCA3RXBUF;
}


