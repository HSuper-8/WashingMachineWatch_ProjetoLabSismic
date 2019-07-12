#include "esp.h"
#include "UART.h"

volatile char RET[200]={0};
volatile unsigned char RX = 0;
volatile unsigned int i=0;

int COMMUNICATION=0;

// BRCLK        Baud Rate   UCOS16      UCBRx   UCBRFx      UCBRSx
// 1000000      115200      0           8       -           0xD6

void delay(volatile int t){
    while(t--);
}

void esp_config(){
    UCA3_Config(115200, 0, 0, 1);
}

void clear_buffer(){
    int j = 0;
    for(j=199; j>=0; j--)
        RET[j] = 0;
    i = 0;
    RX = 0;
}

int esp_cmd(char* cmd, uint32_t t, char* ret){

    UCA3_SendStr(cmd);                  // envia comando
    delay(t);                           // espera um pouco
    if(strstr(RET, ret) != NULL){       // se a resposta de retorno vier (ver documentação)
        COMMUNICATION=1;                // seta flag (para debug)
        clear_buffer();                 // limpa vetor que guarda resposta
        return 1;
    }
    clear_buffer();
    return 0;
}

void esp_init(){
    esp_cmd("AT\r\n", 1000, "OK");              // AT—Tests AT Startup
    esp_cmd("AT+CIPCLOSE\r\n",1000,"OK");       //Closes the TCP/UDP/SSL Connection
    esp_cmd("AT+RST\r\n",2000,"ready");         //Restarts the Module
    esp_cmd("AT+CWMODE=3\r\n",1000,"OK");       //
    esp_cmd("AT+CWJAP=\"" SSID "\",\""WIFIPASS"\"\r\n",7000,"OK");  // connecting wifi (mudar defines!)
    esp_cmd("AT+CIFSR\r\n",1000,"");            // Gets the local IP address

    //esp_cmd(AT+CIPSTART=\"TCP\",\"" SRV_ADDR "\"," SRV_PORT);
    // TCP: connection type: TCP
    // SRV_ADDR (defined) remote IP address
    // SRV_PORT (defined)  remote port number
}

void esp_sendemail(){
    esp_cmd("AT+CIPMUX=1\r\n", 1000, "OK");
    esp_cmd("AT + CIPSERVER = 1,"SRV_PORT"\r\n", 1000, "OK");
    esp_cmd("AT+CIPSTART=4,\"TCP\",\"" SMTP2GO "\"," SMTP2GOPORT"\r\n", 1000, "OK");
    esp_cmd("AT + CIPSEND = 4,"NBYTES_MESSAGE"\r\n", 1000, "");

    // SMTP Commands Reference
    UCA3_SendStr("EHLO"IP);
    UCA3_SendStr("AUTH LOGIN");
    UCA3_SendStr(EMAIL64);                  // email ase 64 format  (LOGIN em smtp2go)
    UCA3_SendStr(" "PASSWORD64);                // password base 64 format
    UCA3_SendStr("MAIL FROM:<"EMAILFROM">");
    UCA3_SendStr("RCPT To:<"EMAILTO">");
    UCA3_SendStr("DATA");
    UCA3_SendStr(MESSAGE);
    UCA3_SendStr("AT+CIPSEND=6\r\n");
    UCA3_SendStr("QUIT");
}

#pragma vector=EUSCI_A3_VECTOR
__interrupt void USCI3RX_ISR(void){
    RET[i++] = UCA3RXBUF;
    //while(UCA3STATW & UCBUSY);
    RX=1;
}







