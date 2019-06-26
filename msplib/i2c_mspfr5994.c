// Configuração placa 5529

#include <msp430.h>
#include "i2c.h"

// Configura Pinos I2C usando UCB2  chamar: config_I2C_B2(1, 0x34, 10, 1);
void config_I2C_B2(int isMaster, uint8_t ownAddr, int whichslave){

    P7SEL0 |= BIT1 | BIT0;
    P7SEL1 &= ~(BIT1 | BIT0);
    P7REN |= BIT1 | BIT0;    // Resistor enable
    P7OUT |= BIT1 | BIT0;    // Pull-up

    UCB2CTLW0 |= UCSWRST;    // B2 em ressete

    UCB2CTLW0 = UCSWRST;

    UCB2CTLW0 = UCSYNC |    //Síncrono
               UCMODE_3 |   //Modo I2C
               UCSSEL_2;    // SMCLK
    
    if(isMaster){                    
        UCB2CTLW0 |= UCMST; // Mestre
    	UCB2I2COA0 = ownAddr; // own address - mestre
    }
    
    else{                                               
    	if(whichslave == 1){                             
    		UCB2I2COA1 = UCOAEN | ownAddr;               
    	}
        else if(whichslave == 2){                       
    		UCB2I2COA2 = UCOAEN | ownAddr;               
    	}
        else if(whichslave == 3){                       
    		UCB2I2COA3 = UCOAEN | ownAddr;               
    	}
    }

    UCB2BRW = BR10K;      // 10 kbps
    UCB2CTLW0 &= ~UCSWRST;
}

// Configura UCB1
void config_I2C_B1(int isMaster, uint8_t ownAddr, int whichslave){

    P5SEL0 |= BIT1 | BIT0;
    P5SEL1 &= ~(BIT1 | BIT0);
    P5REN |= BIT1 | BIT0;    // Resistor enable
    P5OUT |= BIT1 | BIT0;    // Pull-up

    UCB1CTLW0 = UCSWRST;   
    UCB1CTLW0 = UCSYNC |    //Síncrono
               UCMODE_3 |   //Modo I2C
               UCSSEL_2;    // SMCLK
    
    if(isMaster){ 
        UCB1CTLW0 |= UCMST; // Mestre
    	UCB1I2COA0 = ownAddr; // Select master's own address
    }
    else{                                               
        if(whichslave == 1){                            
            UCB1I2COA1 = UCOAEN | ownAddr;               
        }
        else if(whichslave == 2){                       
            UCB1I2COA2 = UCOAEN | ownAddr;               
        }
        else if(whichslave == 3){                       
            UCB1I2COA3 = UCOAEN | ownAddr;               
        }
    }
    UCB1BRW = BR10K;   // baud-rate
    UCB1CTLW0 &= ~UCSWRST;
}

// Transmissão entre B1(escravo) e B2(mestre)
void B2_start_B1(uint8_t addr){
	UCB2I2CSA = addr;                              // Escravo addr
    UCB2CTLW0 |=  UCTR;                           // transmissor
    UCB2CTLW0 |= UCTXSTT;                         // start
}

// Escrita de B2(mestre) em B1(escravo)
void B2_write_on_B1(char data){
    while(!(UCB2IFG & UCTXIFG0));                 // espera TXIFG0 (mestre em I2COA0)
    if(UCB2IFG & UCTXIFG0 == UCNACKIFG)           // NACK?
        while(1);                                 // Se NACK - faz nada
    UCB2TXBUF = data;                             // Data -> buffer
}

void B2_stop_B1(){
    while(!(UCB2IFG & UCTXIFG0));                 // Espera TXIFG0 
    UCB2CTLW0 |= UCTXSTP;                         // Stop
    while(UCB2CTLW0 & UCTXSTP == UCTXSTP);        // Espera Stop
}

// Leitura de UCB2(Mestre)
int B2_read(void){
    int data;
    UCB2CTLW0 &= ~UCTR;                           // Receptor
    UCB2CTLW0 |= UCTXSTT;                         // Start
    while(UCB2CTLW0 & UCTXSTT == UCTXSTT);        // Espera Start
    UCB2CTLW0 |= UCTXSTP;                         // Stop
    while (!(UCB2IFG & UCRXIFG0));                // Espera RXIFG0 
    data = UCB2RXBUF;                             // Captura dado
    return data;                                  // Retorna dado
}

