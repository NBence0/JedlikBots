// BME_SENSOR.h fájlban

#pragma once
#include <Arduino.h>
#include <BME280_t.h>
#include <SPI.h>

class BMESensor {
public:
    // A default konstruktor helyett deklaráljuk a sajátunkat
    BMESensor();

    bool begin();
    bool read();
    float getTemperature();
    float getHumidity();
    float getPressure();
    bool isConnected();

private:
    // Ez a sor marad, a _bme objektumot majd a .cpp-ben inicializáljuk
    BME280<> _bme;
    bool _connected = false;
    float _temperature, _humidity, _pressure;
};