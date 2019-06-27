#include "esp.h"
#include "../msplib/UART.h"


// BRCLK 		Baud Rate 	UCOS16 		UCBRx 	UCBRFx 		UCBRSx
// 1000000 	    115200 		0 			8 		- 			0xD6

void esp_config(){
	uartInit(baudRate, 0, 0, 0);
}

void esp_init(){
	esp_cmd("AT");					// AT—Tests AT Startup 
	esp_cmd("AT+CIPMODE=3");		// 0: normal transmission mode. 
	//esp_cmd("AT+CIPMUX=0");			// 0: single connection 
	esp_cmd("AT+CWQAP");			// Disconnects from the AP 
	esp_cmd("AT+CWJAP=\"" SSID "\",\""password"\"");  // connecting wifi
	esp_cmd("AT+CIFSR");

	//esp_cmd(AT+CIPSTART=\"TCP\",\"" SRV_ADDR "\"," SRV_PORT); 
	// TCP: connection type: TCP
	// SRV_ADDR (defined) remote IP address
	// SRV_PORT (defined)  remote port number
}

void esp_cmd(const char * cmd){
	// Ver Como envia um comando 
	// byte por byte ?
}

void esp_sendemail(){
	esp_cmd("AT+CIPMUX=1");
	esp_cmd("AT + CIPSERVER = 1,"SRV_PORT);
	esp_cmd("AT+CIPSTART=4\"TCP\",\"" EMAILFROM "\"," EMAILFROM_PORT); 
	esp_cmd("AT + CIPSEND = 4,"NBYTES_MESSAGE);   //

	// Verificar o porquê dessa forma
	esp_cmd("EHLO 192.168.1.123");
	esp_cmd("AUTH LOGIN");
    esp_cmd("c2FkZGFtNDIwMUBnbWFpbC5jb20=");    // email ase 64 format
    esp_cmd(" tu934yt89ert83=");				// password base 64 format
    esp_cmd("MAIL FROM:<saddam4201@gmail.com>");
    esp_cmd("RCPT To:<saddam4201@gmail.com>");
    esp_cmd("DATA");
    esp_cmd(MESSAGE);
    esp_cmd("AT+CIPSEND=6");
    esp_cmd("QUIT");
}




