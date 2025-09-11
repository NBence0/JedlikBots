
#include "BME_SENSOR.h"
#include "Constans.h"

bool BMESensor::begin() {

    if (!_bme.begin()) {
        _connected = false;
        return false;
    }

    _connected = true;
    return true;
}


bool BMESensor::read() {
    if (!_connected) {
        return false;
    }

    _bme.refresh();

    _temperature = _bme.temperature;
    _humidity = _bme.humidity;
    _pressure = _bme.pressure / 100.0F; // Átváltás Pascals (Pa) -> hectoPascals (hPa/mbar)

    if (isnan(_temperature) || isnan(_humidity) || isnan(_pressure)) {
        return false;
    }

    return true;
}

float BMESensor::getTemperature() {
    return _temperature;
}

float BMESensor::getHumidity() {
    return _humidity;
}


float BMESensor::getPressure() {
    return _pressure;
}

bool BMESensor::isConnected() {
    return _connected;
}