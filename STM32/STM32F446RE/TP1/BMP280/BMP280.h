#ifndef __BMP280_H__
#define __BMP280_H__

#include <stdint.h>

#define BMP280_I2C_ADDRESS 0x77
#define BMP280_CHIP_ID_REG 0xD0
#define BMP280_DIG_T1_LSB_REG 0x88
#define BMP280_DIG_T1_MSB_REG 0x89
#define BMP280_DIG_T2_LSB_REG 0x8A
#define BMP280_DIG_T2_MSB_REG 0x8B
#define BMP280_DIG_T3_LSB_REG 0x8C
#define BMP280_DIG_T3_MSB_REG 0x8D  
#define BMP280_DIG_P1_LSB_REG 0x8E
#define BMP280_DIG_P1_MSB_REG 0x8F
#define BMP280_DIG_P2_LSB_REG 0x90
#define BMP280_DIG_P2_MSB_REG 0x91
#define BMP280_DIG_P3_LSB_REG 0x92
#define BMP280_DIG_P3_MSB_REG 0x93
#define BMP280_DIG_P4_LSB_REG 0x94
#define BMP280_DIG_P4_MSB_REG 0x95
#define BMP280_DIG_P5_LSB_REG 0x96
#define BMP280_DIG_P5_MSB_REG 0x97
#define BMP280_DIG_P6_LSB_REG 0x98
#define BMP280_DIG_P6_MSB_REG 0x99
#define BMP280_DIG_P7_LSB_REG 0x9A
#define BMP280_DIG_P7_MSB_REG 0x9B
#define BMP280_DIG_P8_LSB_REG 0x9C
#define BMP280_DIG_P8_MSB_REG 0x9D
#define BMP280_DIG_P9_LSB_REG 0x9E
#define BMP280_DIG_P9_MSB_REG 0x9F

typedef int32_t BMP280_S32_t;
typedef uint32_t BMP280_U32_t;
typedef int64_t BMP280_S64_t;
typedef uint16_t BMP280_U16_t;
typedef int16_t BMP280_S16_t;

// === VARIABLES GLOBALES ===
extern BMP280_S32_t t_fine; // DOIT être calculé par temperature d'abord!

// Paramètres de calibration pression (à lire UNE FOIS)
extern BMP280_U16_t dig_P1; // unsigned
extern BMP280_S16_t dig_P2; // signed
extern BMP280_S16_t dig_P3; // signed
extern BMP280_S16_t dig_P4; // signed
extern BMP280_S16_t dig_P5; // signed
extern BMP280_S16_t dig_P6; // signed
extern BMP280_S16_t dig_P7; // signed
extern BMP280_S16_t dig_P8; // signed
extern BMP280_S16_t dig_P9; // signed

// Paramètres de calibration température (à lire UNE FOIS)
extern BMP280_U16_t dig_T1; // unsigned
extern BMP280_S16_t dig_T2; // signed
extern BMP280_S16_t dig_T3; // signed


void BMP280_Init(void);
void BMP280_read_register(uint8_t reg);
void BMP280_write_register(uint8_t reg, uint8_t value);
void BMP280_read_several_registers(uint8_t start_reg, uint8_t end_reg, uint8_t* data_buffer);
void BMP280_16_bits_word(uint8_t reg1, uint8_t reg2);
int BMP280_read_raw_temp();
int BMP280_read_raw_pressure();
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);

#endif /* __BMP280_H__ */

