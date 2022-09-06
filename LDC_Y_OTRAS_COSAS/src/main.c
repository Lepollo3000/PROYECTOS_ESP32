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
void parpadear_led();
bool detectar_boton(gpio_num_t n);
void encender_led(gpio_num_t n);
void apagar_led(gpio_num_t n);

void app_main()
{
    startup();

    while (1)
    {
        /*
        // parpadear_led();
        if (gpio_get_level(BTN_LED) == 1)
            encender_led(PIN_LED);
        else
            apagar_led(PIN_LED);

        vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
        */
        if (gpio_get_level(BTN_LED) == 1)
        {
            printf("encendido\n");
            gpio_set_level(PIN_LED, 1);
        }
        else
        {
            printf("apagado\n");
            gpio_set_level(PIN_LED, 0);
        }

        vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
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

void parpadear_led()
{
    gpio_set_level(PIN_LED, 1);
    vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
    gpio_set_level(PIN_LED, 0);
    vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
}
/*
bool detectar_boton(gpio_num_t n)
{
    return gpio_get_level(n) == 1;
}

void encender_led(gpio_num_t n)
{
    printf("encendido\n");
    gpio_set_level(n, 1);
}

void apagar_led(gpio_num_t n)
{
    printf("apagado\n");
    gpio_set_level(n, 0);
}
*/