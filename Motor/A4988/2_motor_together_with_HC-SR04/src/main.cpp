#include <Arduino.h>
#include <AccelStepper.h>

constexpr int DIR_PIN1 = 17;
constexpr int STEP_PIN1 = 16;

constexpr int DIR_PIN2 = 25;
constexpr int STEP_PIN2 = 26;

constexpr int trigPin = 5;
constexpr int echoPin = 18;

AccelStepper motor1(AccelStepper::DRIVER, STEP_PIN1, DIR_PIN1);
AccelStepper motor2(AccelStepper::DRIVER, STEP_PIN2, DIR_PIN2);

int cycleCount = 0;
float distance = 0;


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Indul a léptetőmotor-vezérlés...");
  int microstepp = 16;

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  motor1.setMaxSpeed(600 * microstepp);
  motor1.setAcceleration(30 * microstepp);
  motor1.setSpeed(0);

  motor2.setMaxSpeed(600 * microstepp);
  motor2.setAcceleration(30 * microstepp);
  motor2.setSpeed(0);
}

void loop() {
  cycleCount++;

  if (cycleCount >= 50000) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 30000);
    distance = duration * 0.0343 / 2;
    cycleCount = 0;
  }

  float speed = distance * 50;

  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor1.runSpeed();
  motor2.runSpeed();
}