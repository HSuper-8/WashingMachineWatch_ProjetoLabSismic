#include <msp430.h> 
#include "MPU.h"


/**
 * main.c
 */
int main(void)
{
    uint8_t   reply[14];                  //Vetor com as últimas medidas da função mpuRead
    int             ax,ay,az,gx,gy,gz,temp;     //signed int value
    unsigned char a_scale, g_scale;
    __enable_interrupt();

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    setupPorts();                   // Setup ports
    setupTimerA0();                 // Setup timer
    I2C_config_B2(setMaster, 0x42,  // Setup I2C interface
                  10, 1);

   // High-impedance treatment
    PM5CTL0 ^= LOCKLPM5;

    //Desired Scale
    a_scale = MPU6050_ACCEL_FS_2;      //2, 4, 8 or 16 g
    g_scale = MPU6050_GYRO_FS_250;      //250, 500, 1000 or 2000 degrees/second

    //Wake up MPU
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x01);
    waitFor(250);

    //Test Comunication
    mpuRead_nb(MPU6050_RA_WHO_AM_I,reply,1);
    if(reply[0]==MPU6050_DEFAULT_ADDRESS){}
    else while(1){P1OUT |= BIT1;};          //If test fails, green led on

	return 0;
}


