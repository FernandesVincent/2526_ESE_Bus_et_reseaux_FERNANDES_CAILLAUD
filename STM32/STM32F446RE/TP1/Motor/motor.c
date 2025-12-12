#include "motor.h"

extern CAN_HandleTypeDef hcan1;

void CAN_InitAndStart(void){
    if (HAL_CAN_Start(&hcan1) != HAL_OK){
        printf("Error starting CAN\r\n");
    } else {
        printf("CAN started\r\n");
    }
}

void Send_CAN_Command_Set_to_0(){
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;

    TxHeader.StdId = 0x62;       
    TxHeader.ExtId = 0;           
    TxHeader.IDE = CAN_ID_STD;    
    TxHeader.RTR = CAN_RTR_DATA;  
    TxHeader.DLC = 0;             
    TxHeader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, 0, &TxMailbox) != HAL_OK){
        printf("Error sending set to 0\r\n");
    } 
    else {
        printf("CAN message set to 0 sendt\r\n");
    }
}

int raw_temp_ref = 0;

void Init_temp_reference() {
    raw_temp_ref = BMP280_read_raw_temp();
}

void Send_CAN_Command_prop_temp(int coefficient){
    int raw_temp = BMP280_read_raw_temp();
    int16_t delta_temp = raw_temp - raw_temp_ref;
    printf("Delta temperature: %d\r\n", delta_temp);
    printf("Coefficient K: %d\r\n", coefficient);
    int16_t angle = ((coefficient * delta_temp)-delta_temp)/1000;
    int16_t orientation;
    if(angle > 0){
        orientation = 0x01;
    }
    else{
        orientation = 0x00;
        angle = -angle;
    }
    Send_CAN_Command_AutoMode(angle, orientation);
}


void Send_CAN_Command_AutoMode(int16_t angle, int16_t orientation){
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[2];
    uint32_t TxMailbox;

    TxHeader.StdId = 0x61;       
    TxHeader.ExtId = 0;           
    TxHeader.IDE = CAN_ID_STD;    
    TxHeader.RTR = CAN_RTR_DATA;  
    TxHeader.DLC = 2;             
    TxHeader.TransmitGlobalTime = DISABLE;

    TxData[0] = (uint8_t)angle;         
    TxData[1] = (uint8_t)orientation;    

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK){
        printf("Error sending CAN message\r\n");
    } 
    else {
        printf("CAN message test sent: angle=%d, orientation=%d\r\n", angle, orientation);
    }
}

extern int K;

void Motor_Test(void){
    CAN_InitAndStart();
    Send_CAN_Command_Set_to_0();
    Init_temp_reference();
    HAL_Delay(1000);
    while (1){
        // Send_CAN_Command_prop_temp(5);
        Send_CAN_Command_prop_temp(K);
        HAL_Delay(1000);
        // Send_CAN_Command_AutoMode(0x5A, 0x00);
        // HAL_Delay(1000);
        // Send_CAN_Command_AutoMode(0x5A, 0x01);
        // HAL_Delay(1000);
    }
}
