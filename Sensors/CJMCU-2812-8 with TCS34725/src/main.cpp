#include <Arduino.h>
#include "ColorSensor.h"
#include "LED.h"

ColorSensor sensor;
LED led(38, 8);  // pin: 38, LED-ek száma: 8

void setup() {
    Serial.begin(115200);
    sensor.begin();
    led.begin();
}

void loop() {
    RGB color = sensor.readColor();

    Serial.print("R: "); Serial.println(color.r);
    Serial.print("G: "); Serial.println(color.g);
    Serial.print("B: "); Serial.println(color.b);

    led.colorplay(color.r, color.g, color.b);
    delay(500);
}
