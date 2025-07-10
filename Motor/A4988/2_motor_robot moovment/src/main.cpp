#include <Arduino.h>
#include <AccelStepper.h>
#define TOUCH_PIN T8

constexpr int DIR_PIN1 = 17;
constexpr int STEP_PIN1 = 16;

constexpr int DIR_PIN2 = 6;
constexpr int STEP_PIN2 = 7;

AccelStepper motor1(AccelStepper::DRIVER, STEP_PIN1, DIR_PIN1);
AccelStepper motor2(AccelStepper::DRIVER, STEP_PIN2, DIR_PIN2);

long currentPos = 0;
int stepSize = 10000;

void moveMotors(long motor1Target, long motor2Target) {
  motor1.moveTo(motor1Target);
  motor2.moveTo(motor2Target);
  while (motor1.distanceToGo() != 0 || motor2.distanceToGo() != 0) {
    motor1.run();
    motor2.run();
  }
  delay(500);
}

void waitForTouch() {
  while (touchRead(TOUCH_PIN) < 30000) {
    Serial.println(touchRead(TOUCH_PIN));
    delay(10);
  }
}

void setup() {


  Serial.begin(115200);
  delay(1000);

  int microstepp = 16;

  motor1.setMaxSpeed(600 * microstepp);
  motor1.setAcceleration(1000 * microstepp);

  motor2.setMaxSpeed(600 * microstepp);
  motor2.setAcceleration(1000 * microstepp);
}


void loop() {
  waitForTouch();
  // Előre
  for (int i = 0; i < 3; i++)
  {
    currentPos += stepSize;
  moveMotors(currentPos, currentPos);
  Serial.println(touchRead(TOUCH_PIN));


  // Hátra
  currentPos -= stepSize;
  moveMotors(currentPos, currentPos);

  // Jobbra
  currentPos += stepSize;
  moveMotors(currentPos, -currentPos);

  // Balra
  currentPos -= stepSize;
  moveMotors(-currentPos, currentPos);
  }
  
  
}
