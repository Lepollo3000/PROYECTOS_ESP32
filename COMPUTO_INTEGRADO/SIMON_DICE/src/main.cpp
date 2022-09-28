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
    int numLed;
    gpio_num_t pin;
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
        GPIO_NUM_22, GPIO_NUM_13, GPIO_NUM_4, GPIO_NUM_27, GPIO_NUM_17, GPIO_NUM_25, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21 \
    }

// DEFINICION PINES LCD
#define LCD_1_ADDR 0x27
#define SDA_PIN 3
#define SCL_PIN 1
#define LCD_COLS 16
#define LCD_ROWS 2

// DELAY PARA INTERRUPCION DE REBOTE
#define INTERRUPT_DELAY 185

// CONFIGURACIONES DE JUEGO
#define PARTIDAS_MAXIMAS 100
#define DELAY_BETWEEN_CLICKS 5000
#define DELAY_GAME_INIT 80
#define DELAY_LED_FLASH 125

// DELAY NIVELES DE DIFICULTAD
#define DELAY_DIFICULTAD_1 500
#define DELAY_DEFICULTAD_2 250
#define DELAY_DIFICULTAD_3 125

// ------------------ DECLARACION VARIABLES GLOBALES ------------------

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE ENTRADA PARA LOS LEDS
const gpio_num_t pinesEntradaLED[] = LED_INPUT_PINES;

// ARREGLO DE ENTEROS QUE REPRESENTA LOS PINES DE SALIDA PARA LOS LEDS
const gpio_num_t pinesSalidaLED[] = LED_OUTPUT_PINES;

// ARREGLO DE DIFICULTADES
const int arrDificultades[] = {DELAY_DIFICULTAD_1, DELAY_DEFICULTAD_2, DELAY_DIFICULTAD_3};

// NIVEL DE DIFICULTAD ACTUAL
uint8_t dificultadActualPartida;

// SABER SI SE ESTÁ JUGANDO ACTUALMENTE
bool juegoActivo;

// SABER SI EL JUGADOR PERDIO
bool juegoPerdido;

// SABER SI EL JUGADOR GANO
bool juegoGanado;

int8_t nivelActualPartida;
int8_t intentoActualPartida;

// ARREGLO DE PARTIDAS_MAXIMAS NUMEROS PARA EL PATRON DE JUEGO
uint8_t patronNumeros[PARTIDAS_MAXIMAS];

// SE DECLARA UN ARREGLO DE LEDS DEL TAMAÑO DE CANTIDAD DE PINES
structLed arrLEDs[sizeof(pinesSalidaLED) / sizeof(gpio_num_t)];

// HANDLER TAREA DE REBOTE
structHandler arrHandlersReboteBtn[sizeof(pinesEntradaLED) / sizeof(gpio_num_t)];

TaskHandle_t esperaDeBotonHandler = NULL;
TaskHandle_t lcdHandler = NULL;

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
    void boton_presionado_task(void *arg);
    void esperar_boton_task(void *arg);

    void aumentar_dificultad();

    void empezar_juego();
    void iniciar_siguiente_nivel();
    void iniciar_siguiente_turno();
    void terminar_turno();
    bool revisar_si_nivel_completado();
    void terminar_juego(bool ganador);

    void mostrar_patron();
    void actualizar_patron();
    bool revisar_patron(structLed *led);

    void mostrar_led(structLed *arg);
    void flash_perdedor(uint8_t cantFlashes);
    void flash_ganador(uint8_t cantFlashes);

    void lcd_task(void *param);
    void mostrar_pantalla_inicial(LCD_I2C pantalla);
    void mostrar_puntos_de_juego_lcd(LCD_I2C pantalla);
    void mostrar_puntos_finales_lcd(LCD_I2C pantalla);
    void mostrar_mensaje_ganador(LCD_I2C pantalla);
    void mostrar_mensaje_perdedor(LCD_I2C pantalla);
    void try_resumir_lcd();
#ifdef __cplusplus
}
#endif

