#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"

#define BTN_1 GPIO_NUM_2
#define BTN_2 GPIO_NUM_4

#define POTENCIOMETRO ADC2_CHANNEL_3

#define COMUN_UNIDADES GPIO_NUM_19
#define COMUN_DECENAS GPIO_NUM_17
#define COMUN_CENTENAS GPIO_NUM_5
#define COMUN_MILLARES GPIO_NUM_18

#define SEGMENTO_A GPIO_NUM_13
#define SEGMENTO_B GPIO_NUM_12
#define SEGMENTO_C GPIO_NUM_14
#define SEGMENTO_D GPIO_NUM_27
#define SEGMENTO_E GPIO_NUM_26
#define SEGMENTO_F GPIO_NUM_25
#define SEGMENTO_G GPIO_NUM_33
#define SEGMENTO_DP GPIO_NUM_32

#define DELAY_TIME 125
#define DELAY_BUTTON 25
#define DELAY_DISPLAY 10

TaskHandle_t maquina_estado_1_task_handler = NULL;
TaskHandle_t maquina_estado_2_task_handler = NULL;
TaskHandle_t maquina_estado_3_task_handler = NULL;
TaskHandle_t maquina_estado_4_task_handler = NULL;

uint16_t contador = 0;

void app_main_setup(void);