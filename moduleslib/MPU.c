#include "MPU.h"
#include "I2C.h"

int offset_int[6];                  //Offset para accel x y z e gyro x y z

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
             0;

    // Timer A0 - CCR0 configuration
    TA0CCR0  = 32767;                          // Count to 1s @32kHz
}

// Setup Timer A1 - setups timer A1
void setupTimerA1(){

    // Timer configuration
    TA1CTL = TASSEL__ACLK      |              // Select ACLK as clock source
             ID__1             |              // Set clock divider to 1.
             MC__UP            |              // Setup but do not count
             TACLR             |              // Clear timer
             0;


    TA1CCTL0 |= CCIE;
    // Timer A0 - CCR0 configuration
    TA1CCR0  = 3277;                          // Count to 1s @32kHz
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
    TA0CTL  |= MC__STOP ;

    // Locks, waiting for the timer.
    //__low_power_mode_0();
}

// MPU Set Byte - Write "data" at
// the internal register "addr" of mpu
void mpuSetByte(uint8_t addr, uint8_t data)
{
    MPU_I2C_B2_write_byte(addr, data);
}


// MPU get byte - writes register address
// from MPU's and read data from it
uint8_t mpuGetByte(uint8_t addr){
    I2C_B2_write_byte(MPU6050_DEFAULT_ADDRESS, addr);
    delay(50);
    return I2C_B2_read_byte(MPU6050_DEFAULT_ADDRESS);
}

void MPU_I2C_B2_write_byte(uint8_t addr, char data){
    UCB2I2CSA = MPU6050_DEFAULT_ADDRESS;
    UCB2CTLW0 |= UCTR;
    UCB2CTLW0 |= UCTXSTT;

    // RA address
    while(!(UCB2IFG & UCTXIFG0));
    UCB2TXBUF = addr;

    while(!(UCB2IFG & UCTXIFG0) && !(UCB2IFG & UCNACKIFG));

    if(UCB2IFG & UCNACKIFG) {
        UCB2CTLW0 |= UCTXSTP;
        while(UCB2CTLW0 & UCTXSTP);
            return ERROR;
    }

    // RA data

    while(!(UCB2IFG & UCTXIFG0));

    UCB2TXBUF = data;

    delay(50);

    while(!(UCB2IFG & UCTXIFG0) && !(UCB2IFG & UCNACKIFG));

    if(UCB2IFG & UCNACKIFG) {
        UCB2CTLW0 |= UCTXSTP;
        while(UCB2CTLW0 & UCTXSTP);
            return ERROR;
    }

    UCB2IFG &= ~UCTXIFG0;

    UCB2CTLW0 |= UCTXSTP;
    while(UCB2CTLW0 & UCTXSTP);   // Wait Stop

    delay(50);
}

