#include "main.h"
#include "MPU9250.h"

extern I2C_HandleTypeDef hi2c1;

static void I2C_Write(uint16_t dev, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    HAL_I2C_Master_Transmit(&hi2c1, dev, buf, 2, HAL_MAX_DELAY);
}

static void I2C_Read(uint16_t dev, uint8_t reg, uint8_t *buf, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c1, dev, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, dev, buf, len, HAL_MAX_DELAY);
}

static int16_t make16(uint8_t h, uint8_t l)
{
    return (int16_t)((h << 8) | l);
}

void MPU9250_Init(void)
{
    I2C_Write(MPU9250_ADDR, PWR_MGMT_1, 0x00);
    I2C_Write(MPU9250_ADDR, CONFIG, 0x01);
    I2C_Write(MPU9250_ADDR, GYRO_CONFIG, 0x00);
    I2C_Write(MPU9250_ADDR, ACCEL_CONFIG, 0x00);
    I2C_Write(MPU9250_ADDR, INT_PIN_CFG, 0x02);

    HAL_Delay(10);
    I2C_Write(AK8963_ADDR, AK8963_CNTL1, 0x16);
    HAL_Delay(10);
}

void MPU9250_ReadAccelRaw(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t data[6];
    I2C_Read(MPU9250_ADDR, ACCEL_XOUT_H, data, 6);

    *ax = make16(data[0], data[1]);
    *ay = make16(data[2], data[3]);
    *az = make16(data[4], data[5]);
}

void MPU9250_ReadGyroRaw(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t data[6];
    I2C_Read(MPU9250_ADDR, GYRO_XOUT_H, data, 6);

    *gx = make16(data[0], data[1]);
    *gy = make16(data[2], data[3]);
    *gz = make16(data[4], data[5]);
}

void MPU9250_ReadMagRaw(int16_t *mx, int16_t *my, int16_t *mz)
{
    uint8_t data[6];
    I2C_Read(AK8963_ADDR, AK8963_HXL, data, 6);

    *mx = make16(data[1], data[0]);
    *my = make16(data[3], data[2]);
    *mz = make16(data[5], data[4]);
}
