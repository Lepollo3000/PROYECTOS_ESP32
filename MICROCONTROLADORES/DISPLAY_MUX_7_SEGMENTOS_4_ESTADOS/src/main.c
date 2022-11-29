#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_wdt.h"
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
#define DELAY_BUTTON 125
#define DELAY_DISPLAY 5

TaskHandle_t maquina_estado_1_task_handler = NULL;
TaskHandle_t maquina_estado_2_task_handler = NULL;
TaskHandle_t maquina_estado_3_task_handler = NULL;
TaskHandle_t maquina_estado_4_task_handler = NULL;

TaskHandle_t cambiar_modo_handler = NULL;

TaskHandle_t isr_boton_handler = NULL;

uint8_t nivel = 1;

void app_main_setup(void);

void show_mux_number_display(uint16_t numero);
void show_individual_number_display(uint8_t numero);
void show_unidades(void);
void show_decenas(void);
void show_centenas(void);
void show_millares(void);

void IRAM_ATTR isr_boton(void *arg);

void cambiar_modo(void *args);

uint8_t aumentar_contador(uint8_t contador);

void maquina_estado_1_task(void *args);
void maquina_estado_2_task(void *args);
void maquina_estado_3_task(void *args);
void maquina_estado_4_task(void *args);

void app_main()
{
    app_main_setup();

    xTaskCreate(maquina_estado_1_task, "Modo 1", 4056, NULL, 5, &maquina_estado_1_task_handler);
    xTaskCreate(maquina_estado_2_task, "Modo 2", 4056, NULL, 5, &maquina_estado_2_task_handler);
    //xTaskCreate(maquina_estado_3_task, "Modo 3", 4056, NULL, 5, &maquina_estado_3_task_handler);
    xTaskCreate(cambiar_modo, "Cambiando Modo", 4056, NULL, 5, &cambiar_modo_handler);
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

    // SE CONFIGURA EL ISR PARA QUE CARGUE DESDE RAM Y NO DESDE FLASH
    gpio_install_isr_service(0);

    // CONFIGURACION DEL TIPO DE EVENTO PARA TRIGGER DEL BOTON
    // EVENTO AL DEJAR DE PRESIONAR EL BOTON (GPIO_INTR_NEGEDGE)
    // EVENTO AL PRESIONAR EL BOTON (GPIO_INTR_POSEDGE)
    gpio_set_intr_type(BTN_2, GPIO_INTR_POSEDGE);

    // SE AGREGA EL HANDLER PARA EL ISR
    gpio_isr_handler_add(BTN_2, isr_boton_handler, (void *) BTN_2);
}

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

void IRAM_ATTR isr_boton(void *arg)
{
    xTaskResumeFromISR(cambiar_modo_handler);
}

void cambiar_modo(void *args)
{
    //vTaskSuspend(maquina_estado_2_task_handler);
    //vTaskSuspend(maquina_estado_3_task_handler);

    while (1)
    {
        vTaskSuspend(NULL);
        vTaskDelay(DELAY_BUTTON / portTICK_PERIOD_MS);

        nivel++;
        printf("nivel %i\n", nivel);

        if (nivel >= 4)
        {
            nivel = 0;
        }

        switch (nivel)
        {
        case 1:
            printf("1\n");
            //vTaskSuspend(maquina_estado_1_task_handler);
            //vTaskResume(maquina_estado_2_task_handler);
            break;
        case 2:
            printf("2\n");
            //vTaskSuspend(maquina_estado_2_task_handler);
            //vTaskResume(maquina_estado_3_task_handler);
            break;
        case 3:
            printf("3\n");
            //vTaskSuspend(maquina_estado_3_task_handler);
            //vTaskResume(maquina_estado_4_task_handler);
            break;
        case 0:
        default:
            printf("4\n");
            //vTaskSuspend(maquina_estado_4_task_handler);
            vTaskResume(maquina_estado_1_task_handler);
            break;
        }
    }
}

uint8_t aumentar_contador(uint8_t contador)
{
    vTaskDelay(DELAY_BUTTON / portTICK_PERIOD_MS);

    if (contador >= 159)
    {
        contador = 0;
    }

    return contador++;
}

void maquina_estado_1_task(void *args)
{
    // contador automático ascendente de 0 a 159
    unsigned long currentTime = 0;
    unsigned long startedTime = 0;
    uint16_t contador = 0;

    while (1)
    {
        currentTime = esp_timer_get_time() / 1000ULL;

        show_mux_number_display(contador);

        if ((currentTime - startedTime) >= DELAY_TIME)
        {
            //printf("%i\n", contador);
            contador++;
            startedTime = currentTime;

            if (contador >= 159)
            {
                contador = 0;
            }
        }
    }
}

void maquina_estado_2_task(void *args)
{
    // contador 0 a 159 deberá de aumentar de forma manual desde un botón "ascendente"
    uint16_t contador = 0;

    while (1)
    {
        show_mux_number_display(contador);

        if (gpio_get_level(BTN_2))
        {
            printf("%i\n", contador);

            contador = aumentar_contador(contador);
        }
    }
}

void maquina_estado_3_task(void *args)
{
    // contador 159 a 0 deberá de descender de forma manual desde un botón "descendente"
}

void maquina_estado_4_task(void *args)
{
    // mostrar la lectura de un potenciómetro ADC (0V → 00, 5V → 999)
}