uint8_t MPU_I2C_B2_read_byte(uint8_t addr){
    int8_t data;

    UCB2I2CSA = MPU6050_DEFAULT_ADDRESS;
    UCB2CTLW0 |= UCTR;
    UCB2CTLW0 |= UCTXSTT;

    while(!(UCB2IFG & UCTXIFG0));
    UCB2TXBUF = addr;

    while(!(UCB2IFG & UCTXIFG0) && !(UCB2IFG & UCNACKIFG));

    if(UCB2IFG & UCNACKIFG) {
        UCB2CTLW0 |= UCTXSTP;
        while(UCB2CTLW0 & UCTXSTP);
            P1OUT |= BIT0;
            while(1);
    }

    UCB2I2CSA = MPU6050_DEFAULT_ADDRESS;
    UCB2CTLW0 &= ~UCTR;
    UCB2CTLW0 |= UCTXSTT;

    while (UCB2CTLW0 & UCTXSTT);

    UCB2CTLW0 |= UCTXSTP;

    while (!(UCB2IFG & UCRXIFG));

    data = UCB2RXBUF;

    return data;
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

uint8_t mpuSelfTest(void){
    unsigned int controle [10];
    unsigned char aux[14];                    //Para leitura dos registradores
    int gx1, gy1, gz1, ax1, ay1, az1;           //Valores coletados com self test desabilitado
    int gx2, gy2, gz2, ax2, ay2, az2;           //Valores coletados com self test desabilitado
    unsigned char st3[6];                       //Valores coletados do registrador de self-test
    float stf[6];                               //Valores de Factory Trim
    float str[6];                               //Alteração em %

    //Passo 1: Desabilitar o self test e realizar as leiuras dos accel, gyro e temp
    //As escalas devem estar em 8g e 250
    mpuSetByte(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACCEL_FS_8 << 3);
    mpuSetByte(MPU6050_RA_GYRO_CONFIG, MPU6050_GYRO_FS_250 << 3);
    waitFor(250);

    mpuRead_nb(MPU6050_RA_ACCEL_XOUT_H, aux, 14);
    ax1 = (int) ((aux[0] << 8) | aux[1]) ;
    ay1 = (int) ((aux[2] << 8) | aux[3]) ;
    az1 = (int) ((aux[4] << 8) | aux[5]) ;
    gx1 = (int) ((aux[8] << 8) | aux[9]) ;
    gy1 = (int) ((aux[10] << 8) | aux[11]) ;
    gz1 = (int) ((aux[12] << 8) | aux[13]) ;

    //Passo 2: Habilitar o self test e realizar as leiuras dos accel gyro
    //As escalas devem estar em 8g e 250
    mpuSetByte(MPU6050_RA_ACCEL_CONFIG, (0xE0 | MPU6050_ACCEL_FS_8 << 3));
    mpuSetByte(MPU6050_RA_GYRO_CONFIG, (0xE0 | MPU6050_GYRO_FS_250 << 3));
    waitFor(250);

    controle[1] = 4;
    mpuRead_nb(MPU6050_RA_ACCEL_XOUT_H, aux, 14);
    ax2 = (int) ((aux[0] << 8) | aux[1]) ;
    ay2 = (int) ((aux[2] << 8) | aux[3]) ;
    az2 = (int) ((aux[4] << 8) | aux[5]) ;
    gx2 = (int) ((aux[8] << 8) | aux[9]) ;
    gy2 = (int) ((aux[10] << 8) | aux[11]) ;
    gz2 = (int) ((aux[12] << 8) | aux[13]) ;

    //Passo 3: Extrair os dados dos registradores de self-test
    mpuRead_nb(MPU6050_RA_SELF_TEST_X, aux, 4);
    st3[0] = (aux[0] >> 3) | (aux[3] & 0x30) >> 4 ;
    st3[1] = (aux[1] >> 3) | (aux[3] & 0x0C) >> 2 ;
    st3[2] = (aux[2] >> 3) | (aux[3] & 0x03) >> 0 ;
    st3[3] = aux[0] & 0x1F ;
    st3[4] = aux[1] & 0x1F ;
    st3[5] = aux[2] & 0x1F ;

    //Passo 4: Calcular o Factory Trim
    stf[0] = (4096.0*0.34) * (pow((0.92/0.34) , (((float)st3[0] - 1.0) / 30.0)));
    stf[1] = (4096.0*0.34) * (pow((0.92/0.34) , (((float)st3[1] - 1.0) / 30.0)));
    stf[2] = (4096.0*0.34) * (pow((0.92/0.34) , (((float)st3[2] - 1.0) / 30.0)));
    stf[3] = ( 25.0 * 131.0) * (pow( 1.046 , ((float)st3[3] - 1.0) ));
    stf[4] = (-25.0 * 131.0) * (pow( 1.046 , ((float)st3[4] - 1.0) ));
    stf[5] = ( 25.0 * 131.0) * (pow( 1.046 , ((float)st3[5] - 1.0) ));

    //Caso especial: se o registrador de self test for 0 o factory trim será 0
    if (st3[0] == 0) stf[0] = 0;
    if (st3[1] == 0) stf[1] = 0;
    if (st3[2] == 0) stf[2] = 0;
    if (st3[3] == 0) stf[3] = 0;
    if (st3[4] == 0) stf[4] = 0;
    if (st3[5] == 0) stf[5] = 0;

    //Passo 5: Cálculo das Percentagens de Alteração
    str[0] = 100.0 * ((float) (ax2 - ax1) - stf[0]) / stf[0];
    str[1] = 100.0 * ((float) (ay2 - ay1) - stf[1]) / stf[1];
    str[2] = 100.0 * ((float) (az2 - az1) - stf[2]) / stf[2];
    str[3] = 100.0 * ((float) (gx2 - gx1) - stf[3]) / stf[3];
    str[4] = 100.0 * ((float) (gy2 - gy1) - stf[4]) / stf[4];
    str[5] = 100.0 * ((float) (gz2 - gz1) - stf[5]) / stf[5];

    //Passo 6: Checar se as percentagens de alteração estão abaixo de 14%
    if (str[0]<14 && str[1]<14 && str[2]<14 && str[3]<14 && str[4]<14 && str[5]<14){
        //if (axr<14 && gxr<14 && gyr<14 && gzr<14)
        return 1;  //Self-test passou
    }

    else {
        return 0;   //Self-test falhou
    }
}

void mpuOffset (unsigned char gyro, unsigned char accl){

    unsigned int j;
    unsigned char aux[14];
    long abx,aby,abz,gbx,gby,gbz;

    abx=aby=abz=gbx=gby=gbz=0;

    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x80);    //Reset do mpu
    waitFor(250);
    mpuSetByte(MPU6050_RA_PWR_MGMT_1, 0x01);    //Retira do modo sleep e seleciona o clock do PLL do gyro eixo x
    waitFor(250);

    mpuSetByte(MPU6050_RA_GYRO_CONFIG , gyro<<3);
    mpuSetByte(MPU6050_RA_ACCEL_CONFIG, accl<<3);
    mpuSetByte(MPU6050_RA_CONFIG, 0x01);        //Seleciona a maior banda possivel para os filtros passa baixo e o giroscópio operara em 1khz
    mpuSetByte(MPU6050_RA_SMPLRT_DIV, 0x00);    //Taxa de amostragem em 1kHz

    //Para otimizar o tempo das amostras será utilizado o data_rdy_int
    //Este bit indica quando os dados de uma leitura foram coletados
    //Será habilitada interrupção para que seja possível coletar os dados antes da próxima leitura

    mpuSetByte(MPU6050_RA_INT_ENABLE, 0x01);    //Habilita interrupção para pooling
    mpuGetByte(MPU6050_RA_INT_STATUS);          //Ler apaga o bit DATA_RDY_INT

    //Serão realizadas 256 leituras
    for (j=0;j<256;j++){
        while((mpuGetByte(MPU6050_RA_INT_STATUS)&1)==0);    //Aguarda a coleta dos dados
        mpuRead_nb(MPU6050_RA_ACCEL_XOUT_H, aux, 14);
        abx += (long) (((unsigned int)aux[0] << 8) | aux[1]) ;
        aby += (long) (((unsigned int)aux[2] << 8) | aux[3]) ;
        abz += (long) (((unsigned int)aux[4] << 8) | aux[5]) ;
        gbx += (long) (((unsigned int)aux[8] << 8) | aux[9]) ;
        gby += (long) (((unsigned int)aux[10] << 8) | aux[11]) ;
        gbz += (long) (((unsigned int)aux[12] << 8) | aux[13]) ;
    }

    //Em seguida calculamos a média das 256 medidas
    offset_int[0] = (int) (abx>>8);
    offset_int[1] = (int) (aby>>8);
    offset_int[2] = (int) (abz>>8);
    offset_int[3] = (int) (gbx>>8);
    offset_int[4] = (int) (gby>>8);
    offset_int[5] = (int) (gbz>>8);
}
