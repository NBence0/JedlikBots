#include <BNO085.h>
#include <Arduino.h>

// A bno(-1) mondja meg a library-nek, hogy ne használjon RST pint
BNO085::BNO085() : bno(-1) {
    _linAccelData = {0, 0, 0};
    _gyroData = {0, 0, 0};
    _gravityData = {0, 0, 0};
    _magData = {0, 0, 0};
    _quatData = {0, 0, 0, 0};
    _newDataAvailable = false;
}

bool BNO085::begin() {
    // Csak I2C cím és a Wire busz
    if (!bno.begin_I2C(0x4B, &Wire)) {
        return false;
    }
    enableSensors();
    return true;
}

void BNO085::enableSensors() { 
    long reportIntervalUs = 5000;

    if (!bno.enableReport(SH2_GAME_ROTATION_VECTOR, reportIntervalUs)) {
        Serial.println("Warning: Rotation vector enable failed");
    }
}

void BNO085::update() {
    unsigned long totalStart = micros();

    if (bno.wasReset()) {
        enableSensors();
    }
    int biztonsagi_limit = 20;

    // Ha nincs INT pin, ez automatikusan lekérdezi I2C-n, hogy van-e új adat
    while (bno.getSensorEvent(&_sensorValue) && biztonsagi_limit > 0) {
    
        biztonsagi_limit--; 

        switch (_sensorValue.sensorId) {
            case SH2_GAME_ROTATION_VECTOR:
                _quatData.x = _sensorValue.un.rotationVector.i;
                _quatData.y = _sensorValue.un.rotationVector.j;
                _quatData.z = _sensorValue.un.rotationVector.k;
                _quatData.w = _sensorValue.un.rotationVector.real;
                _newDataAvailable = true;
                break;

            case SH2_LINEAR_ACCELERATION:
                _linAccelData.x = _sensorValue.un.linearAcceleration.x;
                _linAccelData.y = _sensorValue.un.linearAcceleration.y;
                _linAccelData.z = _sensorValue.un.linearAcceleration.z;
                _newDataAvailable = true;
                break;

            case SH2_GYROSCOPE_CALIBRATED:
                _gyroData.x = _sensorValue.un.gyroscope.x;
                _gyroData.y = _sensorValue.un.gyroscope.y;
                _gyroData.z = _sensorValue.un.gyroscope.z;
                _newDataAvailable = true;
                break;

            case SH2_GRAVITY:
                _gravityData.x = _sensorValue.un.gravity.x;
                _gravityData.y = _sensorValue.un.gravity.y;
                _gravityData.z = _sensorValue.un.gravity.z;
                _newDataAvailable = true;
                break;

            case SH2_MAGNETIC_FIELD_CALIBRATED:
                _magData.x = _sensorValue.un.magneticField.x;
                _magData.y = _sensorValue.un.magneticField.y;
                _magData.z = _sensorValue.un.magneticField.z;
                _newDataAvailable = true;
                break;
        }
    }
}

bool BNO085::hasNewData() {
    bool temp = _newDataAvailable;
    _newDataAvailable = false;
    return temp;
}

Rotacio BNO085::getRotation() { return _quatData; }
Vector3 BNO085::getLinearAcceleration() { return _linAccelData; }
Vector3 BNO085::getGyroscope() { return _gyroData; }
Vector3 BNO085::getGravity() { return _gravityData; }
Vector3 BNO085::getMagnetometer() { return _magData; }

EulerRotacio BNO085::getEulerAngle() {
    float w = _quatData.w;
    float x = _quatData.x;
    float y = _quatData.y;
    float z = _quatData.z;

    EulerRotacio angle;

    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angle.roll = atan2(sinr_cosp, cosr_cosp) * (180.0 / M_PI);

    double sinp = 2 * (w * y - z * x);
    if (abs(sinp) >= 1)
        angle.pitch = copysign(M_PI / 2, sinp) * (180.0 / M_PI);
    else
        angle.pitch = asin(sinp) * (180.0 / M_PI);

    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angle.yaw = atan2(siny_cosp, cosy_cosp) * (180.0 / M_PI);
    updateContinuousYaw(angle.yaw);

    return angle;
}

void BNO085::updateContinuousYaw(float currentRawYaw) {
    float delta = currentRawYaw - _prevRawYaw;

    if (delta > 180.0) {
        delta -= 360.0;
    } else if (delta < -180.0) {
        delta += 360.0;
    }

    _accumulatedYaw += delta;
    _prevRawYaw = currentRawYaw;
}

void BNO085::resetYaw() {
    _yawOffset = _accumulatedYaw;
}

float BNO085::getYaw() { 
    getEulerAngle();
    return _accumulatedYaw - _yawOffset; 
}

float BNO085::getRoll() { return getEulerAngle().roll; }
float BNO085::getPitch() { return getEulerAngle().pitch; }