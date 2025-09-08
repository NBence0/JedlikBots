// command_handler.cpp

#include "command_handler.h"
#include "communication.h" // Hogy hívhassa a prepare_response-t
#include "Constans.h"
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"
#include "esp_crc.h"

// Itt kellene példányosítani a perifériákat, vagy extern-ként hivatkozni rájuk
extern Led led;
extern BNO08xGyro gyro;
extern ColorSensor colorSensor;

void processCommand(const uint8_t* rx_buf) {
    // 1. CRC ellenőrzése
    uint16_t received_crc = (rx_buf[5] << 8) | rx_buf[4];
    uint16_t calculated_crc = esp_rom_crc16_le(0xFFFF, (uint8_t*)rx_buf, PAYLOAD_SIZE);

    if (received_crc != calculated_crc) {
        prepare_response(RSP_ERROR_CRC);
        Serial.printf("CRC Hiba! Kapott: %04X, Számolt: %04X\n", received_crc, calculated_crc);
        return;
    }

    // 2. Parancs feldolgozása
    uint8_t cmd = rx_buf[0];
    uint8_t param1 = rx_buf[1];
    uint8_t param2 = rx_buf[2];

    switch (cmd) {
        case CMD_PING:
            prepare_response(RSP_PONG);
            break;
        case CMD_RESTART:
            prepare_response(RSP_ACK, cmd);
            delay(100);
            ESP.restart();
            break;
        case CMD_READ_GYRO:
            // Ezt a feladatot a tasks.cpp-be is tehetnénk
            if (gyro.update()) {
                float roll, pitch, yaw;
                gyro.getEuler(roll, pitch, yaw);
                // A válasz előkészítése itt hiányzik, ezt implementálni kell!
                // Pl. prepare_response(RSP_GYRO_DATA, ...);
                Serial.printf("Gyro: R:%.2f P:%.2f Y:%.2f\n", roll, pitch, yaw);
            }
            break;
        case CMD_CALIBRATE_GYRO:
            gyro.calibrateToAngle(param1);
            prepare_response(RSP_ACK, cmd);
            break;
        case CMD_READ_COLOR_SENSOR:
            uint16_t value;
            colorSensor.readOneChannel(param2, value);
            // A válasz előkészítése itt is hiányzik
            Serial.printf("Color channel %d: %d\n", param2, value);
            break;
        case CMD_SET_LED:
            led.handleColor(param1);
            prepare_response(RSP_ACK, cmd, param1);
            break;
        case CMD_TEST:
            prepare_response(RSP_PONG);
            Serial.println("Teszt parancs fogadva, PONG válasz küldve.");
            break;
        default:
            prepare_response(RSP_UNKNOWN_CMD, cmd);
            break;
    }
}