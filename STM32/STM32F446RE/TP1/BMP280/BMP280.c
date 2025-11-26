#include "BMP280.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
extern I2C_HandleTypeDef hi2c1;
uint8_t data_buffer[6];

void BMP280_read_data_id(){
    HAL_I2C_Master_Transmit(&hi2c1, 0x77<<1, (uint8_t[]){0xD0}, 1, HAL_MAX_DELAY);
    printf("Demande de lecture de l'ID du capteur BMP280\r\n");
    HAL_I2C_Master_Receive(&hi2c1, 0x77<<1, data_buffer, 6, HAL_MAX_DELAY);
    printf("ID du capteur BMP280: 0x%02X\r\n", data_buffer[0]);
} 