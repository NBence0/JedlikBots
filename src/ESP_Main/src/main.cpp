#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <SPI.h>


Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);
BNO085 bno(BNO_INT, 0, BNO_RST);
TMC5160 tmc1(1);

void setup() {
  Serial.begin(115200);
  SPI.begin(Sensor_SPI_SCL, Sensor_SPI_MISO, Sensor_SPI_MOSI);
  dm.begin();
  tmc1.begin();
  Serial.println("Megy");
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
  static uint32_t last_time = 0;
  static bool dir = true;

  if (millis() - last_time > 5000) {
    last_time = millis();
    dir = !dir;
    if (dir) {
      tmc1.driver.RAMPMODE(1);   // előre
      tmc1.driver.VMAX(60000);
      Serial.println("Előre forog");
    } else {
      tmc1.driver.RAMPMODE(2);   // hátra
      tmc1.driver.VMAX(60000);
      Serial.println("Hátra forog");
    }
  }

}