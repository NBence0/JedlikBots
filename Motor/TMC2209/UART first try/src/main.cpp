#include <TMCStepper.h>
#include <Arduino.h>

#define DIR_PIN  6
#define STEP_PIN 7
#define DRIVER_ADDRESS 0b00
#define R_SENSE 0.11f

#define TX_PIN 17 

HardwareSerial TMCSerial(2);

TMC2209Stepper TMCdriver(&TMCSerial, R_SENSE, DRIVER_ADDRESS);

int accel;
long maxSpeed;
int speedChangeDelay;
bool dir = false;

void setup() {
  Serial.begin(115200);
  TMCSerial.begin(115200, SERIAL_8N1, -1, TX_PIN);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  delay(500);

  Serial.println(115200);
  TMCdriver.begin();
  TMCdriver.toff(5);
  TMCdriver.rms_current(1200);
  TMCdriver.microsteps(256);
  TMCdriver.en_spreadCycle(false);
  TMCdriver.pwm_autoscale(true);
}




// steps_per_sec ≈ VACTUAL * 0.76294
// 1000 step/sec ≈ 1312 VACTUAL
void loop() {
  accel = 10000;
  maxSpeed = 100000;
  speedChangeDelay = 100;

  for (long i = 0; i <= maxSpeed; i += accel) {
    TMCdriver.VACTUAL(i);
    delay(50);
  }

  for (long i = maxSpeed; i >= 0; i -= accel) {
    TMCdriver.VACTUAL(i);
    delay(50);
  }

  dir = !dir;
  TMCdriver.shaft(dir);
  delay(1000);
  TMCdriver.VACTUAL(100000);
  delay(1000);
  TMCdriver.VACTUAL(0);
  delay(1000);
  TMCdriver.VACTUAL(-100000);
}
