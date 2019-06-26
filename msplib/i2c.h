#include <msp430.h>

#define BR100K 10  //100 kbps
#define BR50K  20  // 50 kbps
#define BR10K  100  // 10 kbps

//#define PCF 0x3f //Endereço PCF8574AT
#define PCF 0x27 //Endereço PCF8574T

#define BIT_RS   BIT0
#define BIT_RW   BIT1
#define BIT_E    BIT2
#define BIT_BL   BIT3

#define BIT_D4   BIT4
#define BIT_D5   BIT5
#define BIT_D6   BIT6
#define BIT_D7   BIT7

void delay(long limite);

// MSP430 5529 _________________________________________
/* #define LED1 BIT0
#define LED1_OUT P1_OUT
#define LED2 BIT7
#define LED2_OUT P4OUT */

/*void config_I2C_(int isMaster, uint8_t ownAddr);
void LCD_inic_(void);
void LCD_aux_(char dado);
void PCF_STT_STP_(void);

void PCF_write_(char dado);
int PCF_read_(void);*/

// MSP430 5994 _________________________________________
#define LED1 BIT0
#define LED1_OUT P1_OUT
#define LED2 BIT7
#define LED2_OUT P4OUT

void config_I2C_B2(int isMaster, uint8_t ownAddr, int whichslave);
void config_I2C_B1(int isMaster, uint8_t ownAddr, int whichslave);
void LCD_inic(uint8_t addr);
void LCD_aux(char data);
void PCF_STT_STP(void);

void B2_start_B1(uint8_t addr);
void B2_write_on_B1(char data);
void B2_write_byte_LCD(char data);
int B2_read(void);
void PCF_write(uint8_t slaveAddr, char data);