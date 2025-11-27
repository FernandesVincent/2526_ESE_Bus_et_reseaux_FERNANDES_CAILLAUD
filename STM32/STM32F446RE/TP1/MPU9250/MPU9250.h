#ifndef MPU9250_H
#define MPU9250_H

#include <stdint.h>

#define MPU9250_ADDR     (0x68 << 1)
#define AK8963_ADDR      (0x0C << 1)

#define PWR_MGMT_1       0x6B
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define INT_PIN_CFG      0x37
#define ACCEL_XOUT_H     0x3B
#define GYRO_XOUT_H      0x43

#define AK8963_CNTL1     0x0A
#define AK8963_HXL       0x03

void MPU9250_Init(void);
void MPU9250_ReadAccelRaw(int16_t *ax, int16_t *ay, int16_t *az);
void MPU9250_ReadGyroRaw(int16_t *gx, int16_t *gy, int16_t *gz);
void MPU9250_ReadMagRaw(int16_t *mx, int16_t *my, int16_t *mz);

#endif
