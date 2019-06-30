#include "MPU.h"
#include "I2C.h"

// Setup Port - setups I/O ports
void setupPorts(){
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    P1DIR |= BIT1;
    P1OUT &= ~BIT1;
}

// Setup Timer A0 - setups timer A0
void setupTimerA0(){

    // Timer configuration
    TA0CTL = TASSEL__ACLK      |              // Select ACLK as clock source
             ID__1             |              // Set clock divider to 1.
             MC__STOP          |              // Setup but do not count
             TACLR             |              // Clear timer
             TAIE              |              // Enable Interrupts from timer
             0;

    // Timer A0 - CCR0 configuration
    TA0CCR0  = 32767;                          // Count to 1s @32kHz
}

// Wait for - Counts to time_ms milliseconds until
// time interrupt triggered
void waitFor(unsigned int time_ms){
    // Configure timer A0 and start it.
    TA0CCR0 = time_ms << 5;                   // 1 sec = 1024 ms (rounded up)
    TA0CTL  |= MC__UP | TACLR;                // Count up and clear timer

    while(!(TA0CCTL0 & CCIFG));

    TA0CCTL0 &= ~CCIFG;
    TA0CTL  |= TACLR;
    TA0CTL  = MC__STOP ;

    // Locks, waiting for the timer.
    //__low_power_mode_0();
}

// MPU Set Byte - Write "data" at
// the internal register "addr" of mpu
void mpuSetByte(uint8_t addr, uint8_t data)
{
    I2C_B2_write_byte(MPU6050_DEFAULT_ADDRESS, addr);
    I2C_B2_write_byte(MPU6050_DEFAULT_ADDRESS, data);
}

// MPU get byte - writes register address
// from MPU's and read data from it
uint8_t mpuGetByte(uint8_t addr){
    I2C_B2_write_byte(MPU6050_DEFAULT_ADDRESS, addr);
    return I2C_B2_read_byte(MPU6050_DEFAULT_ADDRESS);
}

// MPU read nibble - read length times starting at "addr"
// from MPU and write it on "data"
void mpuRead_nb(uint8_t addr, uint8_t* data, uint8_t length)
{
    uint8_t i;
    for (i=length;i>0;i--){
        data[length-i]= mpuGetByte(addr+(length-i));
    }
}
