#include "esp.h"
#include "../msplib/UART.h"


// BRCLK 		Baud Rate 	UCOS16 		UCBRx 	UCBRFx 		UCBRSx
// 1000000 	    115200 		0 			8 		- 			0xD6

void esp_config(){
	UCA3_Config(baudRate, 0, 0, 1);
}

void esp_cmd(const char * cmd){
  	for(int i = 0; i < strlen(cmd); i++)
	    UCA3_Send(cmd[i]);
}

void esp_comunicationOK(){
	esp_cmd("AT");					// AT—Tests AT Startup 
	const char* resp = esp_resp();
	if(!(strcmp(resp, "OK"))) printf("Comunicação OK com esp8266\n");
	else 					  printf("Sem comunicação com esp8266\n");
}

void esp_init(){
	esp_cmd("AT\r\n");					// AT—Tests AT Startup 
	esp_cmd("AT+CIPMODE=3\r\n");		// 0: normal transmission mode. 
	esp_cmd("AT+CWQAP\r\n");			// Disconnects from the AP 
	esp_cmd("AT+CWJAP=\"" SSID "\",\""WIFIPASS"\"\r\n");  // connecting wifi
	esp_cmd("AT+CIFSR\r\n");			// Gets the local IP address 

	//esp_cmd(AT+CIPSTART=\"TCP\",\"" SRV_ADDR "\"," SRV_PORT); 
	// TCP: connection type: TCP
	// SRV_ADDR (defined) remote IP address
	// SRV_PORT (defined)  remote port number
}

const char* esp_resp(){
	return UCA3_Get();
}

void esp_sendemail(){
	esp_cmd("AT+CIPMUX=1\r\n");
	esp_cmd("AT + CIPSERVER = 1,"SRV_PORT"\r\n");
	esp_cmd("AT+CIPSTART=4\"TCP\",\"" EMAILFROM "\"," EMAILFROM_PORT"\r\n"); 
	esp_cmd("AT + CIPSEND = 4,"NBYTES_MESSAGE"\r\n");   

	// Verificar o porquê dessa forma, verificar no smtp2go
	esp_cmd("EHLO"IP);
	esp_cmd("AUTH LOGIN");
    esp_cmd(EMAIL64);    				// email ase 64 format  (LOGIN em smtp2go)
    esp_cmd(" "PASSWORD64);				// password base 64 format
    esp_cmd("MAIL FROM:<"EMAILFROM">");
    esp_cmd("RCPT To:<"EMAILTO">");
    esp_cmd("DATA");
    esp_cmd(MESSAGE);
    esp_cmd("AT+CIPSEND=6\r\n");
    esp_cmd("QUIT");
}




