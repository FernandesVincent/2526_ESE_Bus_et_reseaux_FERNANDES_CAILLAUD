#include "echo.h"
#include "BMP280.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

int K = 5;

void Rasbpi_protocol(char *message) {
  if (strcmp(message, "GET_T") == 0) {
    printf("Commande GET_T recue\r\n");
    int raw_temp = BMP280_read_raw_temp();
    printf("T=%05XH\r\n", raw_temp & 0xFFFFF);
    // int compensate_temp = bmp280_compensate_T_int32(raw_temp);
    // int temp_celsius = compensate_temp / 100.0f;
    // printf("Temperature compensee: %d °C\r\n", temp_celsius);

  } else if (strcmp(message, "GET_P") == 0) {
    printf("Commande GET_P recue\r\n");
    int raw_press = BMP280_read_raw_pressure();
    printf("P=%05XH\r\n", raw_press & 0xFFFFF);
    // int compensate_press = bmp280_compensate_P_int64(raw_press);
    // int press_pa = compensate_press / 256.0f;     // Format Q24.8
    // int press_hpa = press_pa / 100;               // Conversion en hPa
    // printf("Pression compensee: %d hPa\r\n", press_hpa);
  } else if (strcmp(message, "GET_K") == 0) {
    printf("Commande GET_K recue\r\n");
    printf("K=%d\r\n", K);

  } else if (strcmp(message, "GET_A") == 0) {
    printf("Commande GET_A recue\r\n");
    printf("A=000C8H\r\n");

  } else if (strncmp(message, "SET_K=", 6) == 0) {
    printf("Commande SET_K recue\r\n");
    char *value_str = message + 6;
    K = atoi(value_str);   
    printf("K=OK\r\n");
    printf("Nouvelle valeur de K: %d\r\n", K);
  }
  else {
    printf("Commande inconnue: %s\r\n", message);
  }
}
