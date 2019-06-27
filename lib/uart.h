#include <stdio.h>
#include <stdint.h>
#include <msp430.h> 

#define baudRate 115200

void uartInit(int br, int isMSB, int parity, int isEven, int nStops, int mode, int isSync);
void uartSendByte(uint8_t byte);
void uartSendString(char* string, int end_string);
uint8_t uartGet(uint8_t *data);