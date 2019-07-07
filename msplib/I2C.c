#include "I2C.h"

void delay(long tempo){
    volatile long cont=0;
    while (cont++ < tempo);
}

void I2C_config_B1(int isMaster, uint8_t ownAddr,
            int baudRate_khz, int slave_number)
{

    P5SEL0 |=  BIT0;
    P5SEL1 &= ~BIT0;

    P5SEL0 |=  BIT1;
    P5SEL1 &= ~BIT1;

    UCB1CTLW0 = UCSWRST;

    UCB1CTLW0 = UCSYNC      |
                UCMODE__I2C |
                UCSSEL__SMCLK;

    if(isMaster)
    {
        UCB1CTLW0 |= UCMST;
        UCB1I2COA0 = ownAddr;
    }

    else
    {
        if(slave_number == 1)
        {
            UCB1I2COA1 = UCOAEN | ownAddr;
        }
        else if(slave_number == 2)
        {
            UCB1I2COA2 = UCOAEN | ownAddr;
        }

        else if(slave_number == 3)
        {
            UCB1I2COA3 = UCOAEN | ownAddr;
        }
    }

    UCB1BRW = 1000/baudRate_khz;

    UCB1CTLW0 &= ~UCSWRST;
}

void I2C_config_B2(int isMaster, uint8_t ownAddr,
            int baudRate_khz, int slave_number)
{

    P7SEL0 |=  BIT0;
    P7SEL1 &= ~BIT0;

    P7SEL0 |=  BIT1;
    P7SEL1 &= ~BIT1;

    P7REN &= ~BIT0;
    P7REN &= ~BIT1;
    P7OUT |=  BIT0 | BIT1;


    UCB2CTLW0 = UCSWRST;

    UCB2CTLW0 = UCSYNC      |
                UCMODE__I2C |
                UCSSEL__SMCLK;

    if(isMaster)
    {
        UCB2CTLW0 |= UCMST;
        UCB2I2COA0 = ownAddr;
    }

    else
    {
        if(slave_number == 1)
        {
            UCB2I2COA1 = UCOAEN | ownAddr;
        }
        else if(slave_number == 2)
        {
            UCB2I2COA2 = UCOAEN | ownAddr;
        }

        else if(slave_number == 3)
        {
            UCB2I2COA3 = UCOAEN | ownAddr;
        }
    }

    UCB2BRW = 1000/baudRate_khz;

    UCB2CTLW0 &= ~UCSWRST;
}

void I2C_B1_write_byte(uint8_t slaveAddr, char data){
    UCB1I2CSA = slaveAddr;
    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;

    while(!(UCB1IFG & UCTXIFG0));
    UCB2TXBUF = data;

    while(!(UCB1IFG & UCTXIFG0) && !(UCB1IFG & UCNACKIFG));

    if(UCB1IFG & UCNACKIFG) {
        UCB1CTLW0 |= UCTXSTP;
        while(UCB1CTLW0 & UCTXSTP);
            return ERROR;
    }

    while(!(UCB1IFG & UCTXIFG0));
    UCB1IFG &= ~UCTXIFG0;

    UCB1CTLW0 |= UCTXSTP;
    while(UCB1CTLW0 & UCTXSTP);   // Wait Stop
}

void I2C_B2_write_byte(uint8_t slaveAddr, char data){
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
    while(UCB2CTLW0 & UCTXSTP);   // Wait Stop
}

uint8_t I2C_B2_read_byte(uint8_t slaveAddr){
    char data;

    UCB2I2CSA = slaveAddr;
    UCB2CTLW0 &= ~UCTR;
    UCB2CTLW0 |= UCTXSTT;

    while (UCB2CTLW0 & UCTXSTT);

    UCB2CTLW0 |= UCTXSTP;

    while (!(UCB2IFG & UCRXIFG));

    data = UCB2RXBUF;

    delay(50);

    return data;
}

void B2_STT_STP(uint8_t addr){
    int x=0;

    UCB2I2CSA = addr;

    while (x<5){
        UCB2CTLW0 |=    UCTR    |
                        UCTXSTT;
        while (!(UCB2IFG & UCTXIFG0));
        UCB2CTLW0 |= UCTXSTP;
        delay(200);
        if (!(UCB2CTLW0 & UCTXSTP))   break;
        x++;
    }
    while ( (UCB2CTLW0 & UCTXSTP));
}


