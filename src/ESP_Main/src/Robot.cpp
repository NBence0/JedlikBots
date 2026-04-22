#include <Robot.h>
#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <ESP32Servo.h>
#include <SPI.h>

Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);
Servo myServo;

Robot::Robot():
    _L_Motor(8),
    _R_Motor(9),
    _bno(),
    _bme(BME280_CS)
{
    
};

void Robot::begin() {
    SPI.begin(Sensor_SPI_SCL, Sensor_SPI_MISO, Sensor_SPI_MOSI);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(47, OUTPUT);
    pinMode(21, OUTPUT);
    pinMode(48, OUTPUT);
    Wire.begin(12,11);
    _L_Motor.begin(21);
    _R_Motor.begin(48);
    //driver.shaft(true); 
    dm.begin();
    Serial.begin(115200);
    
    if (_bno.begin()) {
    Serial.println("Sikeres szenzorinicializáció");
    _bno.enableSensors();
    } else {
    Serial.println("Sikertelen inicializáció");
    }
}

void InitServo(uint8_t s_pin) {
    myServo.setPeriodHertz(50); 
	myServo.attach(s_pin);
}


void MoveServo(uint8_t targetPosition, uint8_t delayTime) {
    int currentPos = myServo.read(); 

    if (currentPos < targetPosition) {
        for (int pos = currentPos; pos <= targetPosition; pos += 1) {
            myServo.write(pos);
            delay(delayTime);
        }
    } 
    else if (currentPos > targetPosition) {
        for (int pos = currentPos; pos >= targetPosition; pos -= 1) {
            myServo.write(pos);
            delay(delayTime);
        }
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


void Robot::TurnToAngle(float target_angle, uint32_t max_speed, uint16_t acceleration) {
    bool reached = false;
    const float TOLERANCE = 0.5; // Fél fok pontosságnál már jónak veszi
    const float KP = 2500.0; // P-szabályozó szorzója (finomhangolni kellhet a sebességedhez mérten!)
    uint32_t min_speed = 10000; // Minimum sebesség, hogy ne akadjon meg a legvégén

    while (!reached) {
        _bno.update();
        float current_yaw = _bno.getYaw();
        float error = target_angle - current_yaw;

        if (abs(error) <= TOLERANCE) {
            reached = true;
            break;
        }
        uint32_t current_speed = abs(error) * KP;
        if (current_speed > max_speed) current_speed = max_speed;
        if (current_speed < min_speed) current_speed = min_speed;

        if (error > 0) { 
            _L_Motor.rotate_motor(false, current_speed, acceleration);
            _R_Motor.rotate_motor(true, current_speed, acceleration);
        } else {
            _L_Motor.rotate_motor(true, current_speed, acceleration);
            _R_Motor.rotate_motor(false, current_speed, acceleration);
        }
        
        delay(10);
    }

    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}

void Robot::TurnWithOneWheel(bool use_left_wheel, bool forward, float target_angle, uint32_t max_speed, uint16_t acceleration) {
    bool reached = false;
    const float TOLERANCE = 0.5;
    
    // Annak a keréknek, amit nem használunk, biztosan állnia kell
    if (use_left_wheel) {
        _R_Motor.stop_motor();
    } else {
        _L_Motor.stop_motor();
    }

    while (!reached) {
        _bno.update();
        float current_yaw = _bno.getYaw();

        float error = target_angle - current_yaw;
        
        // Ez egy egyszerűsített ellenőrzés. Ha a tolerancia alá ér, kész.
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

    // Leállítjuk a hajtott kereket is
    if (use_left_wheel) {
        _L_Motor.stop_motor();
    } else {
        _R_Motor.stop_motor();
    }
}

void Robot::MoveUntilHit(bool dir, uint32_t speed, int8_t sensitivity) {
    Serial.println("Indulas a falnak...");
    
    // 1. StallGuard aktiválása (ezzel a motor picit hangosabbá válik, ez normális!)
    _L_Motor.set_stallguard(sensitivity);
    _R_Motor.set_stallguard(sensitivity);

    // 2. Elindítjuk a motorokat
    _L_Motor.rotate_motor(dir, speed, 2000);
    _R_Motor.rotate_motor(dir, speed, 2000);

    // VÁRUNK, amíg a motor teljesen felgyorsul. Amíg gyorsul, nem mérünk!
    delay(500); 

    bool hit = false;
    unsigned long start_time = millis();

    while (!hit) {
        uint16_t sgL = _L_Motor.get_stall_result();
        uint16_t sgR = _R_Motor.get_stall_result();

        // Ideális esetben üresjáratban 200-800 közötti számokat kell látnod.
        Serial.printf("SG_L: %d, SG_R: %d\n", sgL, sgR);

        // Ha a sebesség stabil, és az érték 0-ra esik, akkor falat értünk!
        if (sgL == 0 || sgR == 0) {
            Serial.println("UTKOZES DETEKTALVA! A robot megakadt.");
            hit = true;
        }

        // Biztonsági timeout 15 másodperc
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
    while (touchRead(touch_pin) < 50000) {
        delay(50);
    }
    
    Serial.println("Erintes erzekelve! Indulas...");
    delay(500); 
}