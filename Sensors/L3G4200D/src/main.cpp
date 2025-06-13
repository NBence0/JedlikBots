#include <Arduino.h>
#include <Wire.h>

// L3G4200D I2C címe
#define L3G4200D_ADDRESS 0x69

// Regiszter címek
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

// Függvény deklarációk
void setupL3G4200D();
void writeRegister(int reg, int value);
int readRegister(int reg);
void getGyroValues(int* x, int* y, int* z);

void setup() {
  Serial.begin(115200); // ESP-nél gyakran magasabb baudrate
  
  // ESP8266-nál egyedi SDA/SCL pineket is megadhatsz:
  // Wire.begin(SDA_PIN, SCL_PIN); // pl. Wire.begin(4, 5);
  Wire.begin(4, 5); // I2C busz indítása a megadott SDA és SCL pinekkel
  
  Serial.println("L3G4200D Giroszkóp inicializálása...");
  
  // Szenzor inicializálása
  setupL3G4200D();
  
  Serial.println("Inicializálás kész!");
  delay(1000);
}

void loop() {
  // Giroszkóp adatok olvasása
  int x, y, z;
  //getGyroValues(&x, &y, &z);
  
  // Nyers adatok kiíratása
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" | Y: ");
  Serial.print(y);
  Serial.print(" | Z: ");
  Serial.println(z);
  
  // Átváltás fok/másodpercre (250 dps skála esetén)
  float x_dps = x * 0.00875; // 250/32768 = 0.00875
  float y_dps = y * 0.00875;
  float z_dps = z * 0.00875;
  
  Serial.print("X: ");
  Serial.print(x_dps);
  Serial.print(" dps | Y: ");
  Serial.print(y_dps);
  Serial.print(" dps | Z: ");
  Serial.print(z_dps);
  Serial.println(" dps");
  
  Serial.println("---");
  delay(500);
}

void setupL3G4200D() {
  // CTRL_REG1: Szenzor bekapcsolása, 100Hz, mind a 3 tengely engedélyezése
  writeRegister(CTRL_REG1, 0x0F);
  
  // CTRL_REG4: ±250 dps skála beállítása
  writeRegister(CTRL_REG4, 0x00);
}

void writeRegister(int reg, int value) {
  Wire.beginTransmission(L3G4200D_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

int readRegister(int reg) {
  Wire.beginTransmission(L3G4200D_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(L3G4200D_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

void getGyroValues(int* x, int* y, int* z) {
  byte xMSB = readRegister(OUT_X_H);
  byte xLSB = readRegister(OUT_X_L);
  *x = ((xMSB << 8) | xLSB);
  
  byte yMSB = readRegister(OUT_Y_H);
  byte yLSB = readRegister(OUT_Y_L);
  *y = ((yMSB << 8) | yLSB);
  
  byte zMSB = readRegister(OUT_Z_H);
  byte zLSB = readRegister(OUT_Z_L);
  *z = ((zMSB << 8) | zLSB);
}

// ESP32 S3 Wroom specifikus bekötés:
// VCC → 3.3V
// GND → GND  
// SDA → GPIO21 (vagy egyéb választott SDA pin)
// SCL → GPIO22 (vagy egyéb választott SCL pin)