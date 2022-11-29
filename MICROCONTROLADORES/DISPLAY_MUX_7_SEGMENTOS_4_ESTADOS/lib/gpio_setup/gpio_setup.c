#include "gpio_setup.h"

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

    gpio_pad_select_gpio(COMUN_MILLARES);
    gpio_set_direction(COMUN_MILLARES, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(COMUN_CENTENAS);
    gpio_set_direction(COMUN_CENTENAS, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(COMUN_DECENAS);
    gpio_set_direction(COMUN_DECENAS, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(COMUN_UNIDADES);
    gpio_set_direction(COMUN_UNIDADES, GPIO_MODE_OUTPUT);

    // DECLARACION ENTRADAS
    gpio_pad_select_gpio(BTN_1);
    gpio_set_direction(BTN_1, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(BTN_2);
    gpio_set_direction(BTN_2, GPIO_MODE_INPUT);

    // SE ACTIVAN LA RESISTENCIAS INTERNA EN MODO PULL-DOWN
    gpio_pulldown_en(BTN_1);
    gpio_pulldown_en(BTN_2);

    adc2_config_channel_atten(POTENCIOMETRO, ADC_ATTEN_0db);
}