void app_main(void)
{
    // CONFIGURACION GENERAL DE PINES Y SERVICIOS
    app_main_setup();

    xTaskCreate(&lcd_task, "Task_Rebote_", 4056, NULL, 5, &lcdHandler);

    // SE CREAN LAS TAREAS PARA REBOTE DE BOTON
    for (int i = 0; i < sizeof(pinesEntradaLED) / sizeof(gpio_num_t); i++)
    {
        xTaskCreate(boton_presionado_task, "Task_Rebote_" + i, 4056, (void *)&arrLEDs[i], 1, (TaskHandle_t *)&arrHandlersReboteBtn[i]);
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

        structLed data = {.numLed = i, .pin = pinesSalidaLED[i], .pinEstado = false};
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

    printf("Dificultad actual %d\n", dificultadActualPartida + 1);

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

void boton_presionado_task(void *arg)
{
    BaseType_t *xHigherPriorityTaskWoken = (BaseType_t *)pdFALSE;
    structLed *led = (structLed *)arg;

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
            // SI SE PERDIO EN LA ANTERIOR PARTIDA
            if (juegoPerdido && !juegoGanado)
            {
                // SE REINICIAN LAS VARIABLES PRINCIPALES
                juegoPerdido = false;
                try_resumir_lcd();
            }
            // SI SE GANO EN LA ANTERIOR PARTIDA
            if (!juegoPerdido && juegoGanado)
            {
                // SE REINICIAN LAS VARIABLES PRINCIPALES
                juegoGanado = false;
                try_resumir_lcd();
            }
            // SI SE HACE CLIC EN EL PRIMER BOTON
            else if (led->pin == arrLEDs[0].pin)
            {
                empezar_juego();
            }
            // SI SE HACE CLIC EN EL TERCER BOTON
            else if (led->pin == arrLEDs[2].pin)
            {
                aumentar_dificultad();
            }
        }
        // SI SI ESTAS EN JUEGO
        else
        {
            printf("6. Respuesta recibida | boton %d\n", led->numLed);
            terminar_turno();

            if (intentoActualPartida <= nivelActualPartida)
            {
                mostrar_led(led);

                // VERIFICAR PATRON Y JALADA Y MEDIA
                if (!revisar_patron(led))
                {
                    terminar_juego(false);
                }
                else if (revisar_si_nivel_completado())
                {
                    iniciar_siguiente_nivel();
                }
                else
                {
                    intentoActualPartida++;
                    iniciar_siguiente_turno();
                }
            }
        }

        // SE CEDE EL USO DEL HANDLER DE TIPO SEMAFORO
        xSemaphoreGiveFromISR(HandlerSemaphoreBtn, xHigherPriorityTaskWoken);
    }
}

void esperar_boton_task(void *arg)
{
    printf("5. Esperando respuesta\n");
    vTaskDelay(DELAY_BETWEEN_CLICKS / portTICK_PERIOD_MS);

    printf("Tiempo transcurrido, perdiste\n\n");
    terminar_juego(false);

    printf("Borrando tarea\n\n");
    vTaskDelete(NULL);
}

void aumentar_dificultad()
{
    dificultadActualPartida++;

    if (dificultadActualPartida >= sizeof(arrDificultades) / sizeof(int))
        dificultadActualPartida = 0;

    printf("Dificultad actual %d\n", dificultadActualPartida + 1);
    try_resumir_lcd();
}

void empezar_juego()
{
    printf("\n\n********** Empezando juego **********\n\n");
    nivelActualPartida = 0;
    intentoActualPartida = 0;
    juegoActivo = true;
    juegoGanado = false;
    juegoPerdido = false;

    printf("1. Iniciando nivel %d\n", nivelActualPartida);

    // ENCENDER LEDS EN PATRON
    for (int i = 0; i < sizeof(arrLEDs) / sizeof(structLed); i++)
    {
        arrLEDs[i].pinEstado = true;
        gpio_set_level(arrLEDs[i].pin, arrLEDs[i].pinEstado);
        vTaskDelay(DELAY_GAME_INIT / portTICK_PERIOD_MS);

        arrLEDs[i].pinEstado = false;
        gpio_set_level(arrLEDs[i].pin, arrLEDs[i].pinEstado);
        vTaskDelay(DELAY_GAME_INIT / portTICK_PERIOD_MS);
    }

    // TE DA EL PRIMER NUMERO RANDOM
    actualizar_patron();
    mostrar_patron();

    iniciar_siguiente_turno();
}

void iniciar_siguiente_nivel()
{
    // REINICIAR INTENTOS Y NIVEL
    intentoActualPartida = 0;
    nivelActualPartida++;

    if (nivelActualPartida >= PARTIDAS_MAXIMAS)
    {
        terminar_juego(true);
    }
    else
    {
        printf("Iniciando nivel %d\n", nivelActualPartida);

        // ACTUALIZAR Y MOSTRAR PATRON
        actualizar_patron();
        mostrar_patron();

        iniciar_siguiente_turno();
    }
}

