#include "Gyro.h"
#include <math.h>
#include <Constans.h>


bool BNO08xGyro::begin() {

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

void BNO08xGyro::getEuler(float &roll, float &pitch, float &yaw, bool applyOffset) {
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

    if (applyOffset) {
        roll -= _rollOffset;
        pitch -= _pitchOffset;
        yaw -= _yawOffset;
    }

}

bool BNO08xGyro::isConnected() {
    return _connected;
}

bool BNO08xGyro::reset() {
    if (!_connected) {
        return false; // Nem tudjuk resetelni, ha nincs kapcsolat
    }

    if (_bno08x.wasReset()) {
    Serial.println("Sensor was reset");
    setReports();
  }
    return true;
}

void BNO08xGyro::calibrateToAngle(float knownYaw) {
    // 1. Frissítjük az adatokat, hogy a legfrissebb pozíciót kapjuk meg
    update();
    delay(10); // Kis szünet a biztos adatért

    // 2. Kiolvassuk a NYERS, offset nélküli szöget.
    // Ehhez ideiglenesen nullázzuk az offsetet, számolunk, majd visszaállítjuk.
    float savedOffset = _yawOffset;
    _yawOffset = 0; // Ideiglenesen kikapcsoljuk a korrekciót

    float currentRawYaw, temp_r, temp_p;
    getEuler(temp_r, temp_p, currentRawYaw); // Lekérjük a nyers szöget

    // 3. Kiszámoljuk az új, helyes offsetet
    // Az offset = (amit a szenzor mér) - (aminek lennie kellene)
    // Pl. ha a szenzor 88.7-et mér, de 90-nek kellene lennie, a hiba -1.3 fok.
    // Az új offset ezt a -1.3-at korrigálja.
    _yawOffset = currentRawYaw - knownYaw;

    // Ha a jövőben a getEuler() levonja ezt az offsetet, a helyes értéket kapjuk.
    // Példa: 88.7 - (-1.3) = 90.0

//    Serial.println("--- Gyro CALIBRATION to angle COMPLETE ---");
  //  Serial.print("Measured angle: "); Serial.println(currentRawYaw);
    //Serial.print("Known angle: "); Serial.println(knownYaw);
    //Serial.print("New Yaw Offset: "); Serial.println(_yawOffset);
}
