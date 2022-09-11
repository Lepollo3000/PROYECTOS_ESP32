#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "driver/gpio.h"

// DEFINICION ESTRUCTURA PARA ESTADO DE LEDS
typedef struct Data_t
{
    gpio_num_t pin;
    bool pin_state;
} structLed;

// DEFINICION PARA REFERENCIA DE ISR
#define ESP_INR_FLAG_DEFAULT 0
// DEFINICION PINES SALIDA
#define LED_OUTPUT_PIN 2
// DEFINICION PINES ENTRADA
#define LED_INPUT_PIN 4
// DELAY DE INTERRUPCION
#define INTERRUPT_DELAY 250

// HANDLER TAREA DE REBOTE
TaskHandle_t HandlerReboteBtn = NULL;
// HANDLER SEMAFORO (THREAD SAFE)
SemaphoreHandle_t HandlerSemaphoreBtn = NULL;

// DECLARACION LEDS (SE DEBE INICIALIZAR EL ESTADO EN TRUE PARA QUE AL INICIAR EL PROGRAMA SE CAMBIE A FALSE)
structLed led1 = {LED_OUTPUT_PIN, true};

// DECLARACION FUNCIONES
void app_main_setup();
void rebote_boton(void *arg);
void IRAM_ATTR isr_handler_boton(void *arg);

void app_main(void)
{
    // CONFIGURACION GENERAL DE PINES Y SERVICIOS
    app_main_setup();

    // SE CREA LA TAREA PARA REBOTE DE BOTON
    xTaskCreate(rebote_boton, "interrupt_task", 4096, (void *)&led1, 1, &HandlerReboteBtn);
}

void app_main_setup()
{
    // DECLARACION ENTRADAS
    gpio_pad_select_gpio(LED_INPUT_PIN);
    gpio_set_direction(LED_INPUT_PIN, GPIO_MODE_INPUT);
    // SE ACTIVA LA RESISTENCIA INTERNA EN MODO PULL-DOWN
    gpio_pulldown_en(LED_INPUT_PIN);

    // DECLARACION SALIDAS
    gpio_pad_select_gpio(LED_OUTPUT_PIN);
    gpio_set_direction(LED_OUTPUT_PIN, GPIO_MODE_OUTPUT);

    // CONFIGURACION PARA EL TRIGGER DE INTERRUPCION DEL BOTON
    // EVENTO AL DEJAR DE PRESIONAR EL BOTON (GPIO_INTR_NEGEDGE)
    // EVENTO AL PRESIONAR EL BOTON (GPIO_INTR_POSEDGE)
    gpio_set_intr_type(LED_INPUT_PIN, GPIO_INTR_POSEDGE);

    // SE CONFIGURA EL ISR<<INTERRUP SERVICE ROUTINE>> PARA QUE CARGUE DESDE RAM Y NO DESDE FLASH
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);
    // SE AGREGA EL HANDLER PARA EL ISR
    gpio_isr_handler_add(LED_INPUT_PIN, isr_handler_boton, NULL);

    // SE CREA EL HANDLER DE TIPO SEMAFORO PARA MULTITASKING
    HandlerSemaphoreBtn = xSemaphoreCreateBinary();
    // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
    xSemaphoreGive(HandlerSemaphoreBtn);
}

void IRAM_ATTR isr_handler_boton(void *arg)
{
    // SE REANUDA LA TAREA
    xTaskResumeFromISR(HandlerReboteBtn);
}

void rebote_boton(void *arg)
{
    BaseType_t *xHigherPriorityTaskWoken = pdFALSE;
    structLed *data = (structLed *)arg;

    while (1)
    {
        // SE APROPIA DEL SEMAFORO PARA AVISAR QUE UTILIZARA LA VARIABLE DE LED
        xSemaphoreTakeFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);

        // UN DELAY PEQUEÑO PARA EVITAR CUALQUIER ERROR
        vTaskDelay(INTERRUPT_DELAY / portTICK_PERIOD_MS);
        // CAMBIA EL ESTADO DEL LED
        data->pin_state = !data->pin_state;
        // SE ACTUALIZA EL ESTADO DEL LED
        gpio_set_level(data->pin, data->pin_state);
        printf("Button pressed!\n");

        // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
        xSemaphoreGiveFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);
        // SE AUTOSUSPENDE LA TAREA
        vTaskSuspend(NULL);
    }
}