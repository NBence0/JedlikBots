// File: main.cpp
#include <Arduino.h>
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"

// --- Protokoll Konstansok ---
// Ezeknek egyezniük kell a Python szkriptben lévőkkel
// Parancsok (RPI -> ESP32)
const uint8_t CMD_PING = 0;
const uint8_t CMD_RESTART = 1;
const uint8_t CMD_READ_GYRO = 2;
const uint8_t CMD_CALIBRATE_GYRO = 3;
const uint8_t CMD_READ_COLOR_SENSOR = 4;
const uint8_t CMD_SET_LED = 7;

// Response (ESP32 -> RPI)
const uint8_t RSP_ACK = 100;         // Sikeres parancs végrehajtás
const uint8_t RSP_PONG = 101;        // Válasz a PING-re
const uint8_t RSP_ERROR = 200;       // Hiba történt (pl. rossz checksum)
const uint8_t RSP_UNKNOWN_CMD = 201; // Ismeretlen parancs


#define RX1_PIN 20
#define TX1_PIN 21

Led led;
BNO08xGyro gyro;
ColorSensor colorSensor;


uint16_t fletcher16(const uint8_t *data, size_t len) {
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  for (size_t i = 0; i < len; ++i) {
    sum1 = (sum1 + data[i]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }
  return (sum2 << 8) | sum1;
}


void sendResponse(uint8_t cmd, uint8_t p1 = 0, uint8_t p2 = 0, uint8_t p3 = 0) {
  uint8_t response[7];
  response[0] = 0xAA;    // Start byte
  response[1] = cmd;
  response[2] = p1;
  response[3] = p2;
  response[4] = p3;

  uint16_t checksum = fletcher16(response, 5);
  response[5] = checksum & 0xFF;
  response[6] = (checksum >> 8) & 0xFF;

  Serial1.write(response, 7);
}


void processPacket() {
  /*
  const uint8_t* buffer ez megy a függvényhívásba
  uint16_t received_checksum = (buffer[6] << 8) | buffer[5];
  uint16_t calculated_checksum = fletcher16(buffer, 5);

  if (received_checksum != calculated_checksum) {
    sendResponse(RSP_ERROR); // Hiba küldése rossz checksum esetén
    return;
  }

  uint8_t cmd    = buffer[1];
  uint8_t param1 = buffer[2];
  // uint8_t param2 = buffer[3]; // Jelenleg nincsenek használva
  // uint8_t param3 = buffer[4];
  */
  uint8_t cmd = CMD_READ_COLOR_SENSOR;
  uint8_t param1 = 25;
  uint8_t param2 = 4;

  switch (cmd) {
    case CMD_PING:
      sendResponse(RSP_PONG);
      break;

    case CMD_RESTART:
      sendResponse(RSP_ACK, cmd);
      delay(100);
      ESP.restart();
      break;

    case CMD_READ_GYRO:
      if (gyro.update()) {
        float roll, pitch, yaw;
        gyro.getEuler(roll, pitch, yaw);
        Serial.print("Roll: "); Serial.print(roll);
        Serial.print(" Pitch: "); Serial.print(pitch);
        Serial.print(" Yaw: "); Serial.println(yaw);
      } else {
        Serial.println("Gyro update failed. No new data.");
      }
      break; // HIÁNYZÓ BREAK!
    case CMD_CALIBRATE_GYRO:
      // Példa: 90 fokra kalibrálás
      gyro.calibrateToAngle(param1);
      sendResponse(RSP_ACK, cmd); // Nyugta küldése a kalibráció után
      break;
    case CMD_READ_COLOR_SENSOR:
      uint16_t value;
      colorSensor.readOneChannel(param2, value);
      Serial.print("Color channel ");
      Serial.print(param2);
      Serial.print("value : ");
      Serial.println(value);
      break;
    case CMD_SET_LED:
      led.handleColor(param1);
      // Nyugta: ACK, az eredeti parancsra, az eredeti paraméterrel
      sendResponse(RSP_ACK, cmd, param1);
      break;


    default:
      sendResponse(RSP_UNKNOWN_CMD, cmd); // Ismeretlen parancs jelzése
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(576000, SERIAL_8N1, RX1_PIN, TX1_PIN);
  led.begin();
  colorSensor.begin();

  gyro.begin();
  led.off();
}

void loop() {
  /*
  static uint8_t buffer[7];
  static uint8_t bytes_received = 0;

  // Nem-blokkoló olvasás a soros portról
  while (Serial1.available() > 0) {
    uint8_t incoming_byte = Serial1.read();

    // Ha szinkronizációs hiba van, keressük a csomag elejét
    if (bytes_received == 0) {
      if (incoming_byte == 0xAA) {
        buffer[0] = incoming_byte;
        bytes_received = 1;
      }
      // Ha nem 0xAA, ignoráljuk, amíg meg nem találjuk
    } else {
      // Gyűjtjük a csomag többi részét
      buffer[bytes_received++] = incoming_byte;
    }

    // Ha a teljes csomag megérkezett
    if (bytes_received == 7) {
      processPacket(buffer);
      bytes_received = 0; // Reset a következő csomag fogadásához
    }
    */
    processPacket();
    delay(100);
  }