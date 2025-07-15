#include <Adafruit_MPL3115A2.h>
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 20

Adafruit_MPL3115A2 mpl;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Adafruit_MPL3115A2 test!");

  if (!mpl.begin(&Wire)) {
    Serial.println("Could not find sensor. Check wiring.");
    while (1);
  }
  Serial.println("Setting mode to barometer (pressure).");
  mpl.setMode(MPL3115A2_BAROMETER);
  mpl.setSeaPressure(1004.5);
}

void loop() {
  Serial.println("Starting a conversion.");
  mpl.startOneShot();

  int count = 0;
  while (!mpl.conversionComplete()) {
    count++;
  }

  Serial.print("Done! Counted to "); Serial.println(count);
  Serial.print("Pressure = ");
  Serial.println(mpl.getLastConversionResults(MPL3115A2_PRESSURE));
  Serial.print("Temperature = ");
  Serial.println(mpl.getLastConversionResults(MPL3115A2_TEMPERATURE));
  //Serial.print("Altitude = ");
  //Serial.println(mpl.getLastConversionResults(MPL3115A2_ALTITUDE));
  Serial.println();

  delay(1000);
}