void terminar_turno()
{
    if (esperaDeBotonHandler != NULL)
    {
        // SE ELIMINA LA TAREA DE ESPERAR BOTON
        printf("7. Terminando turno %d\n", intentoActualPartida);
        vTaskDelete(esperaDeBotonHandler);
        esperaDeBotonHandler = NULL;
    }
    else
    {
        printf("Boton rebelde >:C\n");
    }
}

void iniciar_siguiente_turno()
{
    // SE INICIA LA TAREA DE ESPERAR BOTON
    printf("4. Iniciando turno %d\n", intentoActualPartida);

    xTaskCreate(esperar_boton_task, "Task_Esperar_Boton", 4056, NULL, 1, &esperaDeBotonHandler);
    try_resumir_lcd();
}

void terminar_juego(bool ganador)
{
    printf("Terminando juego\n");

    if (ganador)
    {
        juegoPerdido = false;
        juegoGanado = true;
        flash_ganador(2);
    }
    else
    {
        juegoPerdido = true;
        juegoGanado = false;
        flash_perdedor(3);
    }

    juegoActivo = false;

    try_resumir_lcd();
}

void flash_perdedor(uint8_t cantFlashes)
{
    for (int i = 0; i < cantFlashes; i++)
    {
        for (int j = 0; j < sizeof(arrLEDs) / sizeof(structLed); j++)
        {
            arrLEDs[j].pinEstado = true;
            gpio_set_level(arrLEDs[j].pin, arrLEDs[j].pinEstado);
        }
        vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);

        for (int j = 0; j < sizeof(arrLEDs) / sizeof(structLed); j++)
        {
            arrLEDs[j].pinEstado = false;
            gpio_set_level(arrLEDs[j].pin, arrLEDs[j].pinEstado);
        }
        vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);
    }
}

void flash_ganador(uint8_t cantFlashes)
{
    int nuevosLeds[] = {0, 1, 2, 5, 8, 7, 6, 3};
    int sizeArr = sizeof(nuevosLeds) / sizeof(int);

    for (int i = 0; i < cantFlashes; i++)
    {
        for (int j = 0; j < sizeArr; j++)
        {
            int numLed = nuevosLeds[j];
            structLed led = arrLEDs[numLed];

            led.pinEstado = true;
            gpio_set_level(led.pin, led.pinEstado);
            vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);
        }

        for (int j = 0; j < sizeArr; j++)
        {
            int numLed = nuevosLeds[j];
            structLed led = arrLEDs[numLed];

            led.pinEstado = false;
            gpio_set_level(led.pin, led.pinEstado);
            vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);
        }
    }
}

void mostrar_patron()
{
    printf("3. Mostrando patron\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    for (int i = 0; i <= nivelActualPartida; i++)
    {
        uint8_t numLed = patronNumeros[i];
        structLed led = arrLEDs[numLed];

        led.pinEstado = true;
        gpio_set_level(led.pin, led.pinEstado);
        vTaskDelay(arrDificultades[dificultadActualPartida] / portTICK_PERIOD_MS);

        led.pinEstado = false;
        gpio_set_level(led.pin, led.pinEstado);
        vTaskDelay(arrDificultades[dificultadActualPartida] / portTICK_PERIOD_MS);

        printf("\tled %i,", led.numLed);
    }
    printf("\n");
}

void actualizar_patron()
{
    printf("2. Actualizando patron\n");
    uint8_t numero = esp_random() % 9;
    printf("\tRandom = %d\n", numero);
    patronNumeros[nivelActualPartida] = numero;
}

bool revisar_patron(structLed *led)
{
    printf("9. Revisando boton %i, esperando %i\n", led->numLed, patronNumeros[intentoActualPartida]);

    // SI SI, MUMEN
    if (patronNumeros[intentoActualPartida] == led->numLed)
    {
        printf("\tRespuesta correcta!\n\n");
        return true;
    }

    // SI NO, MUMAL
    printf("\tRespuesta incorrecta >:C\n\n");
    return false;
}

void mostrar_led(structLed *arg)
{
    structLed *led = (structLed *)arg;

    printf("8. Mostrando led %d\n", led->numLed);

    led->pinEstado = true;
    gpio_set_level(led->pin, led->pinEstado);
    vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);

    led->pinEstado = false;
    gpio_set_level(led->pin, led->pinEstado);
    vTaskDelay(DELAY_LED_FLASH / portTICK_PERIOD_MS);
}

bool revisar_si_nivel_completado()
{
    if (intentoActualPartida == nivelActualPartida)
    {
        printf("Nivel %d completado\n\n", nivelActualPartida);
        return true;
    }

    return false;
}

