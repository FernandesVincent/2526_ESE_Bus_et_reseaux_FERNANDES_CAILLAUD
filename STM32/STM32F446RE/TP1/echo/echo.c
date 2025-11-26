#include <stdio.h>
#include <string.h>
#include "echo.h"
#include "stm32f4xx_hal.h"

char message[100];

int echo(){
  printf("Entrez un message: ");
  fgets(message, sizeof(message), stdin);
  printf("Vous avez entre: %s", message);
  return 0;
};