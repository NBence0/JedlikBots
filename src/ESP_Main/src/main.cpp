#include <Arduino.h>
#include <Robot.h>

Robot robot;
void setup() {
  robot.begin();
  robot._L_Motor.stop_motor();
  robot._R_Motor.stop_motor();
  robot._bno.resetYaw();
  delay(100);
//}

//void loop() { 
  robot.WaitUntilTouch(13);
  robot._bno.resetYaw();
  robot.ForwawrdCmWithGyro(true, 45, 200000, 0.0, 2000);
  robot.lecsuko.moveTo(50, 7);
  robot.ForwawrdCmWithGyro(false, 32, 200000, 0.0, 2000);
  robot.WaitUntilTouch(13);
  robot.ForwawrdCmWithGyro(true, 20, 200000, 0.0, 2000);
  // robot.WaitUntilTouch(13);
  // robot.lecsuko.moveTo(160, 7); //nem kell maxra emelni
  robot.TurnDegrees(-20, 100000, 2000);  // +30 fok
  //robot.WaitUntilTouch(13);
  //robot.lecsuko.moveTo(50, 7);
  robot.TurnDegrees(15, 100000, 2000); // 30→10 fok: -20 relatív
  robot.WaitUntilTouch(13);
  robot.ForwawrdCmWithGyro(true, 30, 200000, 20, 2000);
  robot.ForwawrdCmWithGyro(true, 35, 200000, 10, 2000);
  robot.WaitUntilTouch(13);
  // kar fel
  robot.TurnDegrees(18, 100000, 2000); // 10→0 fok: -10 relatív
  robot.WaitUntilTouch(13);
  robot.ForwawrdCmWithGyro(true, 20, 200000, 10, 2000);
  //kar le
  robot.WaitUntilTouch(13);

  robot.TurnDegrees(-14, 100000, 2000); // 10→0 fok: -10 relatív
  robot.WaitUntilTouch(13);
  robot.ForwawrdCmWithGyro(true, 40, 100000, 0, 2000);
  robot.WaitUntilTouch(13);

  robot.TurnDegrees(-50, 100000, 2000);
  robot.TurnDegrees(10, 100000, 2000);
  robot.WaitUntilTouch(13);

  // kar fel
  robot.TurnDegrees(40, 100000, 2000);
  robot.WaitUntilTouch(13);
  robot.ForwawrdCmWithGyro(false, 30, 100000, 0, 2000);


  // robot.ForwawrdCmWithGyro(false, 26, 100000, 0.0, 2000);
  // robot.WaitUntilTouch(13);
  // robot.ForwawrdCmWithGyro(true, 1, 100000, 0.0, 2000);
  // robot.lecsuko.moveTo(160, 2);
  // robot.lecsuko.moveTo(50, 2);
  // robot.ForwawrdCmWithGyro(false, 30, 100000, 0.0, 2000);
  // robot.WaitUntilTouch(13);
  // robot.ForwawrdCmWithGyro(true, 100, 100000, 0.0, 2000);



  
  //robot.TurnToAngle(90, 50000, 1000);
  //robot.TurnToAngle(-90, 50000, 1000);
  //robot.ForwawrdCmWithGyro(false, 30, 100000, 0.0, 1000);
  //robot.lecsuko.moveTo(120, 10);
  // robot.WaitUntilTouch(13);
  // robot.ForwawrdCmWithGyro(true, 50, 50000, 0.0, 1000);
  // robot.WaitUntilTouch(13);
  // robot.WaitUntilTouch(13);
  // robot.ForwawrdCmWithGyro(true, 50, 50000, 90, 1000);
  // robot.WaitUntilTouch(13);
  // robot.TurnToAngle(0, 50000, 1000);
  // robot.WaitUntilTouch(13);
  // robot.ForwawrdCmWithGyro(false, 50, 50000, 0.0, 1000);
  //robot.MoveUntilHit(true, 50000, 10);
  //robot._L_Motor.rotate_motor(true,1000,1000);
  //robot._R_Motor.rotate_motor(true,1000,1000);
  //delay(1000);
  //robot._L_Motor.rotate_motor(true, 30000);
  //robot._R_Motor.rotate_motor(false, 30000);

}


void loop() { 
  delay(1000);
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