#include "display_7_segmentos_mux.h"

TaskHandle_t maquina_estado_1_task_handler = NULL;
TaskHandle_t maquina_estado_2_task_handler = NULL;
TaskHandle_t maquina_estado_3_task_handler = NULL;
TaskHandle_t maquina_estado_4_task_handler = NULL;

uint16_t contador = 0;

void maquina_estado_1_task(void *args);
void maquina_estado_2_task(void *args);
void maquina_estado_3_task(void *args);
void maquina_estado_4_task(void *args);

void init_tasks(void);