#include "display_7_segmentos_mux.h"

void show_individual_number_display(uint8_t numero)
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

void show_mux_number_display(uint16_t numero)
{
    uint8_t unidades = numero % 10;
    show_unidades();
    show_individual_number_display(unidades);
    vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);

    uint8_t decenas = (numero % 100) / 10;
    show_decenas();
    show_individual_number_display(decenas);
    vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);

    uint8_t centenas = (numero % 1000) / 100;
    show_centenas();
    show_individual_number_display(centenas);
    vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);

    uint8_t miles = numero / 1000;
    show_millares();
    show_individual_number_display(miles);
    vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
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
