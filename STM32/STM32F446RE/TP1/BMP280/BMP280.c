#include "BMP280.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
extern I2C_HandleTypeDef hi2c1;
uint8_t data_buffer[64];



void BMP280_read_register(uint8_t reg){
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){reg}, 1, HAL_MAX_DELAY);
    printf("Demande de lecture de l'ID du capteur BMP280\r\n");
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, 6, HAL_MAX_DELAY);
    printf("ID du capteur BMP280: 0x%02X\r\n", data_buffer[0]);
} 

void BMP280_write_register(uint8_t reg, uint8_t value){
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){reg, value}, 2, HAL_MAX_DELAY);
    printf("Ecriture de la valeur 0x%02X dans le registre 0x%02X du BMP280\r\n", value, reg);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, 1, HAL_MAX_DELAY);
    printf("Valeur lue dans le registre 0x%02X: 0x%02X\r\n", reg, data_buffer[0]);
}

uint8_t data_infos[64];

void BMP280_read_several_registers(uint8_t start_reg, uint8_t end_reg, uint8_t* buffer){
    uint8_t num_regs = end_reg - start_reg + 1;
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){start_reg}, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, num_regs, HAL_MAX_DELAY);
    printf("Valeurs lues a partir du registre 0x%02X:\r\n", start_reg);
    for(uint8_t i = 0; i < num_regs; i++){
        printf("Registre 0x%02X: 0x%02X\r\n", start_reg + i, data_buffer[i]);
        buffer[i] = data_buffer[i];
    }
    // printf("Fin de la lecture des registres.\r\n");
    // printf("Stockage des valeurs lues dans un tableau data_infos.\r\n");
    // printf("Contenu du tableau data_infos:\r\n");
    // for(uint8_t i = 0; i < num_regs; i++){
    //     printf("data_infos[%d] = 0x%02X\r\n", i, data_infos[i]);
    // };
}

void BMP280_16_bits_word(uint8_t reg1, uint8_t reg2){
    HAL_I2C_Master_Transmit(&hi2c1, BMP280_I2C_ADDRESS<<1, (uint8_t[]){reg1}, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, BMP280_I2C_ADDRESS<<1, data_buffer, 2, HAL_MAX_DELAY);
    uint16_t word = (data_buffer[0] << 8) | data_buffer[1];
    printf("Mot 16 bits lu a partir des registres 0x%02X et 0x%02X: 0x%04X\r\n", reg1, reg2, word);
}

void BMP280_read_raw_temp(){
  BMP280_read_several_registers(0xFA, 0xFC, data_infos);
  int raw_temp = (data_infos[0] << 12) | data_infos[1] << 4 | (data_infos[2] >> 4);
  printf("Temperature brute lue: %d\r\n", raw_temp);
}

void BMP280_read_raw_pressure(){

}