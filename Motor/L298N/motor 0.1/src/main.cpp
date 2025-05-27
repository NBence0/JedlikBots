#include <Arduino-.h>
#include <AccelStepper.h>

// Define pins for motor driver IN1-IN4
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4

// Create stepper motor object (4 wire mode)
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  stepper.setMaxSpeed(1000);    // beállított maximális sebesség
  stepper.setSpeed(200);        // aktuális mozgási sebesség
}

void loop() {
  stepper.runSpeed();  // folyamatos mozgás
}
// A motor sebessége és iránya a setSpeed() függvényben állítható be
