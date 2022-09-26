#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "HD44780.h"

// DEFINICION ESTRUCTURA PARA ESTADO DE LEDS
typedef struct
{
    gpio_num_t pinSalida;
    bool pinEstado;
} structLed;

typedef struct
{
    TaskHandle_t task;
} structHandler;

// DEFINICION PARA REFERENCIA DE ISR
#define ESP_INR_FLAG_DEFAULT 0

// DEFINICION PINES SALIDA
#define LED_OUTPUT_PINES                                                                                                  \
    {                                                                                                                     \
        GPIO_NUM_2, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_16, GPIO_NUM_26, GPIO_NUM_5, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_23 \
    }
// DEFINICION PINES ENTRADA
#define LED_INPUT_PINES                                                                                                    \
    {                                                                                                                      \
        GPIO_NUM_15, GPIO_NUM_13, GPIO_NUM_4, GPIO_NUM_27, GPIO_NUM_17, GPIO_NUM_25, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21 \
    }

// DEFINICION PINES LCD
#define LCD_ADDR_1 0x27
#define LCD_ADDR_2 0x26
#define SDA_PIN 3
#define SCL_PIN 1
#define LCD_COLS 16
#define LCD_ROWS 2

// DELAY PARA INTERRUPCION DE REBOTE
#define INTERRUPT_DELAY 250
#define DELAY_GAME_INIT 80

// ------------------ DECLARACION VARIABLES GLOBALES ------------------

uint8_t Skull[8] = {0b00000, 0b01110, 0b10101, 0b11011, 0b01110, 0b01110, 0b00000, 0b00000};

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE ENTRADA PARA LOS LEDS
const gpio_num_t pinesEntradaLED[] = LED_INPUT_PINES;

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE SALIDA PARA LOS LEDS
const gpio_num_t pinesSalidaLED[] = LED_OUTPUT_PINES;

// NIVEL DE DIFICULTAD
uint8_t nivelDificultad;

// SABER SI SE ESTÁ JUGANDO ACTUALMENTE
bool juegoActivo;

// SE DECLARA UN ARREGLO DE LEDS DEL TAMAÑO DE CANTIDAD DE PINES
structLed arrLEDs[sizeof(pinesSalidaLED) / sizeof(gpio_num_t)];

// HANDLER TAREA DE REBOTE
structHandler arrHandlersReboteBtn[sizeof(pinesEntradaLED) / sizeof(gpio_num_t)];

// HANDLER SEMAFORO (THREAD SAFE)
SemaphoreHandle_t HandlerSemaphoreBtn = NULL;

// DECLARACION FUNCIONES
#ifdef __cplusplus

extern "C"
{
#endif
    void app_main(void);
    void app_main_setup();

    void IRAM_ATTR isr_handler_boton(void *arg);
    void boton_presionado(void *arg);

    void empezar_juego();
    void revisar_patron(structLed led);
    void LCD_DemoTask(void *param);
#ifdef __cplusplus
}
#endif

void app_main(void)
{
    // CONFIGURACION GENERAL DE PINES Y SERVICIOS
    app_main_setup();

    // xTaskCreate(&LCD_DemoTask, "Task_Rebote_", 4056, NULL, 5, NULL);

    // SE CREAN LAS TAREAS PARA REBOTE DE BOTON
    for (int i = 0; i < sizeof(pinesEntradaLED) / sizeof(gpio_num_t); i++)
    {
        xTaskCreate(boton_presionado, "Task_Rebote_" + i, 4056, (void *)&arrLEDs[i], 1, (TaskHandle_t *)&arrHandlersReboteBtn[i]);
    }
}

