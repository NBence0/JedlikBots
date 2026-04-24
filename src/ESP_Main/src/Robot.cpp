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
        Serial.println("Sikeres szenzorinicializacio");
    } else {
        Serial.println("Sikertelen inicializacio");
    }
    lecsuko.begin(10);
}




void Robot::ForwawrdCmWithGyro(bool dir, float cm, float speed, float target_angle, int acceleration) {
    const float CORRECTION_VALUE  = 500.0;
    const float ERROR_VALUE       = 0.5;
    const float WHEEL_DIAMETER_CM = 6.5;
    const long  cm_in_steps = (200L * TMC_MICROSTEP) / (WHEEL_DIAMETER_CM * 3.1415);
    long target_steps = cm_in_steps * (long)abs(cm);
    long start_pos    = _L_Motor.driver.XACTUAL();

    // Kompenzáció: a motor a stop_motor() hívása után még halad egy kicsit.
    // Formula újrakalibrálva 100cm mérési adatok alapján:
    //   100000 → 90.5cm, 200000 → 93cm, 300000 → 96.5cm
    // (negatív esetén 0 = nincs kompenzáció)
    long compensation = max(0L, (long)(speed * 0.6145f) - 81516L);
    long stop_at      = (target_steps > compensation) ? (target_steps - compensation) : 0;

    while (abs(_L_Motor.driver.XACTUAL() - start_pos) < stop_at) {
        _bno.update();
        float error = target_angle - _bno.getYaw();
        while (error >  180.0) error -= 360.0;
        while (error < -180.0) error += 360.0;
        float correction = (abs(error) > ERROR_VALUE) ? (error * CORRECTION_VALUE) : 0.0f;

        int32_t sL = (int32_t)(speed + (dir ?  correction : -correction));
        int32_t sR = (int32_t)(speed + (dir ? -correction :  correction));
        if (sL < 0) sL = 0;
        if (sR < 0) sR = 0;

        _L_Motor.rotate_motor(dir, (uint32_t)sL, acceleration);
        _R_Motor.rotate_motor(dir, (uint32_t)sR, acceleration);
        delay(5);
    }

    _L_Motor.stop_motor();
    _R_Motor.stop_motor();
}



void Robot::TurnDegrees(float degrees, uint32_t speed, int acceleration) {
    if (abs(degrees) < 0.5) return; // semmi sem kell ha tul kicsi

    const float WHEEL_DIAMETER_CM = 6.5;
    const float TRACK_WIDTH_CM    = 17.0; // kerékközépponttól kerékközéppontig
    const long  cm_in_steps = (200L * TMC_MICROSTEP) / (WHEEL_DIAMETER_CM * 3.1415);

    // A forduláshoz szükséges ívhossz mindkét keréknek
    float arc_cm    = (abs(degrees) / 360.0f) * 3.1415f * TRACK_WIDTH_CM;
    long  turn_steps = (long)(arc_cm * cm_in_steps);

    Serial.printf("TurnDegrees: %.1f fok, ívhossz=%.2fcm, lepesek=%ld\n",
        degrees, arc_cm, turn_steps);

    long start_pos = _L_Motor.driver.XACTUAL();

    // Pozitív fok → balra fordul (bal hátra, jobb előre)
    // Negatív fok → jobbra fordul (bal előre, jobb hátra)
    bool positive = (degrees > 0);
    if (positive) {
        _L_Motor.rotate_motor(false, speed, acceleration);
        _R_Motor.rotate_motor(true,  speed, acceleration);
    } else {
        _L_Motor.rotate_motor(true,  speed, acceleration);
        _R_Motor.rotate_motor(false, speed, acceleration);
    }

    while (abs(_L_Motor.driver.XACTUAL() - start_pos) < turn_steps) {
        delay(2);
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
        _bno.update(); // Fontos: frissitjuk a szenzort, hogy a yaw valtozzon!
        float current_yaw = _bno.getYaw();
        Serial.println("Current Yaw: " + String(current_yaw));
        delay(50);
    }
    Serial.println("Erintes erzekelve! Indulas...");
    delay(500);
}