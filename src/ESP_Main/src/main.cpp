#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <SPI.h>
#include <Robot.h>

Robot robot;
void setup() {
  robot.begin();
  delay(100);
}

void loop() { 
  //robot._L_Motor.rotate_motor(true, 30000);
  //robot._R_Motor.rotate_motor(false, 30000);

}

/*  static uint32_t last_time = 0;
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
    
  
  
  Serial.println("Megy");
  if (bno.begin(&SPI)) {
    Serial.println("Sikeres szenzorinicializáció");
    bno.enableRotation();

  } else {
    Serial.println("Sikertelen inicializáció");
  }
  
  

  bno.update();
  if (bno.hasNewData()) {
  float z = bno.getYaw();
  float x = bno.getRoll();
  float y = bno.getPitch();
  Serial.println(z);
  Serial.println(x);
  Serial.println(y);
  delay(100);
  }


  */