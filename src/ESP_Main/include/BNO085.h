#pragma once
#include <Arduino.h>
#include <hardware_pins.h>
#include <Adafruit_BNO08x.h>

struct Rotacio {
    float x, y, z, w;
};

struct EulerRotacio {
    float roll, pitch, yaw;
};

class BNO085 {

public:
    BNO085(uint8_t bno_int, uint8_t cs, uint8_t rst);
    bool begin(SPIClass *spi);

    void enableRotation();
    void update();
    Rotacio getRotation();
    bool hasNewData();
    EulerRotacio getEulerAngle();
    float getRoll(); // x
    float getPitch(); // y
    float getYaw(); // z



private:
    Adafruit_BNO08x bno;
    uint8_t _int, _cs, _rst;

    sh2_SensorValue_t _sensorValue;
    Rotacio _myData;
    bool _newDataAvailable;
};