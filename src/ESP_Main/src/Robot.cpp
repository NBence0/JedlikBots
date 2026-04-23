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




void Robot::ForwawrdCmWithGyro(bool dir, float cm, float speed, float target_angle, int acceleration) {
    const float WHEEL_DIAMETER_CM = 6.04;
    const float cm_in_steps = (200 * 256) / (WHEEL_DIAMETER_CM * 3.1415); 
    int target_steps = cm_in_steps * cm;
    
    // Szabályzó érzékenysége
    float KP = speed * 0.05; 
    
    long start_pos = _L_Motor.driver.XACTUAL();
    bool reached = false;
    
    _L_Motor.rotate_motor(dir, speed, acceleration);
    _R_Motor.rotate_motor(dir, speed, acceleration);
    
    while (!reached) {
        _bno.update();
        float current_yaw = _bno.getYaw();
        float error = target_angle - current_yaw;

        while (error > 180.0) error -= 360.0;
        while (error < -180.0) error += 360.0;

        float correction = error * KP;
        
        int32_t speedL = speed;
        int32_t speedR = speed;

        if (dir) {
            // Előremenet korrekciója
            speedL += correction;
            speedR -= correction;
        } else {
            // HÁTRAMENET KORREKCIÓJA INVERTÁLVA! Itt volt a hiba.
            speedL -= correction;
            speedR += correction;
        }

        if (speedL < 0) speedL = 0;
        if (speedR < 0) speedR = 0;

        _L_Motor.set_speed(speedL);
        _R_Motor.set_speed(speedR);

        long current_dist = abs(_L_Motor.driver.XACTUAL() - start_pos);
        if (current_dist >= abs(target_steps)) {
            reached = true;
        }
        
        delay(10); 
    }
    
    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}


void Robot::TurnToAngle(float target_angle, uint32_t max_speed, uint16_t acceleration) {
    _bno.update();
    float current_yaw = _bno.getYaw();
    float error = target_angle - current_yaw;

    // Kiszámítjuk, hogy merre kell fordulni a legrövidebb úton
    while (error > 180.0) error -= 360.0;
    while (error < -180.0) error += 360.0;

    // Ha véletlenül már pontosan a célon állunk, ne csináljon semmit
    if (abs(error) <= 0.5) return;

    // Eltároljuk az eredeti irányt (pozitív vagy negatív irányba indulunk)
    bool turning_positive = (error > 0);

    // 1. Elindítjuk a motorokat a megfelelő irányba
    // ITT VOLT A HIBA! Felcseréltük a true/false értékeket, hogy egyezzen a gyro irányával!
    if (turning_positive) {
        _L_Motor.rotate_motor(true, max_speed, acceleration);
        _R_Motor.rotate_motor(false, max_speed, acceleration);
    } else {
        _L_Motor.rotate_motor(false, max_speed, acceleration);
        _R_Motor.rotate_motor(true, max_speed, acceleration);
    }

    // 2. Várakozás, amíg el nem érjük a szöget
    while (true) {
        _bno.update();
        current_yaw = _bno.getYaw();
        error = target_angle - current_yaw;

        // Szög normalizálása
        while (error > 180.0) error -= 360.0;
        while (error < -180.0) error += 360.0;

        if (turning_positive) {
            // Ha pozitív irányba fordulunk, és a hiba 0 vagy az alá csökken, megvagyunk.
            if (error <= 0.0) break;
        } else {
            // Ha negatív irányba fordulunk, és a hiba 0 vagy a fölé nő, megvagyunk.
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
    while (touchRead(touch_pin) < 30000) {
        float current_yaw = _bno.getYaw();
        Serial.println("Current Yaw: " + String(current_yaw));
        //Serial.println(touchRead(touch_pin));
        delay(50);
    }
    Serial.println("Erintes erzekelve! Indulas...");
    delay(500); 
}