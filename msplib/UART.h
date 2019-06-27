#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

#define PARITY_NONE                                         0
#define PARITY_EVEN                                         1
#define PARITY_ODD                                          2
#define RESET_MSB                                           0
#define SET_MSB                                             1
#define RXIN                                                P4IN
#define RXBIT                                               BIT2
#define TXOUT                                               P4OUT
#define TXDIR                                               P4DIR
#define TXBIT                                               BIT1

void UCA3_Config(int baudRate, int parity,
                 int isMSB, int nStops);

void UCA3_Send(uint8_t data);

void UART_BbSend(uint8_t data);

uint8_t UART_BbGet();

#endif


