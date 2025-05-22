#include <Arduino.h>

const int trigPin1 = 11;
const int echoPin1 = 12;
const int trigPin2 = 8;
const int echoPin2 = 9;

void led(int r, int g, int b);

int i = 0;

float duration1, distance1;
float duration2, distance2;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  Serial.begin(9600);
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
}