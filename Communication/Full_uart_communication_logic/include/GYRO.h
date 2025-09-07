#pragma once
#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <SPI.h>

class BNO08xGyro {
public:
    BNO08xGyro() = default;

    bool begin();     // csak akkor hívd, ha a szenzor fizikailag aktív
    bool update();    // új adat van-e
    void getQuaternion(float &r, float &i, float &j, float &k);
    void getEuler(float &roll, float &pitch, float &yaw);
    bool isConnected();  // true, ha működik a szenzor (pl. begin sikeres volt)

private:
    void setReports();

    Adafruit_BNO08x _bno08x = Adafruit_BNO08x(6);  // RESET = 6
    sh2_SensorValue_t _sensorValue;
    bool _connected = false;
};
