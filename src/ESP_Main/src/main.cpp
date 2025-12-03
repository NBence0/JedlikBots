#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <SPI.h>



Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);
BNO085 bno(BNO_INT, 0, BNO_RST);

void setup() {
  Serial.begin(115200);
  dm.begin();
  Serial.println("Megy");
  SPI.begin(Sensor_SPI_SCL, Sensor_SPI_MISO, Sensor_SPI_MOSI);
  if (bno.begin(&SPI)) {
    Serial.println("Sikeres szenzorinicializáció");
    bno.enableRotation();

  } else {
    Serial.println("Sikertelen inicializáció");
  }
}

void loop() { 
  bno.update();
  if (bno.hasNewData()) {
  float z = bno.getYaw();
  Serial.println(z);
  delay(2000);
  }
}