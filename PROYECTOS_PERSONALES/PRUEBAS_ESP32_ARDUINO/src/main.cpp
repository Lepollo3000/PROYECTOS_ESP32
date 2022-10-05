#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*OneWire ourWire(13); // Se establece el pin 2  como bus OneWire
DallasTemperature sensors(&ourWire); // Se declara una variable u objeto para nuestro sensor*/

int temt6000Pin = 13; // Analog pin for reading sensor data
float light;
int light_value;

void setup()
{
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(9600);
  /*sensors.begin(); // Se inicia el sensor*/

  pinMode(temt6000Pin, INPUT); // data pin for ambientlight sensor
  // pinMode(13, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  /*sensors.requestTemperatures();           // Se envía el comando para leer la temperatura
  float temp = sensors.getTempCByIndex(0); // Se obtiene la temperatura en ºC*/

  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976; // percentage calculation
  Serial.println(light);
  delay(100);
  /*if (light_value > 1000) {
    digitalWrite(13, HIGH);
  }
  else {digitalWrite(13,LOW);}*/

  /*Serial.print("Temperatura= ");
  Serial.print(temp);
  Serial.println(" C");
  delay(100);*/
}