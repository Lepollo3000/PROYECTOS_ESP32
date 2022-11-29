#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <driver/adc.h>
#include "esp_system.h"
#include "tasksSetup.h"
#include "gpioSetup.h"
#include "multi7Segmentos.h"

TaskHandle_t alta_task_handler = NULL;
TaskHandle_t bien_task_handler = NULL;
TaskHandle_t baja_task_handler = NULL;

void IRAM_ATTR isr(uint8_t nivel);

void app_main()
{
    int read_raw;

    app_main_setup();

    xTaskCreate(alta_task, "Task_ALTA", 4056, NULL, 5, &alta_task_handler);
    xTaskCreate(bien_task, "Task_BIEN", 4056, NULL, 5, &bien_task_handler);
    xTaskCreate(baja_task, "Task_BAJA", 4056, NULL, 5, &baja_task_handler);

    while (true)
    {
        adc2_get_raw(POTENCIOMETRO, ADC_WIDTH_BIT_DEFAULT, &read_raw);

        // 0 a 50
        if (read_raw >= 0 && read_raw <= 1365)
        {
            isr(0);
        }
        // 51 a 100
        else if (read_raw >= 1366 && read_raw <= 2730)
        {
            isr(1);
        }
        // 101 a 255
        else if (read_raw >= 2731 && read_raw <= 4095)
        {
            isr(2);
        }
    }
}

void IRAM_ATTR isr(uint8_t nivel)
{
    switch (nivel)
    {
    case 1:
        // SE REANUDA LA TAREA BIEN
        xTaskResumeFromISR(bien_task_handler);
        break;
    case 2:
        // SE REANUDA LA TAREA ALTA
        xTaskResumeFromISR(alta_task_handler);
        break;
    case 0:
    default:
        // SE REANUDA LA TAREA BAJA
        xTaskResumeFromISR(baja_task_handler);
        break;
    }
}