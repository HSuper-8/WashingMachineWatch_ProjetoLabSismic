#include <msp430.h> 
#include "MPU.h"

float           measures[6];
uint8_t         reply[14];                      //Lasts samples from MPU array
int             ax,ay,az,gx,gy,gz,temp;         //Signed int Accelerometer and gyroscope measures(not converted)
float           ACCEL[3],GYRO[3],TEMP;          //Converted value

/**
 * main.c
 */
int main(void)
{
    volatile int i = 0;
    uint8_t a_scale, g_scale;

	WDTCTL = WDTPW | WDTHOLD;	                // stop watchdog timer
	
    setupPorts();                               // Setup ports
    setupTimerA0();                             // Setup timer
    I2C_config_B2(setMaster, 0x42,              // Setup I2C interface
                  100, 1);

    // Activate Pins
    PM5CTL0 ^= LOCKLPM5;

    //Desired Scale
    a_scale = MPU6050_ACCEL_FS_2;              //2, 4, 8 or 16 g
    g_scale = MPU6050_GYRO_FS_250;              //250, 500, 1000 or 2000 degrees/second

    // Wake up MPU
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x01);
    waitFor(250);

    // Test Comunication
    mpuRead_nb(MPU6050_RA_WHO_AM_I,reply,1);
    if(reply[0]==MPU6050_DEFAULT_ADDRESS){}
    else while(1){P1OUT |= BIT0;};              // Reports error with Red LED

    // MPU config and reset
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x80);    //MPU Reset
    waitFor(250);
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x01);    //Wake up and selects clock from PLL in gyroscope, axis x
    waitFor(250);
    mpuSetByte(MPU6050_RA_CONFIG, 0x03);        //Rate 1khz Accel=44hz Gyro=42hz
    mpuSetByte(MPU6050_RA_SMPLRT_DIV, 0x04);    //Sampling rate: 200Hz
                                                //sampling rate = rate/ (1+SMPLRT_DIV)

    //Set scales
    mpuSetByte(MPU6050_RA_GYRO_CONFIG , g_scale<<3);
    mpuSetByte(MPU6050_RA_ACCEL_CONFIG , a_scale<<3);

    //Get one full reading from accelerometers, giroscope and temperature
    mpuRead_nb(MPU6050_RA_ACCEL_XOUT_H, reply, 14);
    ax = (int) ((reply[0] << 8) | reply[1]) ;
    ay = (int) ((reply[2] << 8) | reply[3]) ;
    az = (int) ((reply[4] << 8) | reply[5]) ;
    temp = (int) ((reply[6] << 8) | reply[7]) ;
    gx = (int) ((reply[8] << 8) | reply[9]) ;
    gy = (int) ((reply[10] << 8) | reply[11]) ;
    gz = (int) ((reply[12] << 8) | reply[13]) ;

    //Data conversion to it's apropriate unit
    ACCEL[0] = (float) ax*(2<<a_scale)/32767;
    ACCEL[1] = (float) ay*(2<<a_scale)/32767;
    ACCEL[2] = (float) az*(2<<a_scale)/32767;
    TEMP = (float) temp/340 + 36.53;
    GYRO[0] = (float) gx*(250<<g_scale)/32767;
    GYRO[1] = (float) gy*(250<<g_scale)/32767;
    GYRO[2] = (float) gz*(250<<g_scale)/32767;

    return 0;
}


