#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define TIME_DELAY 100

// DECLARACION PUERTOS
#define PIN_LED GPIO_NUM_2
#define BTN_LED GPIO_NUM_4

void startup();

void app_main()
{
    startup();

    while (1)
    {
        
    }
    
}

void startup()
{
    // INICIALIZACION PINES
    gpio_reset_pin(PIN_LED);
    gpio_reset_pin(BTN_LED);

    // DECLARACION SALIDAS
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

    // DECLARACION ENTRADAS
    gpio_set_direction(BTN_LED, GPIO_MODE_INPUT);
}