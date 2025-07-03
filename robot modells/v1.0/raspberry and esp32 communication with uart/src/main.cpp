#include <Arduino.h>

void setup() {
  Serial.begin(115200); // Debug USB
  Serial1.begin(115200, SERIAL_8N1, 37, 38); // RX=18, TX=17, Raspberry Pi Pico W-vel kommunikáció
}


void loop() {
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();

    if (command == "GET") {
      int number = random(100, 200);  // Másik tartomány
      Serial1.println(number);        // Válasz
    }
  }
}