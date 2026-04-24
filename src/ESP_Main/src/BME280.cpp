#include <BME280.h>

BME280::BME280(uint8_t cs_pin) : bme(Adafruit_BME280(cs_pin)) {}

bool BME280::begin() {
    bme.begin();
    return true;
}

float BME280::readTemperature() {
    return bme.readTemperature();
}

float BME280::readHumidity() {
    return bme.readHumidity();
}

float BME280::readPressure() {
    return bme.readPressure();
}
