// File: main.cpp
#include <Arduino.h>
#include "LED.h"

// --- Protokoll Konstansok ---
// Ezeknek egyezniük kell a Python szkriptben lévőkkel
// Parancsok (RPI -> ESP32)
const uint8_t CMD_PING = 0;
const uint8_t CMD_RESTART = 1;
const uint8_t CMD_SET_LED = 7;

// Válaszok (ESP32 -> RPI)
const uint8_t RSP_ACK = 100;         // Sikeres parancs végrehajtás
const uint8_t RSP_PONG = 101;        // Válasz a PING-re
const uint8_t RSP_ERROR = 200;       // Hiba történt (pl. rossz checksum)
const uint8_t RSP_UNKNOWN_CMD = 201; // Ismeretlen parancs

// --- Kommunikációs Pinek ---
#define RX1_PIN 20
#define TX1_PIN 21

Led led; // LED vezérlő objektum

// Ellenőrzőösszeg számító függvény
uint16_t fletcher16(const uint8_t *data, size_t len) {
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  for (size_t i = 0; i < len; ++i) {
    sum1 = (sum1 + data[i]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }
  return (sum2 << 8) | sum1;
}

// Válasz küldése a Raspberry Pi-nek
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

// Bejövő csomag feldolgozása
void processPacket(const uint8_t* buffer) {
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

  switch (cmd) {
    case CMD_PING:
      sendResponse(RSP_PONG);
      break;

    case CMD_RESTART:
      sendResponse(RSP_ACK, cmd); // Nyugta küldése újraindítás előtt
      delay(100); // Időt adunk a válasznak, hogy kimenjen
      ESP.restart();
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
  // A Serial debug portot kikapcsolhatod a végleges kódban
  // Serial.begin(115200);
  
  // Kommunikáció a Raspberry Pi-vel
  Serial1.begin(576000, SERIAL_8N1, RX1_PIN, TX1_PIN);
  
  led.begin(); // LED szalag inicializálása
  led.off();   // Kezdetben a LED legyen kikapcsolva
}

void loop() {
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
  }
}