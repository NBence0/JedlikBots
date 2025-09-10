#pragma once
#include "GYRO.h"
#include "BME_SENSOR.h"
#include "HC154.h"

// Enum az eszközök egyértelmű azonosítására
enum DeviceType {
    DEVICE_GYRO,
    DEVICE_BME
};

class DeviceManager {
public:
    DeviceManager(HC154& mux, BNO08xGyro& gyro, BMESensor& bme);
    void beginAll();
    void selectDevice(DeviceType device);
    void deselectAll();

private:
    HC154& _mux;
    BNO08xGyro& _gyro;
    BMESensor& _bme;
};