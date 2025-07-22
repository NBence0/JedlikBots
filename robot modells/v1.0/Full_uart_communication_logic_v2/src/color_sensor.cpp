#include <COLOR_SENSOR.h>
#include <Wire.h>

#define SDA_PIN 42
#define SCL_PIN 41

bool ColorSensor::begin(uint8_t ATIME, uint16_t ASTEP, as7341_gain_t gain) {
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!_as7341.begin()) {
        _connected = false;
        return false;
    }
    _connected = true;
        _as7341.enableLED(true);  // bekapcsolja a LED-et

    _as7341.setATIME(ATIME);
    _as7341.setASTEP(ASTEP);
    _as7341.setGain(AS7341_GAIN_256X);
    return true;
}



void ColorSensor::readOneChannel(uint8_t channel, uint16_t &value) {
    if (!_connected) {
        value = 0;
        return;
    }
    if (!_as7341.readAllChannels()) {
        Serial.println("Error reading all channels!");
        return;
    }
    if (channel < 11) {
        switch (channel)
        {
        case 1:
            value = _as7341.getChannel(AS7341_CHANNEL_415nm_F1);
            break;
        case 2:
            value = _as7341.getChannel(AS7341_CHANNEL_445nm_F2);
            break;
        case 3:
            value = _as7341.getChannel(AS7341_CHANNEL_480nm_F3);
            break;
        case 4:
            value = _as7341.getChannel(AS7341_CHANNEL_515nm_F4);
            break;
        case 5:
            value = _as7341.getChannel(AS7341_CHANNEL_555nm_F5);
            break;
        case 6:
            value = _as7341.getChannel(AS7341_CHANNEL_590nm_F6);
            break;
        case 7:
            value = _as7341.getChannel(AS7341_CHANNEL_630nm_F7);
            break;
        case 8:
            value = _as7341.getChannel(AS7341_CHANNEL_680nm_F8);
            break;
        case 9:
            value = _as7341.getChannel(AS7341_CHANNEL_CLEAR);
            break;
        case 10:
            value = _as7341.getChannel(AS7341_CHANNEL_NIR);
        default:
            break;
        }
    } else {
        value = 0; // Invalid channel
    }
}