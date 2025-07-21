#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <SPI.h>

// SPI lábkiosztás
#define BNO08X_CS    35
#define BNO08X_INT    36
#define BNO08X_RESET  48

#define SPI_SCK     15
#define SPI_MISO    16
#define SPI_MOSI    14

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setReports() {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    Serial.println("Could not enable game vector");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // SPI inicializálása a megadott lábakkal
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, BNO08X_CS);

  Serial.println("Adafruit BNO08x test (SPI mode)");

  // Szenzor inicializálása SPI-n keresztül
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) delay(10);
  }
  Serial.println("BNO08x Found!");

  for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
    Serial.print("Part ");
    Serial.print(bno08x.prodIds.entry[n].swPartNumber);
    Serial.print(": Version ");
    Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
    Serial.print(" Build ");
    Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }

  setReports();
  Serial.println("Reading events...");
  delay(100);
}

void loop() {
  delay(10);

  if (bno08x.wasReset()) {
    Serial.println("Sensor was reset");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
    Serial.print("Game Rotation Vector - r: ");
    Serial.print(sensorValue.un.gameRotationVector.real);
    Serial.print(" i: ");
    Serial.print(sensorValue.un.gameRotationVector.i, 4);
    Serial.print(" j: ");
    Serial.print(sensorValue.un.gameRotationVector.j, 4);
    Serial.print(" k: ");
    Serial.println(sensorValue.un.gameRotationVector.k, 4);
    bno08x.wasReset();
  }
}
