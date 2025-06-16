#include "ColorSensor.h"
#include "TCS34725.h"
#include "Arduino.h"
#include <Wire.h>

static TCS34725 tcs;

void ColorSensor::begin() {
    Wire.begin(36, 37);  // ESP32 S3 I2C pinjei
    if (!tcs.attach(Wire)) {
        Serial.println("ERROR: TCS34725 NOT FOUND !!!");
    }

    tcs.integrationTime(33);
    tcs.gain(TCS34725::Gain::X01);
}

RGB ColorSensor::readColor() {
    RGB result = {0, 0, 0};
    if (tcs.available()) {
        TCS34725::Color color = tcs.color();
        result.r = color.r;
        result.g = color.g;
        result.b = color.b;
    }
    return result;
}
