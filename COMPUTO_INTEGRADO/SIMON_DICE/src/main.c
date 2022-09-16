#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "driver/gpio.h"

// DEFINICION ESTRUCTURA PARA ESTADO DE LEDS
typedef struct Data_t
{
    gpio_num_t pinSalida;
    bool pinEstado;
} structLed;

// DEFINICION PARA REFERENCIA DE ISR
#define ESP_INR_FLAG_DEFAULT 0

// DEFINICION PINES SALIDA
#define LED_OUTPUT_PINES                   \
    {                                      \
        13, 12, 14, 27, 26, 25, 33, 32, 23 \
    }
// DEFINICION PINES ENTRADA
#define LED_INPUT_PINES                 \
    {                                   \
        35, 34, 15, 2, 4, 16, 17, 5, 18 \
    }

// DELAY DE INTERRUPCION
#define INTERRUPT_DELAY 250

// ------------------ DECLARACION VARIABLES GLOBALES ------------------

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE ENTRADA PARA LOS LEDS
const gpio_num_t pinesEntradaLed[] = LED_INPUT_PINES;

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE SALIDA PARA LOS LEDS
const gpio_num_t pinesSalidaLed[] = LED_OUTPUT_PINES;

// SE DECLARA UN ARREGLO DE LEDS DEL TAMAÑO DE CANTIDAD DE PINES
structLed leds[sizeof(pinesSalidaLed) / sizeof(gpio_num_t)];

// HANDLER TAREA DE REBOTE
TaskHandle_t HandlersReboteBtn[sizeof(pinesEntradaLed) / sizeof(gpio_num_t)];

// HANDLER SEMAFORO (THREAD SAFE)
SemaphoreHandle_t HandlerSemaphoreBtn = NULL;

// DECLARACION FUNCIONES
void app_main_setup();
bool readKeypad();
void rebote_boton(void *arg);
void IRAM_ATTR isr_handler_boton(void *arg);

void app_main(void)
{
    // CONFIGURACION GENERAL DE PINES Y SERVICIOS
    app_main_setup();

    // SE CREAN LAS TAREAS PARA REBOTE DE BOTON
    for (int i = 0; i < sizeof(pinesEntradaLed) / sizeof(int); i++)
    {
        xTaskCreate(rebote_boton, "Task_Rebote_" + i, 4096, (void *)&leds[i], 1, &HandlersReboteBtn[i]);
    }
}

void app_main_setup()
{
    // DECLARACION ENTRADAS
    for (int i = 0; i < sizeof(pinesEntradaLed) / sizeof(gpio_num_t); i++)
    {
        gpio_pad_select_gpio(pinesEntradaLed[i]);
        gpio_set_direction(pinesEntradaLed[i], GPIO_MODE_INPUT);
        // SE ACTIVA LA RESISTENCIA INTERNA EN MODO PULL-DOWN
        gpio_pulldown_en(pinesEntradaLed[i]);
    }

    // DECLARACION SALIDAS
    for (int i = 0; i < sizeof(pinesSalidaLed) / sizeof(gpio_num_t); i++)
    {
        gpio_pad_select_gpio(pinesSalidaLed[i]);
        gpio_set_direction(pinesSalidaLed[i], GPIO_MODE_OUTPUT);

        structLed data = {pinesSalidaLed[i], false};
        leds[i] = data;
    }

    // SE CONFIGURA EL ISR PARA QUE CARGUE DESDE RAM Y NO DESDE FLASH
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    // CONFIGURACION ISR <<INTERRUP SERVICE ROUTINE>>
    for (int i = 0; i < sizeof(pinesEntradaLed) / sizeof(gpio_num_t); i++)
    {
        // CONFIGURACION DEL TIPO DE EVENTO PARA TRIGGER DEL BOTON
        // EVENTO AL DEJAR DE PRESIONAR EL BOTON (GPIO_INTR_NEGEDGE)
        // EVENTO AL PRESIONAR EL BOTON (GPIO_INTR_POSEDGE)
        gpio_set_intr_type(pinesEntradaLed[i], GPIO_INTR_POSEDGE);

        // SE CONFIGURA EL HANDLER PARA LA TAREA
        TaskHandle_t data = NULL;
        HandlersReboteBtn[i] = data;

        // SE AGREGA EL HANDLER PARA EL ISR
        gpio_isr_handler_add(pinesEntradaLed[i], isr_handler_boton, &HandlersReboteBtn[i]);
    }

    // SE CREA EL HANDLER DE TIPO SEMAFORO PARA MULTITASKING
    HandlerSemaphoreBtn = xSemaphoreCreateBinary();
    // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
    xSemaphoreGive(HandlerSemaphoreBtn);
}

void IRAM_ATTR isr_handler_boton(void *arg)
{
    TaskHandle_t *data = (TaskHandle_t *)arg;

    printf("OvO %p \n", data);
    // SE REANUDA LA TAREA
    xTaskResumeFromISR(data);
}

void rebote_boton(void *arg)
{
    BaseType_t *xHigherPriorityTaskWoken = pdFALSE;
    structLed *data = (structLed *)arg;

    while (1)
    {
        // SE APROPIA DEL SEMAFORO PARA AVISAR QUE UTILIZARA LA VARIABLE DE LED
        xSemaphoreTakeFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);

        // CAMBIA EL ESTADO DEL LED
        data->pinEstado = !data->pinEstado;
        // SE ACTUALIZA EL ESTADO DEL LED
        gpio_set_level(data->pinSalida, data->pinEstado);
        printf("Button %i pressed!\n", data->pinSalida);

        // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
        xSemaphoreGiveFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);

        // UN DELAY PEQUEÑO PARA EVITAR CUALQUIER ERROR
        vTaskDelay(INTERRUPT_DELAY / portTICK_PERIOD_MS);
        // SE AUTOSUSPENDE LA TAREA
        vTaskSuspend(NULL);
    }
}