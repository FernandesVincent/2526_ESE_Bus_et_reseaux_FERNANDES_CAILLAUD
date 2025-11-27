#include <stdio.h>
#include <string.h>
#include "echo.h"
#include "stm32f4xx_hal.h"
#include "BMP280.h"
#include "usart.h"

char message[100];
char buffer[100];

int echo(){
  printf("Entrez un message: ");
  fgets(message, sizeof(message), stdin);
  printf("Vous avez entre: %s", message);
  return 0;
};

void Rasbpi_protocol(char *message){
      int i = 0;
      while(*message != '\0' && *message != '\r' && *message != '\n' && i < sizeof(buffer)-1){
          buffer[i++] = *message++;
      }

      buffer[i] = '\0';

      if( strcmp(buffer, "GET_T") == 0 ){
        printf("Commande GET_T recue\r\n");
        int raw_temp = BMP280_read_raw_temp();
        int compensate_temp = bmp280_compensate_T_int32(raw_temp); 
        char hex[5];
        sprintf(hex, "%X", compensate_temp);
        char response[20];
        sprintf(response, "T:%sH\r\n", hex);
        HAL_UART_Transmit(&huart1, (uint8_t *)response, strlen(response), HAL_MAX_DELAY);
      }
      else if( strcmp(buffer, "GET_P") == 0 ){
        printf("Commande GET_P recue\r\n");
        int raw_press = BMP280_read_raw_pressure();
        int compensate_press = bmp280_compensate_P_int64(raw_press);
        char hex[5];
        sprintf(hex, "%X", compensate_press);
        char response[20];
        sprintf(response, "P:%sH\r\n", hex);
        HAL_UART_Transmit(&huart1, (uint8_t *)response, strlen(response), HAL_MAX_DELAY);
      }
      else if( strcmp(buffer, "GET_K") == 0 ){

      }
      else if( strcmp(buffer, "GET_A") == 0 ){

      }
      else if( strcmp(buffer, "SET_K=1234") == 0 ){

      }

      else {
        printf("Commande inconnue: %s\r\n", buffer);
      }

}
