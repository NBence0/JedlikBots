#pragma once
#include <Arduino.h>
#include <hardware_pins.h>
#include <Adafruit_BNO08x.h>

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Rotacio {
    float x, y, z, w;
};

struct EulerRotacio {
    float roll, pitch, yaw;
};

class BNO085 {

public:
    // Nincs szükség már semmilyen lábra paraméterként
    BNO085();
    bool begin();

    void enableSensors();
    void update();
    bool hasNewData();

    Rotacio getRotation();
    EulerRotacio getEulerAngle();
    
    Vector3 getLinearAcceleration();
    Vector3 getGyroscope();
    Vector3 getGravity();
    Vector3 getMagnetometer();

    float getRoll();
    float getPitch();
    float getYaw();

    void resetYaw();
    float getContinuousYaw(); 

private:
    Adafruit_BNO08x bno;

    sh2_SensorValue_t _sensorValue;
    
    Rotacio _quatData;
    Vector3 _linAccelData;
    Vector3 _gyroData;
    Vector3 _gravityData;
    Vector3 _magData;
    float _prevRawYaw = 0.0;
    float _accumulatedYaw = 0.0;
    float _yawOffset = 0.0;
    void updateContinuousYaw(float currentRawYaw);
    
    bool _newDataAvailable;
};