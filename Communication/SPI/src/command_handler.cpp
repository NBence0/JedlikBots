// command_handler.cpp (MÓDOSÍTOTT)

#include "command_handler.h"
#include "communication.h"
#include "Constans.h"
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"
#include "esp_crc.h"
#include "DeviceManager.h"

extern Led led;
extern BNO08xGyro gyro;
extern ColorSensor colorSensor;
extern DeviceManager deviceManager;

void processCommand(const uint8_t* rx_buf) {
    uint16_t received_crc = (rx_buf[5] << 8) | rx_buf[4];
    uint16_t calculated_crc = esp_rom_crc16_le(0xFFFF, (uint8_t*)rx_buf, PAYLOAD_SIZE);

    if (received_crc != calculated_crc) {
        prepare_response(RSP_ERROR_CRC);
        Serial.printf("CRC Hiba! Kapott: %04X, Számolt: %04X\n", received_crc, calculated_crc);
        return;
    }

    uint8_t cmd = rx_buf[0];
    uint8_t param1 = rx_buf[1];
    uint8_t param2 = rx_buf[2];
    uint8_t param3 = rx_buf[3];




    switch (cmd) {
        case CMD_PING:
            prepare_response(RSP_PONG);
            break;

        case CMD_RESTART:
            prepare_response(RSP_ACK, cmd);
            delay(100);
            ESP.restart();
            break;

        case CMD_READ_GYRO: {
            deviceManager.selectDevice(DEVICE_GYRO);
            if (gyro.update()) {
                float roll, pitch, yaw;
                gyro.getEuler(roll, pitch, yaw);
                Serial.printf("Gyro: R:%.2f P:%.2f Y:%.2f\n", roll, pitch, yaw);

                int16_t scaled_roll = (int16_t)(roll * 100.0f);
                
                prepare_response(RSP_GYRO_DATA, 
                                 scaled_roll & 0xFF,        // Alsó bájt (LSB)
                                 (scaled_roll >> 8) & 0xFF, // Felső bájt (MSB)
                                    0);

                digitalWrite(DATA_READY_PIN, HIGH);
            } else {
                prepare_response(RSP_ERROR_GYRO, cmd);
            }
            deviceManager.deselectAll();
            break;
        }

        case CMD_CALIBRATE_GYRO:
            gyro.calibrateToAngle(param1);
            prepare_response(RSP_ACK, cmd);
            digitalWrite(DATA_READY_PIN, HIGH);
            break;

        case CMD_READ_COLOR_SENSOR: {
            uint16_t value;
            colorSensor.readOneChannel(param2, value);
            Serial.printf("Color channel %d: %d\n", param2, value);

            // Válasz előkészítése a 16 bites adattal
            prepare_response(RSP_COLOR_DATA, value & 0xFF, (value >> 8) & 0xFF, param2);

            digitalWrite(DATA_READY_PIN, HIGH);
            break;
        }

        case CMD_READ_BME:
            deviceManager.selectDevice(DEVICE_BME);

            break;
        case CMD_SET_LED:
            led.handleColor(param1);
            prepare_response(RSP_ACK, cmd, param1);
            break;

        case CMD_TEST:
            prepare_response(RSP_PONG);
            Serial.println("Teszt parancs fogadva, PONG válasz küldve.");
            break;
        case CMD_NOP:
            break;
        default:
            prepare_response(RSP_UNKNOWN_CMD, cmd);
            break;
    }
}