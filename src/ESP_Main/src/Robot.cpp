#include <Robot.h>
#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <ESP32Servo.h>
#include <SPI.h>

Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);

Robot::Robot():
    _L_Motor(9),
    _R_Motor(8),
    _bno(),
    _bme(BME280_CS),
    lecsuko()
{
    
};

void Robot::begin() {
    SPI.begin(Sensor_SPI_SCL, Sensor_SPI_MISO, Sensor_SPI_MOSI);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(47, OUTPUT);
    pinMode(21, OUTPUT);
    pinMode(48, OUTPUT);
    pinMode(10, OUTPUT);
    Wire.begin(12,11);
    _L_Motor.begin(48);
    _R_Motor.begin(21);
    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
    //driver.shaft(true); 
    dm.begin();
    Serial.begin(115200);
    
    if (_bno.begin()) {
        Serial.println("Sikeres szenzorinicializáció");
        _bno.enableSensors();
    } else {
        Serial.println("Sikertelen inicializáció");
    }
    lecsuko.begin(10);
}




void Robot::ForwawrdCmWithGyro(bool dir, float cm, float base_speed, float target_angle, int acceleration) {
    const float WHEEL_DIAMETER_CM = 6.04;
    const float cm_in_steps = (200.0 * 256.0) / (WHEEL_DIAMETER_CM * 3.1415); 
    int32_t target_steps_diff = cm_in_steps * cm;

    float KP = base_speed * 0.05; 
    
    int32_t start_pos_L = _L_Motor.driver.XACTUAL();
    int32_t start_pos_R = _R_Motor.driver.XACTUAL();

    int32_t target_pos_L = dir ? (start_pos_L + target_steps_diff) : (start_pos_L - target_steps_diff);
    int32_t target_pos_R = dir ? (start_pos_R + target_steps_diff) : (start_pos_R - target_steps_diff);

    _L_Motor.driver.RAMPMODE(0);
    _R_Motor.driver.RAMPMODE(0);

    _L_Motor.driver.AMAX(acceleration);
    _L_Motor.driver.DMAX(acceleration); 
    _R_Motor.driver.AMAX(acceleration);
    _R_Motor.driver.DMAX(acceleration);

    _L_Motor.driver.VMAX(base_speed);
    _R_Motor.driver.VMAX(base_speed);

    _L_Motor.driver.XTARGET(target_pos_L);
    _R_Motor.driver.XTARGET(target_pos_R);

    while (!_L_Motor.driver.position_reached() || !_R_Motor.driver.position_reached()) {
        _bno.update();
        float current_yaw = _bno.getYaw();
        float error = target_angle - current_yaw;

        while (error > 180.0) error -= 360.0;
        while (error < -180.0) error += 360.0;

        float correction = error * KP;
        
        int32_t speedL = base_speed;
        int32_t speedR = base_speed;

        if (dir) {
            speedL += correction;
            speedR -= correction;
        } else {
            speedL -= correction;
            speedR += correction;
        }

        if (speedL < 1000) speedL = 1000;
        if (speedR < 1000) speedR = 1000;
        if (speedL > base_speed * 1.5) speedL = base_speed * 1.5;
        if (speedR > base_speed * 1.5) speedR = base_speed * 1.5;

        _L_Motor.set_speed(speedL);
        _R_Motor.set_speed(speedR);

        delay(5); 
    }
    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}


void Robot::TurnToAngle(float target_angle, uint32_t max_speed, uint16_t acceleration) {
    _bno.update();
    float current_yaw = _bno.getYaw();
    float error = target_angle - current_yaw;

    while (error > 180.0) error -= 360.0;
    while (error < -180.0) error += 360.0;

    if (abs(error) <= 0.5) return;

    bool turning_positive = (error > 0);

    if (turning_positive) {
        _L_Motor.rotate_motor(true, max_speed, acceleration);
        _R_Motor.rotate_motor(false, max_speed, acceleration);
    } else {
        _L_Motor.rotate_motor(false, max_speed, acceleration);
        _R_Motor.rotate_motor(true, max_speed, acceleration);
    }

    while (true) {
        _bno.update();
        current_yaw = _bno.getYaw();
        error = target_angle - current_yaw;

        while (error > 180.0) error -= 360.0;
        while (error < -180.0) error += 360.0;

        if (turning_positive) {
            if (error <= 0.0) break;
        } else {
            if (error >= 0.0) break;
        }

        delay(10); // 10ms várakozás
    }

    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}


void Robot::TurnWithOneWheel(bool use_left_wheel, bool forward, float target_angle, uint32_t max_speed, uint16_t acceleration) {
    bool reached = false;
    const float TOLERANCE = 0.5;
    
    if (use_left_wheel) {
        _R_Motor.stop_motor();
    } else {
        _L_Motor.stop_motor();
    }

    while (!reached) {
        _bno.update();
        float current_yaw = _bno.getYaw();

        float error = target_angle - current_yaw;
        
        if (abs(error) <= TOLERANCE) {
            reached = true;
            break;
        }

        if (use_left_wheel) {
            _L_Motor.rotate_motor(forward, max_speed, acceleration);
        } else {
            _R_Motor.rotate_motor(forward, max_speed, acceleration);
        }

        delay(10);
    }

    if (use_left_wheel) {
        _L_Motor.stop_motor();
    } else {
        _R_Motor.stop_motor();
    }
}

void Robot::MoveUntilHit(bool dir, uint32_t speed, int8_t sensitivity) {
    Serial.println("Indulas a falnak...");
    
    _L_Motor.set_stallguard(sensitivity);
    _R_Motor.set_stallguard(sensitivity);

    _L_Motor.rotate_motor(dir, speed, 2000);
    _R_Motor.rotate_motor(dir, speed, 2000);

    delay(500); 

    bool hit = false;
    unsigned long start_time = millis();

    while (!hit) {
        uint16_t sgL = _L_Motor.get_stall_result();
        uint16_t sgR = _R_Motor.get_stall_result();

        Serial.printf("SG_L: %d, SG_R: %d\n", sgL, sgR);

        if (sgL == 0 || sgR == 0) {
            Serial.println("UTKOZES DETEKTALVA! A robot megakadt.");
            hit = true;
        }

        if (millis() - start_time > 15000) {
            Serial.println("Timeout: Nem talaltam falat.");
            break;
        }
        
        delay(20);
    }

    _L_Motor.stop_motor();
    _R_Motor.stop_motor();

    _L_Motor.driver.en_pwm_mode(1);
    _R_Motor.driver.en_pwm_mode(1);

    _L_Motor.driver.TCOOLTHRS(0);
    _R_Motor.driver.TCOOLTHRS(0);
}

void Robot::WaitUntilTouch(uint8_t touch_pin) {
    Serial.println("Varakozas erintesre...");
    while (touchRead(touch_pin) < 30000) {
        float current_yaw = _bno.getYaw();
        Serial.println("Current Yaw: " + String(current_yaw));
        //Serial.println(touchRead(touch_pin));
        delay(50);
    }
    Serial.println("Erintes erzekelve! Indulas...");
    delay(500); 
}