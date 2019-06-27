#include "uart.h"

// mode = 0, Sync = 0

void uartInit(int br, int isMSB, int parity, int nStops){

  UCA3CTLW0 = UCSWRST;  // Pronto para confiurar
  UCA3CTLW0 |= (isMSB ? BITD : 0) |
              (parity ? BITF : 0) | // 0 no parity, 1 par, 2 ímpar
              (parity == 1 ? BITE : 0) |
              (nStops == 2 ? BITB : 0);
	
	UCA3CTLW0 |= UCSSEL__SMCLK;

  uint32_t brDiv = (1000000 << 3)/br; // Boudrate Divider. Desloca 3 bits da parte fracionária
  UCA3BRW = brDiv >>3;                // tira a parte fracionaria

  UCA3MCTLW = (brDiv & 0x07) << 1;    // brDiv & 0x07 : parte fracionaria

  UCA3CTLW0 &= ~UCSWRST;
  //UCA3IE |= UCRXIE;

	P6DIR  |=  BIT0;
  P6SEL0 |=  BIT0;
  P6SEL1 &= ~BIT0;   // Primary module function is selected

  P6DIR  &= ~BIT1;
  P6SEL0 |=  BIT1;
  P6SEL1 &= ~BIT1;
}


void uartSendByte(uint8_t byte){
  while(!(UCA3IFG & UCTXIFG));  
  UCA3TXBUF = byte;            
}

void uartSendString(char* string){
  int i;
  int size = strlen(string);
  for(i = 0; i < size; i++){
    uartSendByte(string[i++]);
  }
  if(end_string){
    uartSendByte('\0'); 
  }
}

uint8_t uartGet(uint8_t *data){
    TA2CTL = TASSEL__ACLK | MC__UP | ID__4;
    TA2CCR0 = 0xFFFF; // 8 segundos

    while(!(UCA3IFG & RXIFG) && !(TA2CTL & TAIFG));
    if(TA2CTL & TAIFG) return -1;

    *data = UCA3RXBUF;
    return 0;
}
