#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "can.h"
#include "BMP280.h"

void CAN_InitAndStart(void);
void Send_CAN_Command_AutoMode(int16_t angle, int16_t orientation);
void Send_CAN_Command_Set_to_0(void);
void Motor_Test(void);


#endif /* __MOTOR_H__ */