#include <TMCStepper.h>
#include <Arduino.h>
#include <VL53L1X.h>

// Motor 1
#define DIR_PIN1  6
#define STEP_PIN1 7
#define TX_PIN1   17

// Motor 2
#define DIR_PIN2  8
#define STEP_PIN2 9
#define TX_PIN2   16

#define DRIVER_ADDRESS 0b00  // Ha külön címen vannak, ezt módosítani kell!
#define R_SENSE 0.11f

// UART portok
HardwareSerial TMCSerial1(2); // UART2 (TX: GPIO17)
HardwareSerial TMCSerial2(1); // UART1 (TX: GPIO16)

// TMC2209 driverek
TMC2209Stepper TMCdriver1(&TMCSerial1, R_SENSE, DRIVER_ADDRESS);
TMC2209Stepper TMCdriver2(&TMCSerial2, R_SENSE, DRIVER_ADDRESS);

int accel = 10000;
long maxSpeed = 100000;
int speedChangeDelay = 50;
bool dir = false;

void setup() {
  Serial.begin(115200);

  // UART init
  TMCSerial1.begin(115200, SERIAL_8N1, -1, TX_PIN1);
  TMCSerial2.begin(115200, SERIAL_8N1, -1, TX_PIN2);

  // Step / Dir pinek
  pinMode(STEP_PIN1, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);

  delay(500);
  Serial.println("TMC2209 Motor indítás...");

  // Motor 1 init
  TMCdriver1.begin();
  TMCdriver1.toff(5);
  TMCdriver1.microsteps(16);
  TMCdriver2.rms_current(10);
  TMCdriver1.en_spreadCycle(false);
  TMCdriver1.pwm_autoscale(true);

  // Motor 2 init
  TMCdriver2.begin();
  TMCdriver2.toff(5);
  TMCdriver2.microsteps(16);
  TMCdriver2.rms_current(10);  // kb. 1.1 A PEAK, 0.8 A RMS
  TMCdriver2.en_spreadCycle(false);
  TMCdriver2.pwm_autoscale(true);
}

void loop() {

  for (long i = 0; i <= maxSpeed; i += accel) {
    TMCdriver1.VACTUAL(i);
    TMCdriver2.VACTUAL(i);
    delay(speedChangeDelay);
  }


  for (long i = maxSpeed; i >= 0; i -= accel) {
    TMCdriver1.VACTUAL(i);
    TMCdriver2.VACTUAL(i);
    delay(speedChangeDelay);
    //Serial.println(TMCdriver2.cs2rms(TMCdriver2.cs_actual()));
  }


  dir = !dir;
  TMCdriver1.shaft(dir);
  TMCdriver2.shaft(dir);
}