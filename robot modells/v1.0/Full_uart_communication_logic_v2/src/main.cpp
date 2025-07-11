#include <Arduino.h>
#include "LED.h"

// Commands (RPI -> ESP32)
const uint8_t CMD_PING = 0;
const uint8_t CMD_RESTART = 1;
const uint8_t CMD_SET_LED = 7;

// Responses (ESP32 -> RPI)
const uint8_t RSP_ACK = 100;         // Command executed successfully
const uint8_t RSP_PONG = 101;        // Response to PING
const uint8_t RSP_ERROR = 200;       // Error occurred (e.g. bad checksum)
const uint8_t RSP_UNKNOWN_CMD = 201; // Unknown command

#define RX1_PIN 20
#define TX1_PIN 21

Led led; // LED controller object

// Checksum calculation function
uint16_t fletcher16(const uint8_t *data, size_t len) {
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  for (size_t i = 0; i < len; ++i) {
    sum1 = (sum1 + data[i]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }
  return (sum2 << 8) | sum1;
}

// Send response to Raspberry Pi
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
  //Serial.print("Sent response: ");
  for (int i = 0; i < 7; i++) {
    //Serial.print(response[i]);
    //Serial.print(" ");
  }
  //Serial.println();
}

void processPacket(const uint8_t* buffer) {
  uint16_t received_checksum = (buffer[6] << 8) | buffer[5];
  uint16_t calculated_checksum = fletcher16(buffer, 5);

  if (received_checksum != calculated_checksum) {
    sendResponse(RSP_ERROR); // Send error if checksum is invalid
    return;
  }

  uint8_t cmd    = buffer[1];
  uint8_t param1 = buffer[2];
  // uint8_t param2 = buffer[3];
  // uint8_t param3 = buffer[4];

  switch (cmd) {
    case CMD_PING:
      sendResponse(RSP_PONG);
      break;

    case CMD_RESTART:
      sendResponse(RSP_ACK, cmd);
      delay(100); // Give time for response to be sent
      ESP.restart();
      break;

    case CMD_SET_LED:
      led.handleColor(param1);
      // ACK with original command and parameter
      sendResponse(RSP_ACK, cmd, param1);
      break;

    default:
      sendResponse(RSP_UNKNOWN_CMD, cmd); // Unknown command
      break;
  }
}

void setup() {
  Serial1.begin(576000, SERIAL_8N1, RX1_PIN, TX1_PIN); // RPI Uart
  Serial.begin(115200); // Debug port for Serial Monitor
  
  led.begin();
  led.off();
}

void loop() {
  static uint8_t buffer[7];
  static uint8_t bytes_received = 0;

  // Non-blocking read from serial port
  while (Serial1.available() > 0) {
    uint8_t incoming_byte = Serial1.read(); // Read bytes one by one

    if (bytes_received == 0) {
      if (incoming_byte == 0xAA) {
        buffer[0] = incoming_byte;
        bytes_received = 1;
      }
      // Ignore until "0xAA" start byte found
    } else {
      // Collect the rest of the packet
      buffer[bytes_received++] = incoming_byte;
    }

    // Full packet received
    if (bytes_received == 7) {
      /*
      Serial.print("Received packet:");
      for (int i = 0; i < 7; i++) {
        Serial.print(buffer[i]);
        Serial.print(" ");
      }
      */
      processPacket(buffer);
      bytes_received = 0; // Reset for next packet
    }
  }
}
