#include "tasksSetup.h"

void alta_task(void *arg)
{
    while (true)
    {
        vTaskSuspend(NULL);

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('A');
        show_millares();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('L');
        show_centenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('T');
        show_decenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('A');
        show_unidades();
    }
}

void bien_task(void *arg)
{
    while (true)
    {
        vTaskSuspend(NULL);

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('B');
        show_millares();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('I');
        show_centenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('E');
        show_decenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('N');
        show_unidades();
    }
}

void baja_task(void *arg)
{
    while (true)
    {
        vTaskSuspend(NULL);

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('B');
        show_millares();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('A');
        show_centenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('J');
        show_decenas();

        vTaskDelay(DELAY_DISPLAY / portTICK_PERIOD_MS);
        show_letter_display('A');
        show_unidades();
    }
}