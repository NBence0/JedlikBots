#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LED.h"


class Robot {
public:
  int getcolor(uint8_t index) {
    return 100 + index;
  }
};

Robot robot;
Led led;
uint16_t fletcher16(const uint8_t *data, size_t len) {
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  for (size_t i = 0; i < len; ++i) {
    sum1 = (sum1 + data[i]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }
  return (sum2 << 8) | sum1;
}

void sendResponse(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4 = 0) {
  uint8_t response[7];
  response[0] = 0xAA;
  response[1] = data1;
  response[2] = data2;
  response[3] = data3;
  response[4] = data4;

  uint16_t checksum = fletcher16(response, 5);
  response[5] = checksum & 0xFF;
  response[6] = (checksum >> 8) & 0xFF;

  Serial1.write(response, 7);
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(576000, SERIAL_8N1, 20, 21); // RX, TX
  while (!Serial);
  Serial.println("ESP32 UART vevő elindult.");
}

void loop() {
  if (Serial1.available() >= 7) {
    uint8_t buffer[7];
    Serial1.readBytes(buffer, 7);
    /*
    Serial.print("📥 Fogadott: ");
    for (int i = 0; i < 7; i++) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    */
    if (buffer[0] != 0xAA) {
      return;
    }

    uint8_t cmd    = buffer[1];
    uint8_t param1 = buffer[2];
    uint8_t param2 = buffer[3];
    uint8_t param3 = buffer[4];

    uint16_t received_checksum = (buffer[6] << 8) | buffer[5];
    uint16_t calculated_checksum = fletcher16(buffer, 5);

    if (received_checksum != calculated_checksum) {
      return;
    }

    switch (cmd) {
      case 0: //reastart
        Serial.println("Restart");
        ESP.restart();
        break;

      case 1: //color sensor
        Serial.print("Color Sensor");
        //sendResponse(param1, param2, param3);
        break;

      case 2: //Gyro
        Serial.print("Gyro");
        break;
      case 3:

        break;
      case 4:

        break;
      case 5:
      
        break;
      case 6:

        break;
      case 7:
        led.handleColor(param1); // param1 Led color
        break;
      case 8:
        break;
      case 9:
        break;
      case 10:
        break;
      default:
        Serial.println("Ismeretlen parancs");
        break;
    }
  }
}
