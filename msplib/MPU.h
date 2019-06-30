#ifndef MPU_H
#define MPU_H

#include <msp430.h>
#include "I2C.h"
#include <stdint.h>

#define MPU6050_DEFAULT_ADDRESS         0x68

// Mpu Registers
#define MPU6050_RA_PWR_MGMT_1           0x6B
#define MPU6050_RA_SMPLRT_DIV           0x19
#define MPU6050_RA_CONFIG               0x1A
#define MPU6050_RA_GYRO_CONFIG          0x1B
#define MPU6050_RA_ACCEL_CONFIG         0x1C
#define MPU6050_RA_ACCEL_XOUT_H         0x3B
#define MPU6050_RA_WHO_AM_I             0x75

// Scales
// Output(degrees/second) = Output(signed integer) * max_range/32767
#define MPU6050_GYRO_FS_250             0x00
#define MPU6050_GYRO_FS_500             0x01
#define MPU6050_GYRO_FS_1000            0x02
#define MPU6050_GYRO_FS_2000            0x03

// Output(g) = Output(signed integer) * max_range/32767
#define MPU6050_ACCEL_FS_2              0x00
#define MPU6050_ACCEL_FS_4              0x01
#define MPU6050_ACCEL_FS_8              0x02
#define MPU6050_ACCEL_FS_16             0x03


/////////////////////////////////////////////////////////////////////////////////////////

#define SLAVE           00
#define MASTER          01

#define TRANSMITTER     00
#define RECEIVER        01

float           ACCEL[3],GYRO[3];      //converted value

//Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////
void    setupPorts          (void);
void    setupTimerA0        (void);
void    waitFor             (unsigned int time_ms);
void    mpuSetByte          (uint8_t addr, uint8_t data);
uint8_t mpuGetByte    (uint8_t addr);
void    mpuRead_nb          (uint8_t addr, uint8_t * data, uint8_t length);

#endif
