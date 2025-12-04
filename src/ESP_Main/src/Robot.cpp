#include <Robot.h>
#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <SPI.h>

Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);

Robot::Robot():
    _L_Motor(21),
    _R_Motor(48),
    _bno(BNO_INT, 47, BNO_RST),
    _bme(BME280_CS)
{
    
};

void Robot::begin() {
    SPI.begin(Sensor_SPI_SCL, Sensor_SPI_MISO, Sensor_SPI_MOSI);
    pinMode(21, OUTPUT);
    pinMode(47, OUTPUT);
    pinMode(48, OUTPUT);
    _L_Motor.begin();
    _R_Motor.begin();
    //driver.shaft(inverse); 
    dm.begin();
    Serial.begin(115200);
    
    if (_bno.begin(&SPI)) {
    Serial.println("Sikeres szenzorinicializáció");
    _bno.enableRotation();
    } else {
    Serial.println("Sikertelen inicializáció");
    }
}


void Robot::ForwawrdCmWithGyro(bool dir, int cm, float speed, float target_angle, int acceleration) {
    const int CORRECTION_VALUE = 500;
    const float ERROR_VALUE = 0.5;
    const float WHEEL_DIAMETER_CM = 6.5;
    const int cm_in_steps = (200*TMC_MICROSTEP)/(WHEEL_DIAMETER_CM*3.1415);
    int target_steps = cm_in_steps * cm;
    long start_pos = _L_Motor.driver.XACTUAL();
    bool reached = false;
    
    
    while (!reached) {
        float correction = 0;
        _bno.update();
        float current_yaw = _bno.getYaw();
        Serial.println("Current Yaw: " + String(current_yaw));
        float error = target_angle-current_yaw;

        if (abs(error)<= ERROR_VALUE) {
        } else {
                correction = error * CORRECTION_VALUE;
            }
        int32_t speedL, speedR;

        if (dir) {
            speedL = speed + correction;
            speedR = speed - correction;
        } else {
            speedL = speed - correction;
            speedR = speed + correction;
        }

        if (speedL < 0) speedL = 0;
        if (speedR < 0) speedR = 0;

        if (dir) {
            _L_Motor.rotate_motor(true, speedL, acceleration);
            _R_Motor.rotate_motor(true, speedR, acceleration);
        } else {
            _L_Motor.rotate_motor(false, speedL, acceleration);
            _R_Motor.rotate_motor(false, speedR, acceleration);
        }
        long current_dist = abs(_L_Motor.driver.XACTUAL() - start_pos);
        if (current_dist >= abs(target_steps)) {
            reached = true;
        }
        delay(50);
    }
    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}