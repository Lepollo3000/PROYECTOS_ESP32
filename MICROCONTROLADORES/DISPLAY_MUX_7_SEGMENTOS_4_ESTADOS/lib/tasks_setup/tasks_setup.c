#include "tasks_setup.h"

void maquina_estado_1_task(void *args)
{
    // contador automático ascendente de 0 a 159
    unsigned long currentTime = 0;
    unsigned long startedTime = 0;

    while (1)
    {
        vTaskSuspend(NULL);

        currentTime = esp_timer_get_time() / 1000ULL;

        show_mux_number_display(contador);

        if ((currentTime - startedTime) >= DELAY_TIME)
        {
            // printf("%i\n", contador);
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
    int lastButtonState = 0;
    unsigned long lastDebounceTime = 0;

    int buttonState = 0;

    while (1)
    {
        vTaskSuspend(NULL);
        int reading = gpio_get_level(BTN_2);

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
                    contador++;

                    if (contador >= 160)
                    {
                        contador = 0;
                    }
                }
            }
        }

        show_mux_number_display(contador);

        lastButtonState = reading;
    }
}

void maquina_estado_3_task(void *args)
{
    // contador 159 a 0 deberá de descender de forma manual desde un botón "descendente"
    int lastButtonState = 0;
    unsigned long lastDebounceTime = 0;

    int buttonState = 0;

    while (1)
    {
        vTaskSuspend(NULL);
        int reading = gpio_get_level(BTN_2);

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
                    contador--;

                    if (contador <= 0)
                    {
                        contador = 159;
                    }
                }
            }
        }

        show_mux_number_display(contador);

        lastButtonState = reading;
    }
}

void maquina_estado_4_task(void *args)
{
    // mostrar la lectura de un potenciómetro ADC (0V → 00, 5V → 999)
    int read_raw;
    unsigned long currentTime = 0;
    unsigned long startedTime = 0;

    while (true)
    {
        vTaskSuspend(NULL);

        currentTime = esp_timer_get_time() / 1000ULL;

        if ((currentTime - startedTime) >= DELAY_TIME)
        {
            adc2_get_raw(POTENCIOMETRO, ADC_WIDTH_12Bit, &read_raw);

            printf("%i\n", read_raw * 999 / 4095);
            startedTime = currentTime;
        }

        show_mux_number_display(read_raw * 999 / 4095);
    }
}

void init_tasks()
{
    xTaskCreate(maquina_estado_1_task, "Modo 1", 4056, NULL, 5, &maquina_estado_1_task_handler);
    xTaskCreate(maquina_estado_2_task, "Modo 2", 4056, NULL, 5, &maquina_estado_2_task_handler);
    xTaskCreate(maquina_estado_3_task, "Modo 3", 4056, NULL, 5, &maquina_estado_3_task_handler);
    xTaskCreate(maquina_estado_4_task, "Modo 4", 4056, NULL, 5, &maquina_estado_4_task_handler);
}