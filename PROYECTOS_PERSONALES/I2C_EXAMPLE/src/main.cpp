#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "HD44780.h"

#define LCD_ADDR 0x27
#define SDA_PIN 3
#define SCL_PIN 1
#define LCD_COLS 16
#define LCD_ROWS 2

#ifdef __cplusplus

extern "C"
{
#endif
    void app_main(void);
#ifdef __cplusplus
}
#endif

void app_main()
{
    LCD_I2C lcd0;
    //LCD_I2C lcd1;

    lcd0.init(0x27, 3, 1, 2, 16);
    //lcd1.init(0x27, 3, 1, 2, 16);

    // lcd.home();
    lcd0.clear();
    //lcd1.clear();

    while (1)
    {
        lcd0.clear();
        lcd0.setCursor(0, 0);
        lcd0.print("Hi there!"); // printing a string
        //lcd1.setCursor(0, 0);
        //lcd1.print("Hi there!"); // printing a string
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        lcd0.clear();
        //lcd1.clear();

        lcd0.println("if you read this"); // using println to print in next line
        lcd0.print("Everything works!");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        lcd0.clear();

        for (int i = 10; i > 0; i--)
        {
            lcd0.setCursor(5, 0);
            lcd0.print(':'); // printing chars
            lcd0.println(')');

            lcd0.print("Restarting in:");
            lcd0.print(i); // printing int
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            lcd0.clear();
        }
    }
}
/*
void LCD_DemoTask(void *param)
{
    char num[20];

    while (1) {
        LCD_home();
        LCD_clearScreen();

        LCD_writeStr("16x2 I2C LCD");
        printf("16x2 I2C LCD\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
        LCD_clearScreen();

        LCD_writeStr("Lets Count 0-10!");
        printf("Lets Count 0-10!\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
        LCD_clearScreen();

        for (int i = 0; i <= 10; i++) {
            LCD_setCursor(8, 1);
            sprintf(num, "%d", i);
            printf("%d\n", i);
            LCD_writeStr(num);
            vTaskDelay(500 / portTICK_RATE_MS);
        }

    }
}*/