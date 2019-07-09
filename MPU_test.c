#include <msp430.h> 
#include "MPU.h"

float           measures[6];
// Lasts samples from MPU array
uint8_t         reply[14];

// Signed int Accelerometer
// and gyroscope measures(not converted)
int             gy;

//Converted value
float           GYRO;

int             gyro_avg[100];
int i=0;
int j=0;

// Accelerometer states
// Start: Initial values from accelerometer(initial values is floating
// glitched around values)
// Sample: begins to process the gyroscope's axis y value
// and check its value(indicates an idle or a movement)
enum ACCL_STATE {start, sample} ACCL_STATE=start;

// IdleState: Detects washing finish, and send email
void IdleState();

// MovementState: stand by until IdleState(don't send email)
void MovementState();

/**
 * main.c
 */
int main(void)
{
    volatile int i = 0;
    uint8_t a_scale, g_scale;

    WDTCTL = WDTPW | WDTHOLD;                   // stop watchdog timer

    setupPorts();                               // Setup ports
    setupTimerA0();                             // Setup timer
    setupTimerA1();                             // Setup timer
    I2C_config_B2(setMaster, 0x42,              // Setup I2C interface
                  100, 1);

    // Activate Pins
    PM5CTL0 ^= LOCKLPM5;

    //Desired Scale
    a_scale = MPU6050_ACCEL_FS_2;              //2, 4, 8 or 16 g
    g_scale = MPU6050_GYRO_FS_1000;            //250, 500, 1000 or 2000 degrees/second

    B2_STT_STP(MPU6050_DEFAULT_ADDRESS);

    // Wake up MPU
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x01);
    waitFor(250);

    // Test Comunication
    mpuRead_nb(MPU6050_RA_WHO_AM_I,reply,1);
    if(reply[0]==MPU6050_DEFAULT_ADDRESS){}
    else while(1){P1OUT |= BIT0;};              // Reports error with Red LED

    waitFor(250);
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

    // Waits for 4s(awaits for a steady accelerometer state)
    mpuRead_nb(MPU6050_RA_GYRO_YOUT_H, reply, 2);
    waitFor(1000);
    P1OUT |= BIT1;
    waitFor(1000);
    P1OUT &= ~BIT1;
    waitFor(1000);
    P1OUT |= BIT1;
    waitFor(1000);
    P1OUT &= ~BIT1;

    __enable_interrupt();

    while(1){
        //Get one full reading from accelerometers, giroscope and temperature
        mpuRead_nb(MPU6050_RA_GYRO_YOUT_H, reply, 2);
        gy = (int) ((reply[0] << 8) | reply[1]) ;
        GYRO = (float) gy*(250<<g_scale)/32767;
    }


    return 0;
}

#pragma vector= TIMER1_A0_VECTOR
__interrupt void TA1_CCR0_ISR() {
    switch (ACCL_STATE) {
    case start:
        // Bypass 7 first accelerometer readings
        // (bad measure values)
        if(j>7){
            ACCL_STATE = sample;
        }
        break;

    case sample:
        // Compares if gyroscope sampling average at axis-y
        // indicates a movement pattern(values out of range
        // 190 < w < 250)
        if(i == 10)
        {
            // If its moving(washing machine's on)
            if(!(gyro_avg[j] < 255 && gyro_avg[j] > 190))
            {
                MovementState();
            }

            // It's not moving(washing machine's off)
            else
            {
                IdleState();
            }
        }
        break;
    }

    // Gyroscope sampling average computing(10 values)
    if(i < 10){
        gyro_avg[j] += GYRO/10;
        i++;
    }

    // Surpassing of 10 values, starts to compute
    // average again, and resets its value
    else {
        i = 0;
        j++;
        j %= 100;
    }
}

void IdleState(){
    P1OUT &= ~BIT0;
    // Waits 6 seconds, to confirm washing finish
    waitFor(2000);
    waitFor(2000);
    waitFor(2000);
}
void MovementState(){
    P1OUT |= BIT0;
}
