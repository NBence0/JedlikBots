#pragma once
#include <Arduino.h>
#include <BME280_t.h> // Ez a header fájl név helyes
#include <SPI.h>

class BMESensor {
public:
    BMESensor() = default;

    bool begin();
    bool read();
    float getTemperature();
    float getHumidity();
    float getPressure();
    bool isConnected();

private:
    BME280<> _bme; 
    bool _connected = false;
    float _temperature, _humidity, _pressure;
};