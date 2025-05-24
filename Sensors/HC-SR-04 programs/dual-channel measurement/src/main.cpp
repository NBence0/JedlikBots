#include <Arduino.h>

const int trigPin1 = 5;
const int echoPin1 = 18;
const int trigPin2 = 22;
const int echoPin2 = 23;

float duration1, distance1;
float duration2, distance2;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  Serial.begin(115200);
}

void loop() {
    // Szenzor 1 mérése
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = (duration1 * 0.0343) / 2;

    // Szenzor 2 mérése
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = (duration2 * 0.0343) / 2;

    // Kiírás
    Serial.print("Szenzor 1 távolság: ");
    Serial.print(distance1);
    Serial.println(" cm");

    Serial.print("Szenzor 2 távolság: ");
    Serial.print(distance2);
    Serial.println(" cm");
    delay(100); // Várakozás 1 másodperc
}