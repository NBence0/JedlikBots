#include <BNO085.h>
#include <Arduino.h>

BNO085::BNO085(uint8_t bno_int, uint8_t cs, uint8_t rst) : bno(rst) {
    _cs = cs;
    _int = bno_int;
    _rst = rst;
}

bool BNO085::begin(SPIClass *spi) {
    if (!bno.begin_SPI(_cs, _int, spi)) {
        return false;
    } 
    return true;
}

void BNO085::enableRotation() { 
    if (!bno.enableReport(SH2_GAME_ROTATION_VECTOR, 5000)) { // Sensor mode and measure time in us
        Serial.println("Error: unable to setup, the rotation");
    }
}

void BNO085::update() {
    if (bno.wasReset()) {
        Serial.println("Bno Reseted");
        enableRotation(); // Than turn on the rotation mode again
    }

    // If the int is low the function returns
    if (!bno.getSensorEvent(&_sensorValue)) { // writes the sensor data to the _sensorValue structure
        return;
    }

    if (_sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) { 
        // writes the data to our structure
        _myData.x = _sensorValue.un.gameRotationVector.i;
        _myData.y = _sensorValue.un.gameRotationVector.j;
        _myData.z = _sensorValue.un.gameRotationVector.k;
        _myData.w = _sensorValue.un.gameRotationVector.real;
        _newDataAvailable = true;
    }
}

Rotacio BNO085::getRotation() {
    _newDataAvailable = false;
    return _myData;
}

bool BNO085::hasNewData() {
    return _newDataAvailable;
}

EulerRotacio BNO085::getEulerAngle() {
    _newDataAvailable = false;
    
    float w = _myData.w;
    float x = _myData.x;
    float y = _myData.y;
    float z = _myData.z;

    EulerRotacio angle;

    // Roll (x-axis rotation)
    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    angle.roll = atan2(sinr_cosp, cosr_cosp) * (180.0 / M_PI);

    // Pitch (y-axis rotation)
    double sinp = 2 * (w * y - z * x);
    if (abs(sinp) >= 1)
        angle.pitch = copysign(M_PI / 2, sinp) * (180.0 / M_PI); // use 90 degrees if out of range
    else
        angle.pitch = asin(sinp) * (180.0 / M_PI);

    // Yaw (z-axis rotation)
    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    angle.yaw = atan2(siny_cosp, cosy_cosp) * (180.0 / M_PI);

    return angle;
}

float BNO085::getYaw() {
    _newDataAvailable = false;
    
    float w = _myData.w;
    float x = _myData.x;
    float y = _myData.y;
    float z = _myData.z;

    float yaw; 

    double siny_cosp = 2 * (w * z + x * y);
    double cosy_cosp = 1 - 2 * (y * y + z * z);
    yaw = atan2(siny_cosp, cosy_cosp) * (180.0 / M_PI);

    return yaw;
}

float BNO085::getRoll() {
    _newDataAvailable = false;

    float w = _myData.w;
    float x = _myData.x;
    float y = _myData.y;
    float z = _myData.z;
    
    float roll;

    double sinr_cosp = 2 * (w * x + y * z);
    double cosr_cosp = 1 - 2 * (x * x + y * y);
    roll = atan2(sinr_cosp, cosr_cosp) * (180.0 / M_PI);

    return roll;
}


float BNO085::getPitch() {
    _newDataAvailable = false;
    float pitch;
    
    float w = _myData.w;
    float x = _myData.x;
    float y = _myData.y;
    float z = _myData.z;

    double sinp = 2 * (w * y - z * x);
    if (abs(sinp) >= 1)
        pitch = copysign(M_PI / 2, sinp) * (180.0 / M_PI); // use 90 degrees if out of range
    else
        pitch = asin(sinp) * (180.0 / M_PI);

    return pitch;
}