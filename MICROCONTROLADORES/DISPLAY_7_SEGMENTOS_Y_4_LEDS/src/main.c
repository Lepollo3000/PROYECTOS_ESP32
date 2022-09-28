#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"

#define BTN_1 GPIO_NUM_2
#define BTN_2 GPIO_NUM_4

#define LED_4 GPIO_NUM_16
#define LED_3 GPIO_NUM_17
#define LED_2 GPIO_NUM_5
#define LED_1 GPIO_NUM_18

#define SEGMENTO_A GPIO_NUM_13
#define SEGMENTO_B GPIO_NUM_12
#define SEGMENTO_C GPIO_NUM_14
#define SEGMENTO_D GPIO_NUM_27
#define SEGMENTO_E GPIO_NUM_26
#define SEGMENTO_F GPIO_NUM_25
#define SEGMENTO_G GPIO_NUM_33
#define SEGMENTO_DP GPIO_NUM_32

#define DELAY_TIME 300

void app_main_setup();
void show_number_display(int8_t numero);
void show_numberBinario(int8_t numero);
bool button_pressed(gpio_num_t pin);

void app_main()
{
    app_main_setup();

    int8_t x = 0;
    int8_t y = 0;

    while (1)
    {
        // 0 && 0
        if (!button_pressed(BTN_1) && !button_pressed(BTN_2))
        {
            // X = E
            x = 14;
            y = 15;
        }
        // 0 && 1
        else if (!button_pressed(BTN_1) && button_pressed(BTN_2))
        {
            if (x == 2 || x == 14 || y < 0)
            {
                y = 16;
            }

            x = 1;
            y--;
        }
        // 1 && 0
        else if (button_pressed(BTN_1) && !button_pressed(BTN_2))
        {
            if (x == 1 || x == 14 || y < 0)
            {
                y = -1;
            }

            x = 2;
            y++;
        }
        // 1 && 1
        else if (button_pressed(BTN_1) && button_pressed(BTN_2))
        {
            if (y > 0)
            {
                x = 10;
            }

            x--;
            y = 0;
        }

        // RESETEAR CONTEO DISPLAY
        if (x < 0)
            x = 9;

        // RESETEAR CONTEO LEDS
        if (y > 15)
            y = 0;
        if (y < 0)
            y = 15;

        show_number_display(x);
        show_numberBinario(y);

        printf("x = %d\n", x);
        printf("y = %d\n\n", y);

        vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
}

void app_main_setup()
{
    // DECLARACION SALIDAS
    gpio_pad_select_gpio(SEGMENTO_A);
    gpio_set_direction(SEGMENTO_A, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_B);
    gpio_set_direction(SEGMENTO_B, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_C);
    gpio_set_direction(SEGMENTO_C, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_D);
    gpio_set_direction(SEGMENTO_D, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_E);
    gpio_set_direction(SEGMENTO_E, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_F);
    gpio_set_direction(SEGMENTO_F, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_G);
    gpio_set_direction(SEGMENTO_G, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEGMENTO_DP);
    gpio_set_direction(SEGMENTO_DP, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LED_1);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_2);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_3);
    gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_4);
    gpio_set_direction(LED_4, GPIO_MODE_OUTPUT);

    // DECLARACION ENTRADAS
    gpio_pad_select_gpio(BTN_1);
    gpio_set_direction(BTN_1, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(BTN_2);
    gpio_set_direction(BTN_2, GPIO_MODE_INPUT);
    // SE ACTIVAN LAS RESISTENCIAS INTERNAS EN MODO PULL-DOWN
    gpio_pulldown_en(BTN_1);
    gpio_pulldown_en(BTN_2);
}

void show_number_display(int8_t numero)
{
    switch (numero)
    {
    case 1:
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, false);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 2:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 3:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 4:
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 5:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 6:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 7:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, false);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 8:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    case 9:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    // A
    case 10:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    // B
    case 11:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    // C
    case 12:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, false);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    // D
    case 13:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, false);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    // E
    case 14:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    // F
    case 15:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        gpio_set_level(SEGMENTO_DP, true);
        break;
    case 0:
    default:
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, false);
        gpio_set_level(SEGMENTO_DP, false);
        break;
    }
}

void show_numberBinario(int8_t numero)
{
    switch (numero)
    {
    case 1:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, true);
        break;

    case 2:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, false);
        break;

    case 3:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, true);
        break;

    case 4:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, false);
        break;

    case 5:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, true);
        break;

    case 6:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, false);
        break;

    case 7:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, true);
        break;
    case 8:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, false);
        break;

    case 9:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, true);
        break;
    case 10:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, false);
        break;
    case 11:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, true);
        break;
    case 12:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, false);
        break;
    case 13:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, true);
        break;
    case 14:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, false);
        break;
    case 15:
        gpio_set_level(LED_1, true);
        gpio_set_level(LED_2, true);
        gpio_set_level(LED_3, true);
        gpio_set_level(LED_4, true);
        break;
    case 0:
    default:
        gpio_set_level(LED_1, false);
        gpio_set_level(LED_2, false);
        gpio_set_level(LED_3, false);
        gpio_set_level(LED_4, false);
        break;
    }
}

bool button_pressed(gpio_num_t pin)
{
    return gpio_get_level(pin) == 1;
}