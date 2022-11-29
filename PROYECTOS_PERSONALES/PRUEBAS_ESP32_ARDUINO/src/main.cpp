#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire ourWire(27); // Se establece el pin 2  como bus OneWire
DallasTemperature sensors(&ourWire); // Se declara una variable u objeto para nuestro sensor

const byte temt6000Pin = 13; // Analog pin for reading sensor data
const byte pHpin = 14;       // Connect the sensor's Po output to analogue pin 0.
float calibration_value = -6;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;

float light;
int light_value;

void setup()
{
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(9600);
  sensors.begin(); // Se inicia el sensor

  pinMode(temt6000Pin, INPUT); // data pin for ambientlight sensor
  pinMode(pHpin, INPUT);       // data pin for ambientlight sensor
  // pinMode(13, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  //sensors.requestTemperatures();           // Se envía el comando para leer la temperatura
  float temp = 22;//sensors.getTempCByIndex(0); // Se obtiene la temperatura en ºC

  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(pHpin);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  float ph_act =  volt + calibration_value;

  Serial.print("Lectura Sonda: ");
  Serial.print(ph_act);
  Serial.println(" pH");

  int light_value = analogRead(temt6000Pin);
  light = light_value * 0.0976; // percentage calculation

  Serial.print("light: ");
  Serial.println(light);

  delay(500);

  /*if (light_value > 1000) {
    digitalWrite(13, HIGH);
  }
  else {digitalWrite(13,LOW);}*/

  Serial.print("Temperatura= ");
  Serial.print(22);
  Serial.println(" C\n");
  delay(100);
}
