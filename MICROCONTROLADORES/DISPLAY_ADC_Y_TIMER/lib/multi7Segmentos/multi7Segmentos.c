#include "multi7Segmentos.h"
#include "driver/gpio.h"

void show_letter_display(char letra)
{
    switch (letra)
    {
    case 'A':
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        break;
    case 'L':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, false);
        break;
    case 'T':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        break;
    case 'B':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        break;
    case 'I':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, false);
        break;
    case 'E':
        gpio_set_level(SEGMENTO_A, true);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, true);
        gpio_set_level(SEGMENTO_G, true);
        break;
    case 'N':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, true);
        break;
    case 'J':
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, true);
        gpio_set_level(SEGMENTO_C, true);
        gpio_set_level(SEGMENTO_D, true);
        gpio_set_level(SEGMENTO_E, true);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, false);
        break;
    case ' ':
    default:
        gpio_set_level(SEGMENTO_A, false);
        gpio_set_level(SEGMENTO_B, false);
        gpio_set_level(SEGMENTO_C, false);
        gpio_set_level(SEGMENTO_D, false);
        gpio_set_level(SEGMENTO_E, false);
        gpio_set_level(SEGMENTO_F, false);
        gpio_set_level(SEGMENTO_G, false);
        break;
    }
}

void show_unidades()
{
    gpio_set_level(COMUN_MILLARES, true);
    gpio_set_level(COMUN_CENTENAS, true);
    gpio_set_level(COMUN_DECENAS, true);
    gpio_set_level(COMUN_UNIDADES, false);
}

void show_decenas()
{
    gpio_set_level(COMUN_MILLARES, true);
    gpio_set_level(COMUN_CENTENAS, true);
    gpio_set_level(COMUN_DECENAS, false);
    gpio_set_level(COMUN_UNIDADES, true);
}

void show_centenas()
{
    gpio_set_level(COMUN_MILLARES, true);
    gpio_set_level(COMUN_CENTENAS, false);
    gpio_set_level(COMUN_DECENAS, true);
    gpio_set_level(COMUN_UNIDADES, true);
}

void show_millares()
{
    gpio_set_level(COMUN_MILLARES, false);
    gpio_set_level(COMUN_CENTENAS, true);
    gpio_set_level(COMUN_DECENAS, true);
    gpio_set_level(COMUN_UNIDADES, true);
}