void app_main_setup()
{
    // DECLARACION ENTRADAS
    for (int i = 0; i < sizeof(pinesEntradaLED) / sizeof(gpio_num_t); i++)
    {
        gpio_pad_select_gpio(pinesEntradaLED[i]);
        gpio_set_direction(pinesEntradaLED[i], GPIO_MODE_INPUT);
        // SE ACTIVA LA RESISTENCIA INTERNA EN MODO PULL-DOWN
        gpio_pulldown_en(pinesEntradaLED[i]);
    }

    // DECLARACION SALIDAS
    for (int i = 0; i < sizeof(pinesSalidaLED) / sizeof(gpio_num_t); i++)
    {
        gpio_pad_select_gpio(pinesSalidaLED[i]);
        gpio_set_direction(pinesSalidaLED[i], GPIO_MODE_OUTPUT);

        structLed data = {pinesSalidaLED[i], false};
        arrLEDs[i] = data;
    }

    // SE CONFIGURA EL ISR PARA QUE CARGUE DESDE RAM Y NO DESDE FLASH
    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);

    // CONFIGURACION ISR <<INTERRUP SERVICE ROUTINE>>
    for (int i = 0; i < sizeof(pinesEntradaLED) / sizeof(gpio_num_t); i++)
    {
        // CONFIGURACION DEL TIPO DE EVENTO PARA TRIGGER DEL BOTON
        // EVENTO AL DEJAR DE PRESIONAR EL BOTON (GPIO_INTR_NEGEDGE)
        // EVENTO AL PRESIONAR EL BOTON (GPIO_INTR_POSEDGE)
        gpio_set_intr_type(pinesEntradaLED[i], GPIO_INTR_POSEDGE);

        // SE CONFIGURA EL HANDLER PARA LA TAREA
        TaskHandle_t data = NULL;
        arrHandlersReboteBtn[i].task = data;

        // SE AGREGA EL HANDLER PARA EL ISR
        gpio_isr_handler_add(pinesEntradaLED[i], isr_handler_boton, (void *)&arrHandlersReboteBtn[i]);
    }

    // SE CREA EL HANDLER DE TIPO SEMAFORO PARA MULTITASKING
    HandlerSemaphoreBtn = xSemaphoreCreateBinary();
    // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
    xSemaphoreGive(HandlerSemaphoreBtn);
}

void IRAM_ATTR isr_handler_boton(void *arg)
{
    structHandler *data = (structHandler *)arg;

    // SE REANUDA LA TAREA
    xTaskResumeFromISR(data->task);
}

void boton_presionado(void *arg)
{
    BaseType_t *xHigherPriorityTaskWoken = (BaseType_t *)pdFALSE;
    structLed *data = (structLed *)arg;
    // uint8_t arrPatronPines[] = ;

    while (1)
    {
        // UN DELAY PEQUEÑO PARA EVITAR CUALQUIER ERROR
        vTaskDelay(INTERRUPT_DELAY / portTICK_PERIOD_MS);
        // SE AUTOSUSPENDE LA TAREA PARA REANUDAR DESPUÉS EN ESTE MISMO PUNTO
        vTaskSuspend(NULL);

        // SE APROPIA DEL SEMAFORO PARA AVISAR QUE UTILIZARA LA VARIABLE DE LED
        xSemaphoreTakeFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);

        // SI NO ESTAS EN JUEGO
        if (!juegoActivo)
        {
            if (data->pinSalida == arrLEDs[0].pinSalida)
            {
                empezar_juego();

                juegoActivo = !juegoActivo;
            }
        }
        // SI SI ESTAS EN JUEGO
        else
        {
            // VERIFICAR PATRON Y JALADA Y MEDIA
        }

        /*
        // CAMBIA EL ESTADO DEL LED
        data->pinEstado = !data->pinEstado;
        // SE ACTUALIZA EL ESTADO DEL LED
        gpio_set_level(data->pinSalida, data->pinEstado);

        vTaskDelay(INTERRUPT_DELAY / portTICK_PERIOD_MS);

        // CAMBIA EL ESTADO DEL LED
        data->pinEstado = !data->pinEstado;
        // SE ACTUALIZA EL ESTADO DEL LED
        gpio_set_level(data->pinSalida, data->pinEstado);
        */
        printf("Button %i pressed!\n", data->pinSalida);

        // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
        xSemaphoreGiveFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);
    }
}

void empezar_juego()
{
    // ENCENDER LEDS EN PATRON
    for (int i = 0; i < sizeof(arrLEDs) / sizeof(structLed); i++)
    {
        arrLEDs[i].pinEstado = true;
        gpio_set_level(arrLEDs[i].pinSalida, arrLEDs[i].pinEstado);
        vTaskDelay(DELAY_GAME_INIT / portTICK_RATE_MS);

        arrLEDs[i].pinEstado = false;
        gpio_set_level(arrLEDs[i].pinSalida, arrLEDs[i].pinEstado);
        vTaskDelay(DELAY_GAME_INIT / portTICK_RATE_MS);
    }
}

void LCD_DemoTask(void *param)
{
    LCD_I2C lcd;

    lcd.init(0x27, 21, 22, 2, 16);

    while (1)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hi there!"); // printing a string
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        lcd.clear();

        lcd.println("if you read this"); // using println to print in next line
        lcd.print("Everything works!");
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        lcd.clear();

        for (int i = 10; i > 0; i--)
        {
            lcd.setCursor(5, 0);
            lcd.print(':'); // printing chars
            lcd.println(')');

            lcd.print("Restarting in:");
            lcd.print(i); // printing int
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            lcd.clear();
        }
    }
}