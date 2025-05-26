#include <Arduino.h>

const int dirPin = D4;
const int stepPin = D3;
const int stepsPerRevolution = 200;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);  // Soros kommunikáció
}

void spin(int count, int dir, int del) {
  digitalWrite(dirPin, dir);

  unsigned long startTime = micros();  // Kezdő idő rögzítése

  for (int i = 0; i < count; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(del);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(del);
  }

  unsigned long endTime = micros();  // Végidő

  unsigned long duration = endTime - startTime;
  Serial.print("Delay: ");
  Serial.print(del);
  Serial.print(" us -> One revolution took: ");
  Serial.print(duration);
  Serial.println(" microseconds");
}

void loop() {
  int delays[] = {2000, 1800,1600,1400, 1200, 1000, 800, 600};  // Különböző sebességek µs-ban

  for (int i = 0; i < 5; i++) {
    spin(stepsPerRevolution, 1, delays[i]);
    delay(1000);  // Várakozás két kör között
  }

  while (1); // Állj meg a loop végén
}
