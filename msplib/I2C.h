#ifndef I2C_H
#define I2C_H

#include <msp430.h>
#include <stdint.h>
#include <string.h>

#define MasterAddr                       (0x0042)
#define setMaster                        (   1   )
#define UCMODE__I2C                      (UCMODE_3)    // Communication I2C type
#define MASTER_ADDR                      (0x0034)
#define ERROR                            (0x0034)

void delay(long tempo);

// P5.0(SDA) and P5.1(SCL) I2C setting
void I2C_config_B1(int isMaster, uint8_t ownAddr,
            int baudRate_khz, int slave_number);

// P7.0(SDA) and P7.1(SCL) I2C setting
void I2C_config_B2(int isMaster, uint8_t ownAddr,
            int baudRate_khz, int slave_number);

void I2C_B1_write_byte(uint8_t slaveAddr, char data);

void I2C_B2_write_byte(uint8_t slaveAddr, char data);

uint8_t  I2C_B2_read_byte(uint8_t slaveAddr);

void B2_STT_STP(uint8_t addr);

#endif
