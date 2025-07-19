#include "Gyro.h"
#include <math.h>

// Fix lábkiosztás
#define BNO08X_CS    20
#define BNO08X_INT    21
#define BNO08X_RESET  48

#define SPI_SCK     15
#define SPI_MISO    16
#define SPI_MOSI    14

bool BNO08xGyro::begin() {
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, BNO08X_CS);

    if (!_bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        _connected = false;
        return false;
    }

    setReports();
    _connected = true;
    return true;
}

void BNO08xGyro::setReports() {
    _bno08x.enableReport(SH2_GAME_ROTATION_VECTOR);
}

bool BNO08xGyro::update() {
    if (!_connected) return false;

    if (_bno08x.wasReset()) {
        setReports();
    }

    return _bno08x.getSensorEvent(&_sensorValue);
}

void BNO08xGyro::getQuaternion(float &r, float &i, float &j, float &k) {
    r = _sensorValue.un.gameRotationVector.real;
    i = _sensorValue.un.gameRotationVector.i;
    j = _sensorValue.un.gameRotationVector.j;
    k = _sensorValue.un.gameRotationVector.k;
}

void BNO08xGyro::getEuler(float &roll, float &pitch, float &yaw) {
    float r = _sensorValue.un.gameRotationVector.real;
    float i = _sensorValue.un.gameRotationVector.i;
    float j = _sensorValue.un.gameRotationVector.j;
    float k = _sensorValue.un.gameRotationVector.k;

    roll = atan2(2.0f * (r * i + j * k), 1.0f - 2.0f * (i * i + j * j));
    pitch = asin(2.0f * (r * j - k * i));
    yaw = atan2(2.0f * (r * k + i * j), 1.0f - 2.0f * (j * j + k * k));

    roll *= 180.0f / PI;
    pitch *= 180.0f / PI;
    yaw *= 180.0f / PI;
}

bool BNO08xGyro::isConnected() {
    return _connected;
}
