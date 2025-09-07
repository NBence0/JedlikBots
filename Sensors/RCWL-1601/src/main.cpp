#include <Arduino.h>
#include <Wire.h>
#include "RCWL_1601_i2c.h"

const int SDA_PIN = 21; // ESP32 default
const int SCL_PIN = 20; // ESP32 default

// csak a fontos kommentek
I2C_Sonar Sonar(0x57, &Wire); // fix 7-bit cím

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);            // init I2C
  Sonar.startScanning();   // elindítjuk a mérést
}

void loop() {
  // a modulnak kell idő a pinghez — kb 200-500 ms
  delay(300);
  long um = Sonar.readUm();     // visszaadja mikrométerben (big-endian 3 byte)
  float cm = um / 10000.0;      // um -> cm (10000 um = 1 cm)
  Serial.print(cm);
  Serial.println(" cm");

  Sonar.stopScanning();   // leállítjuk, hogy más I2C eszköz is használhassa a buszt
  delay(100);             // kis szünet, hogy a szenzor visszaálljon
  Sonar.startScanning();  // újraindítjuk a következő méréshez
}
