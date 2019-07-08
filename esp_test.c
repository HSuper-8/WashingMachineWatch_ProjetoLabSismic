#include <msp430.h> 
#include "esp.h"
#include "UART.h"

/**
 * main.c
 */
int main(void){
	WDTCTL = WDTPW | WDTHOLD;	     // stop watchdog timer
	PM5CTL0 ^= LOCKLPM5;             // High-impedance treatment

	esp_config();

	esp_cmd("AT\r\n", 1000, "OK");	// AT—Tests AT Startup
	
	return 0;
}




