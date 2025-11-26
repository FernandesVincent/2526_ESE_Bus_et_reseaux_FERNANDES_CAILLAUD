#ifndef __BMP280_H__
#define __BMP280_H__

#include <stdint.h>

#define BMP280_I2C_ADDRESS 0x77

void BMP280_read_register(uint8_t reg);
void BMP280_write_register(uint8_t reg, uint8_t value);
void BMP280_read_several_registers(uint8_t start_reg, uint8_t end_reg, uint8_t* data_infos);
void BMP280_16_bits_word(uint8_t reg1, uint8_t reg2);
void BMP280_read_raw_temp();
void BMP280_read_raw_pressure();
#endif /* __BMP280_H__ */

