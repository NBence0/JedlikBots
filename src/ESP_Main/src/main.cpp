#include <Arduino.h>
#include <Robot.h>

Robot robot;
void setup() {
  robot.begin();
  delay(100);
}

void loop() { 
  
  //robot.ForwawrdCmWithGyro(true, 100, 50000, 0.0, 1000);
  //robot.TurnToAngle(90,1000, 200);
  //robot._bno.resetYaw();
  robot.ForwawrdCmWithGyro(false, 50, 50000, 0.0, 1000);
  robot.WaitUntilTouch(13);
  robot.MoveUntilHit(true, 50000, 10); // Kezdjünk egy enyhén pozitív SGT értékkel (pl. 5)
  //robot._L_Motor.rotate_motor(true,1000,1000);
  //robot._R_Motor.rotate_motor(true,1000,1000);
  delay(1000);
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