// Coloca escravo em estado conhecido
void PCF_STT_STP(){
    int x=0;
    UCB2I2CSA = PCF_ADR;   		                   //Endereço Escravo

    while (x<5){
        UCB2CTLW0 |= UCTR    |                      //Mestre TX
                    UCTXSTT;                        //Gerar START
        while ( (UCB2IFG & UCTXIFG) == 0);	        //Esperar TXIFG=1
        UCB2CTLW0 |= UCTXSTP;    			        //Gerar STOP
        delay(200);
        if ( (UCB2CTLW0 & UCTXSTP) == 0)   break;   //Esperar STOP
        x++;
    }
    while ( (UCB2CTLW0 & UCTXSTP) == UCTXSTP);      //I2C Travado (Desligar / Ligar)
}

// Inicializa LCD em modo 4 bits
void LCD_inic(uint8_t addr){

    PCF_STT_STP()                             // LCD em estado conhecido

    // Preparar I2C para operar
    UCB0I2CSA = PCF;                         //Endereço Escravo
    UCB2CTL0 |= UCTR    |                    //Mestre TX
                UCTXSTT;                     //Gerar START
    while(!(UCB2IFG & UCTXIFG0));            //Esperar TXIFG=1
    UCB2TXBUF = 0;                           // Slave's exit = 0;
    while(UCB2CTLW0 & UCTXSTT == UCTXSTT);   // Espera Start
    if(UCB2IFG & UCTXIFG0 == UCNACKIFG);     // NACK?
        while(1);                            // If NACK - faz nada

    // Inicialização
    LCD_aux(0);                               // RS = RW = 0, BL=1
    delay(20000);
    LCD_aux(3);                                       
    delay(10000);
    LCD_aux(3);                                       
    delay(10000);
    LCD_aux(3);                                       
    delay(10000);
    LCD_aux(2);                                       

    // Modo 4 bits
    LCD_aux(2);     LCD_aux(8);                    //0x28
    LCD_aux(0);     LCD_aux(8);                    //0x08
    LCD_aux(0);     LCD_aux(1);                    //0x01
    LCD_aux(0);     LCD_aux(6);                    //0x06
    LCD_aux(0);     LCD_aux(0xF);                  //0x0F

    while(!(UCB2IFG & UCTXIFG0));                  //Esperar TXIFG=1 (com mestre em I2COA0)
    UCB2CTLW0 |= UCTXSTP;                          //Gerar STOP
    while(UCB2CTLW0 & UCTXSTP == UCTXSTP);         // Espera Stop
    delay(500);
}

// Auxiliar inicialização do LCD (RS = RW = 0)
// Somente para inicialização 
void LCD_aux(char data){
    while(!(UCB2IFG & UCTXIFG0));                       //Esperar TXIFG=1
    UCB2TXBUF = ((data<<4)&0XF0) | BIT_BL;              //PCF7:4 = dado;
    delay(50);
    while(!(UCB2IFG & UCTXIFG0));                       //Esperar TXIFG=1
    UCB2TXBUF = ((data<<4)&0XF0) | BIT_BL | BIT_E;      //E=1
    delay(50);
    while(!(UCB2IFG & UCTXIFG0));                       //Esperar TXIFG=1
    UCB2TXBUF = ((data<<4)&0XF0) | BIT_BL;              //E=0;
}

void B2_write_byte_LCD(char data){
	B2_start_B1(0x27);
    B2_write_on_B1(data);
    B2_stop_B1();
}

void PCF_write(uint8_t slaveAddr, char data){
    UCB2I2CSA = slaveAddr;
    UCB2CTLW0 |= UCTR;
    UCB2CTLW0 |= UCTXSTT;

    while(!(UCB2IFG & UCTXIFG0));
    UCB2TXBUF = data;

    while(!(UCB2IFG & UCTXIFG0) && !(UCB2IFG & UCNACKIFG));

    if(UCB2IFG & UCNACKIFG) {
        UCB2CTLW0 |= UCTXSTP;
        while(UCB2CTLW0 & UCTXSTP);
            return ERROR;
    }

    while(!(UCB2IFG & UCTXIFG0));
    UCB2IFG &= ~UCTXIFG0;

    UCB2CTLW0 |= UCTXSTP;
    while(UCB2CTLW0 & UCTXSTP);   // Espera Stop