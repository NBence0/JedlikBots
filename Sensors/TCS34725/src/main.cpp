#include "TCS34725.h"
#include <Wire.h>
#include <Arduino.h>

const int LED_RED = 13;
const int LED_GREEN = 12;
const int LED_BLUE = 11;

TCS34725 tcs;

void setup(void)
{
    Serial.begin(115200);

    Wire.begin(36, 37);  // SDA = GPIO21, SCL = GPIO22 az ESP32-S3-n
    if (!tcs.attach(Wire))
        Serial.println("ERROR: TCS34725 NOT FOUND !!!");

    tcs.integrationTime(33);
    tcs.gain(TCS34725::Gain::X01);


}

void led(int r, int g, int b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

void loop(void)

{
    if (tcs.available())
    {
        TCS34725::Color color = tcs.color();
        float lux = tcs.lux();
        Serial.print("Color Temp : "); Serial.println(tcs.colorTemperature());
        Serial.print("Lux        : "); Serial.println(lux);
        Serial.print("R          : "); Serial.println(color.r);
        Serial.print("G          : "); Serial.println(color.g);
        Serial.print("B          : "); Serial.println(color.b);

        if (lux < 50) {
          Serial.println("The object is too far away");
          led(0, 0, 0);
        } else {
          led(color.r, color.g, color.b);
        }
        delay(500);
    }
}