void lcd_task(void *param)
{
    LCD_I2C pantalla;

    // INICIALIZAR LCD
    pantalla.init(LCD_1_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);

    pantalla.clear();
    pantalla.setCursor(0, 0);

    while (1)
    {
        pantalla.clear();
        pantalla.setCursor(0, 0);

        // SI AUN NO SE ESTA EN JUEGO
        if (!juegoActivo)
        {
            if (juegoGanado && !juegoPerdido)
            {
                mostrar_mensaje_ganador(pantalla);
            }
            else if (juegoPerdido && !juegoGanado)
            {
                mostrar_mensaje_perdedor(pantalla);
            }
            else
            {
                mostrar_pantalla_inicial(pantalla);
            }

            // SE QUEDA LA PANTALLA AQUI
            vTaskSuspend(NULL);
        }
        // SI YA SE ESTA EN JUEGO
        else
        {
            if (intentoActualPartida <= nivelActualPartida)
            {
                mostrar_puntos_de_juego_lcd(pantalla);
            }

            // SE QUEDA LA PANTALLA AQUI
            vTaskSuspend(NULL);
        }
    }
}

void mostrar_pantalla_inicial(LCD_I2C pantalla)
{
    // CREAR MENSAJE DESDE AFUERA PORQUE LCD.PRINT NO PERMITE EL FORMATEO DE STRING
    char *mensaje;

    asprintf(&mensaje, "Nivel");
    pantalla.println(mensaje);

    asprintf(&mensaje, "seleccionado = %d", dificultadActualPartida + 1);
    pantalla.println(mensaje);
}

void mostrar_puntos_de_juego_lcd(LCD_I2C pantalla)
{
    // CREAR MENSAJE DESDE AFUERA PORQUE LCD.PRINT NO PERMITE EL FORMATEO DE STRING
    char *mensaje;

    asprintf(&mensaje, "Puntaje = %d", nivelActualPartida);
    pantalla.println(mensaje);

    asprintf(&mensaje, "Nivel = %d", dificultadActualPartida + 1);
    pantalla.println(mensaje);
}

void mostrar_puntos_finales_lcd(LCD_I2C pantalla)
{
    // EMOTICON DE CALAVERA
    // uint8_t Skull[8] = {0b00000, 0b01110, 0b10101, 0b11011, 0b01110, 0b01110, 0b00000, 0b00000};

    // CREAR MENSAJE DESDE AFUERA PORQUE LCD.PRINT NO PERMITE EL FORMATEO DE STRING
    char *mensaje;

    pantalla.clear();
    pantalla.setCursor(0, 0);

    asprintf(&mensaje, "Puntaje = %d", nivelActualPartida);
    pantalla.println(mensaje);

    asprintf(&mensaje, "Nivel = %d", dificultadActualPartida + 1);
    pantalla.println(mensaje);

    /*// SE IMPRIME LA CALAVERA
    pantalla.setCursor(1, 11);
    for (int i = 0; i < 8; i++)
    {
        Skull[i] = i * i;
        asprintf(&mensaje, "%02X", Skull);
        //printf("x[%d]: %02X\n", i, Skull[i]);
    }
    pantalla.print(mensaje);*/
}

void mostrar_mensaje_ganador(LCD_I2C pantalla)
{
    // CREAR MENSAJE DESDE AFUERA PORQUE LCD.PRINT NO PERMITE EL FORMATEO DE STRING
    char *mensaje;

    asprintf(&mensaje, "Lograste completar");
    pantalla.println(mensaje);

    asprintf(&mensaje, "el juego!");
    pantalla.println(mensaje);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    mostrar_puntos_finales_lcd(pantalla);

    pantalla.setCursor(14, 1);
    pantalla.print("C:");
}

void mostrar_mensaje_perdedor(LCD_I2C pantalla)
{
    // CREAR MENSAJE DESDE AFUERA PORQUE LCD.PRINT NO PERMITE EL FORMATEO DE STRING
    char *mensaje;

    asprintf(&mensaje, "Mas suerte para");
    pantalla.println(mensaje);

    asprintf(&mensaje, "La proxima");
    pantalla.println(mensaje);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    mostrar_puntos_finales_lcd(pantalla);

    pantalla.setCursor(14, 1);
    pantalla.print(":C");
}

void try_resumir_lcd()
{
    // INICIAR LCD DE NUEVO SI AUN NO LO HACE
    if (eTaskGetState(lcdHandler) == eSuspended)
    {
        vTaskResume(lcdHandler);
    }
}