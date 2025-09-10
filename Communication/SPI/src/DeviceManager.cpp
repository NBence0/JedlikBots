#include "DeviceManager.h"
#include "Constans.h"

DeviceManager::DeviceManager(HC154& mux, BNO08xGyro& gyro, BMESensor& bme)
    : _mux(mux), _gyro(gyro), _bme(bme) {}

void DeviceManager::beginAll() {
    Serial.println("Eszközkezelő: Szenzorok inicializálása...");

    // Giroszkóp inicializálása
    selectDevice(DEVICE_GYRO);
    if (_gyro.begin()) {
        Serial.println("  Giro OK.");
    } else {
        Serial.println("  Giro HIBA!");
    }

    // BME280 inicializálása
    selectDevice(DEVICE_BME);
    if (_bme.begin()) {
        Serial.println("  BME280 OK.");
    } else {
        Serial.println("  BME280 HIBA!");
    }

    deselectAll();
    Serial.println("Eszközkezelő: Inicializálás kész.");
}

void DeviceManager::selectDevice(DeviceType device) {
    uint8_t channel;
    switch (device) {
        case DEVICE_GYRO:
            channel = MUX_CHANNEL_GYRO;
            break;
        case DEVICE_BME:
            channel = MUX_CHANNEL_BME;
            break;
        default:
            channel = MUX_NO_DEVICE_SELECTED;
            break;
    }
    _mux.selectChannel(channel);
    delay(1); // Rövid várakozás a MUX stabil működéséért
}

void DeviceManager::deselectAll() {
    _mux.selectChannel(MUX_NO_DEVICE_SELECTED);
}