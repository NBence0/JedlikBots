#include <Arduino.h>
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 20); // ESP32S3 I2C lábak

  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Szenzor nem található!");
    while (1);
  }

  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(50);
}

void loop() {
  int distance = sensor.read();
  if (sensor.timeoutOccurred()) {
    Serial.println("Időtúllépés!");
  } else {
    Serial.print("Távolság: ");
    Serial.print(distance-22);
    Serial.println(" mm");
  }
  delay(100);
}
