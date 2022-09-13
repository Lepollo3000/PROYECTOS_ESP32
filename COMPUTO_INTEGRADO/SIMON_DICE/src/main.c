#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "driver/gpio.h"

// DEFINICION ESTRUCTURA PARA ESTADO DE LEDS
typedef struct Data_t
{
    gpio_num_t pinSalida;
    uint32_t pinPosicionEnArreglo;
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
const uint8_t countRows = 4;
const uint8_t countColumns = 4;
const uint8_t rowsPins[countRows] = { 35, 34, 15, 2 };
const uint8_t columnsPins[countColumns] = { 4, 16, 17, 5 };
char keys[countRows][countColumns] = {
  { '1','2','3', 'A' },
  { '4','5','6', 'B' },
  { '7','8','9', 'C' },
  { '#','0','*', 'D' }
};
#define LED_INPUT_PINES                 \
    {                                   \
        35, 34, 15, 2, 4, 16, 17, 5, 18 \
    }
// DELAY DE INTERRUPCION
#define INTERRUPT_DELAY 250

// ------------------ DECLARACION VARIABLES GLOBALES ------------------

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE SALIDA PARA LOS LEDS
const int pinesSalidasLed[] = LED_OUTPUT_PINES;

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE ENTRADA PARA LOS LEDS
const int pinesEntradasLed[] = LED_INPUT_PINES;

// SE DECLARA UN ARREGLO DE LEDS DEL TAMAÑO DE CANTIDAD DE PINES
structLed leds[sizeof(pinesSalidasLed) / sizeof(int)];

// HANDLER TAREA DE REBOTE
TaskHandle_t HandlersReboteBtn[sizeof(pinesEntradasLed) / sizeof(int)];

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
    for (int i = 0; i < sizeof(pinesEntradasLed) / sizeof(int); i++)
    {
        xTaskCreate(rebote_boton, "Task_Rebote_" + i, 4096, (void *)&leds[i], 1, &HandlersReboteBtn[i]);
    }
}

void app_main_setup()
{
    // DECLARACION ENTRADAS
    for (int i = 0; i < sizeof(pinesEntradasLed) / sizeof(int); i++)
    {
        gpio_pad_select_gpio(pinesEntradasLed[i]);
        gpio_set_direction(pinesEntradasLed[i], GPIO_MODE_INPUT);
        // SE ACTIVA LA RESISTENCIA INTERNA EN MODO PULL-DOWN
        gpio_pullup_en(pinesEntradasLed[i]);
    }

    // DECLARACION SALIDAS
    for (int i = 0; i < sizeof(pinesSalidasLed) / sizeof(int); i++)
    {
        gpio_pad_select_gpio(pinesSalidasLed[i]);
        gpio_set_direction(pinesSalidasLed[i], GPIO_MODE_OUTPUT);

        // (INICIALIZAR EL ESTADO EN TRUE PARA QUE AL INICIAR EL PROGRAMA ESTE CAMBIE A FALSE)
        structLed data = {pinesSalidasLed[i], i, true};
        leds[i] = data;
    }

    // SE CONFIGURA EL ISR PARA QUE CARGUE DESDE RAM Y NO DESDE FLASH
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    // CONFIGURACION ISR <<INTERRUP SERVICE ROUTINE>>
    for (int i = 0; i < sizeof(pinesEntradasLed) / sizeof(int); i++)
    {
        // CONFIGURACION DEL TIPO DE EVENTO PARA TRIGGER DEL BOTON
        // EVENTO AL DEJAR DE PRESIONAR EL BOTON (GPIO_INTR_NEGEDGE)
        // EVENTO AL PRESIONAR EL BOTON (GPIO_INTR_POSEDGE)
        gpio_set_intr_type(pinesEntradasLed[i], GPIO_INTR_POSEDGE);

        // SE AGREGA EL HANDLER PARA EL ISR
        gpio_isr_handler_add(pinesEntradasLed[i], isr_handler_boton, &pinesEntradasLed[i]);

        // SE CONFIGURA EL HANDLER PARA LA TAREA
        TaskHandle_t data = NULL;
        HandlersReboteBtn[i] = data;
    }

    // SE CREA EL HANDLER DE TIPO SEMAFORO PARA MULTITASKING
    HandlerSemaphoreBtn = xSemaphoreCreateBinary();
    // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
    xSemaphoreGive(HandlerSemaphoreBtn);
}

bool readKeypad()
{
  bool rst = false;
  // Barrido de columnas
  for (byte c = 0; c < countColumns; c++)
  {
    // Poner columna a LOW
    pinMode(columnsPins[c],OUTPUT);
    digitalWrite(columnsPins[c], LOW);
    
    // Barrer todas las filas comprobando pulsaciones
    for (byte r = 0; r < countRows; r++)
    {
      if (digitalRead(columnsPins[r]) == LOW)   
      {
        // Pulsacion detectada, guardar fila y columna
        iRow = r;
        iCol = c;
        rst = true; 
      }
    }
    // Devolver la columna a alta impedancia
    digitalWrite(columnsPins[c], HIGH);
    pinMode(columnsPins[c], INPUT);
  }
  return rst;
}

void IRAM_ATTR isr_handler_boton(void *arg)
{
    TaskHandle_t *data = (TaskHandle_t *)arg;

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

        // UN DELAY PEQUEÑO PARA EVITAR CUALQUIER ERROR
        vTaskDelay(INTERRUPT_DELAY / portTICK_PERIOD_MS);
        // CAMBIA EL ESTADO DEL LED
        // data->pinEstado = !data->pinEstado;
        // SE ACTUALIZA EL ESTADO DEL LED
        // gpio_set_level(data->pinSalida, data->pinEstado);
        printf("Button %i pressed!\n", data->pinPosicionEnArreglo);

        // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
        xSemaphoreGiveFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);
        // SE AUTOSUSPENDE LA TAREA
        vTaskSuspend(NULL);
    }
}