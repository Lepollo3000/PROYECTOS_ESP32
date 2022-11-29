#define COMUN_MILLARES GPIO_NUM_18
#define COMUN_CENTENAS GPIO_NUM_5
#define COMUN_DECENAS GPIO_NUM_17
#define COMUN_UNIDADES GPIO_NUM_16

#define SEGMENTO_A GPIO_NUM_13
#define SEGMENTO_B GPIO_NUM_12
#define SEGMENTO_C GPIO_NUM_14
#define SEGMENTO_D GPIO_NUM_27
#define SEGMENTO_E GPIO_NUM_26
#define SEGMENTO_F GPIO_NUM_25
#define SEGMENTO_G GPIO_NUM_33

void show_letter_display(char letra);

void show_unidades(void);
void show_decenas(void);
void show_centenas(void);
void show_millares(void);

void alta_task(void *arg);
void bien_task(void *arg);
void baja_task(void *arg);