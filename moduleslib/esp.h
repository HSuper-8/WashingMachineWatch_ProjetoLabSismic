// Biblioteca para o módulo ESP8266


#include <stdio.h>
#include <string.h>

#define BAUDRATE  115200

#define SSID "MILTCORALLE"
#define WIFIPASS "1000tons"

#define SMTP2GO "mail.smtp2go.com"
#define SMTP2GOPORT "2525"
#define SRV_PORT  "80"
#define IP "104.196.201.1"

#define EMAILFROM "wmwproj@gmail.com"
#define EMAILTO "hevelyn.sthefany@gmail.com"

#define EMAIL64  "d213cHJvakBnbWFpbC5jb20="
#define PASSWORD64  "d213c2lzbWlj"

#define NBYTES_MESSAGE "180"
#define MESSAGE "Trabalho da Lavadora terminado. Suas roupas estão prontas!" 


void esp_config();
void clear_buffer();
int esp_cmd(char* cmd, int t, char* ret);
void esp_init();
void esp_sendemail();
