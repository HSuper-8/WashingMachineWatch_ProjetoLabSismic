#include <msp430.h> 
#include "esp.h"
#include "UART.h"

/**
 * main.c
 */
int main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 ^= LOCKLPM5;        // High-impedance treatment

	esp_config();

	__enable_interrupt();

	esp_init();
	esp_sendemail();
	return 0;
}




