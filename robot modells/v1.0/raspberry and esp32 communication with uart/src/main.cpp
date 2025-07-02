#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "GET") {
      int randNum = random(1, 101);  // 1–100 közötti szám
      Serial.println(randNum);
    }
  }
}
