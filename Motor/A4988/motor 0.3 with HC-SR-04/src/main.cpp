#include <Arduino.h>

const int trigPin = D1;  // GPIO5
const int echoPin = D2;  // GPIO4
const int dirPin = D4;
const int stepPin = D3;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  digitalWrite(dirPin, HIGH); // állandó irány (opcionálisan váltogatható is)
}

// Távolságmérés (HC-SR04)
long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // max 20 ms = kb. 340 cm
  long distance = duration * 0.034 / 2;

  if (distance == 0) distance = 50;  // ha nincs jel, tekintsük max távolnak
  return constrain(distance, 5, 50); // korlátozzuk 5 és 50 cm közé
}

// Motor forgatása adott sebességgel
void spin(int count, int dir, int del) {
  digitalWrite(dirPin, dir);
  for (int i = 0; i < count; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(del);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(del);
  }
}

void loop() {
  // Minden ciklusban új távolság és új sebesség
  long distance = readDistanceCM();

  // Leképezés: 5 cm → 500 µs delay, 50 cm → 5000 µs delay
  int delayMicros = map(distance, 5, 50, 500, 5000);

  // Debug info soros monitorra
  Serial.print("Tavolsag: ");
  Serial.print(distance);
  Serial.print(" cm  ->  Sebesseg delay: ");
  Serial.print(delayMicros);
  Serial.println(" us");

  // Pörögjön folyamatosan kis adagokban (pl. 20 lépés), automatikusan frissítve
  spin(20, 1, delayMicros);
}
