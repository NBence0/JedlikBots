#pragma once
#include <Arduino.h>
#include <Adafruit_AS7341.h>

class ColorSensor {
public:
    ColorSensor() = default;

    bool begin(uint8_t ATIME = 100, uint16_t ASTEP = 999, as7341_gain_t gain = AS7341_GAIN_256X);
    void readOneChannel(uint8_t channel, uint16_t &value);
private:
    Adafruit_AS7341 _as7341 = Adafruit_AS7341();
    bool _connected = false;

};