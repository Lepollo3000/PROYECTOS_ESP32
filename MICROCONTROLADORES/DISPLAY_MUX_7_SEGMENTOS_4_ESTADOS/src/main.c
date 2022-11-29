#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"

#include "gpio_setup.h"
#include "tasks_setup.h"
#include "display_7_segmentos_mux.h"

uint8_t nivel = 0;

void app_main()
{
    int lastButtonState = 0;
    unsigned long lastDebounceTime = 0;

    int buttonState = 0;

    app_main_setup();
    init_tasks();

    while (true)
    {
        int reading = gpio_get_level(BTN_1);

        // SI EL SWITCH CAMBIA
        if (reading != lastButtonState)
        {
            // "RESETEA" EL TIMER
            lastDebounceTime = esp_timer_get_time() / 1000ULL;
        }

        if (((esp_timer_get_time() / 1000ULL) - lastDebounceTime) > DELAY_BUTTON)
        {
            // SI EL ESTADO DEL BOTON CAMBIO
            if (reading != buttonState)
            {
                buttonState = reading;

                // SI EL ESTADO DEL BOTON ES 1
                if (buttonState == 1)
                {
                    nivel++;
                    printf("%i\n", nivel);

                    if (nivel >= 4)
                    {
                        nivel = 0;
                    }
                }
            }
        }

        switch (nivel)
        {
        case 1:
            vTaskResume(maquina_estado_2_task_handler);
            break;
        case 2:
            vTaskResume(maquina_estado_3_task_handler);
            break;
        case 3:
            vTaskResume(maquina_estado_4_task_handler);
            break;
        case 0:
        default:
            vTaskResume(maquina_estado_1_task_handler);
            break;
        }

        lastButtonState = reading;
    }
}