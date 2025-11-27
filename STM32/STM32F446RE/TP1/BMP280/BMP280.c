#include "BMP280.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
extern I2C_HandleTypeDef hi2c1;

uint8_t data_buffer[64];
// === DÉFINITIONS DES VARIABLES GLOBALES ===
BMP280_S32_t t_fine;

// Paramètres température
BMP280_U16_t dig_T1;
BMP280_S16_t dig_T2;
BMP280_S16_t dig_T3;

// Paramètres pression
BMP280_U16_t dig_P1;
BMP280_S16_t dig_P2;
BMP280_S16_t dig_P3;
BMP280_S16_t dig_P4;
BMP280_S16_t dig_P5;
BMP280_S16_t dig_P6;
BMP280_S16_t dig_P7;
BMP280_S16_t dig_P8;
BMP280_S16_t dig_P9;

void BMP280_Init(void)
{
    uint8_t calib[24]; // 0x88 à 0x9F
    
    HAL_I2C_Mem_Read(&hi2c1, BMP280_I2C_ADDRESS<<1, 0x88, 1, calib, 24, 100);
    
    dig_T1 = (calib[1] << 8) | calib[0];   // 0x88/0x89
    dig_T2 = (calib[3] << 8) | calib[2];   // 0x8A/0x8B
    dig_T3 = (calib[5] << 8) | calib[4];   // 0x8C/0x8D
    
    dig_P1 = (calib[7]  << 8) | calib[6];  // 0x8E/0x8F
    dig_P2 = (calib[9]  << 8) | calib[8];  // 0x90/0x91
    dig_P3 = (calib[11] << 8) | calib[10]; // 0x92/0x93
    dig_P4 = (calib[13] << 8) | calib[12]; // 0x94/0x95
    dig_P5 = (calib[15] << 8) | calib[14]; // 0x96/0x97
    dig_P6 = (calib[17] << 8) | calib[16]; // 0x98/0x99
    dig_P7 = (calib[19] << 8) | calib[18]; // 0x9A/0x9B
    dig_P8 = (calib[21] << 8) | calib[20]; // 0x9C/0x9D
    dig_P9 = (calib[23] << 8) | calib[22]; // 0x9E/0x9F

}

void BMP280_read_register(uint8_t reg){
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){reg}, 1, HAL_MAX_DELAY);
    printf("Demande de lecture de l'ID du capteur BMP280\r\n");
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, 6, HAL_MAX_DELAY);
    printf("ID du capteur BMP280: 0x%02X\r\n", data_buffer[0]);
} 

void BMP280_write_register(uint8_t reg, uint8_t value){
    uint8_t tx_data[2] = {reg, value};
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, tx_data, 2, HAL_MAX_DELAY);
    printf("Ecriture de la valeur 0x%02X dans le registre 0x%02X du BMP280\r\n", value, reg);
    
    // Petite pause pour laisser le registre se mettre à jour
    HAL_Delay(10);
    
    // Relire pour vérifier
    HAL_I2C_Mem_Read(&hi2c1, BMP280_I2C_ADDRESS<<1, reg, 1, data_buffer, 1, HAL_MAX_DELAY);
    printf("Valeur lue dans le registre 0x%02X: 0x%02X\r\n", reg, data_buffer[0]);
}

void BMP280_read_several_registers(uint8_t start_reg, uint8_t end_reg, uint8_t* buffer){
    uint8_t num_regs = end_reg - start_reg + 1;
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){start_reg}, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, num_regs, HAL_MAX_DELAY);
    printf("Valeurs lues a partir du registre 0x%02X:\r\n", start_reg);
    for(uint8_t i = 0; i < num_regs; i++){
        printf("Registre 0x%02X: 0x%02X\r\n", start_reg + i, data_buffer[i]);
        buffer[i] = data_buffer[i];
    }
}

void BMP280_16_bits_word(uint8_t reg1, uint8_t reg2){
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){reg1}, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, 2, HAL_MAX_DELAY);
    uint16_t word = (data_buffer[0] << 8) | data_buffer[1];
    printf("Mot 16 bits lu a partir des registres 0x%02X et 0x%02X: 0x%04X\r\n", reg1, reg2, word);
}

int BMP280_read_raw_temp(){
  BMP280_read_several_registers(0xFA, 0xFC, data_buffer);
  int raw_temp = (data_buffer[0] << 12) | data_buffer[1] << 4 | (data_buffer[2] >> 4);
  printf("Temperature brute lue: %d\r\n", raw_temp);
  return raw_temp;
}

int  BMP280_read_raw_pressure(){
  BMP280_read_several_registers(0xF7, 0xF9, data_buffer);
  int raw_pressure = (data_buffer[0] << 12) | data_buffer[1] << 4 | (data_buffer[2] >> 4);
  printf("Pression brute lue: %d\r\n", raw_pressure);
  return raw_pressure;
}


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;

BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
            ((BMP280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    int64_t var1, var2, p;
    
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    
    return (BMP280_U32_t)p;
}