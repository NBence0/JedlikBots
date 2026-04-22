#pragma once
#include <Robot.h>
#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <SPI.h>
#include <BME280.h>

class Robot {
public:
    TMC5160 _L_Motor;
    TMC5160 _R_Motor;
    BNO085 _bno;
    BME280 _bme;
    Robot();
    void begin();
    void ForwawrdCmWithGyro(bool dir, int cm, float speed, float target_angle, int acceleration);
    void InitServo(uint8_t servo_pin);
    void MoveServo(uint8_t position, uint8_t delay);
    void TurnToAngle(float target_angle, uint32_t max_speed, uint16_t acceleration);
    void TurnWithOneWheel(bool use_left_wheel, bool forward, float target_angle, uint32_t max_speed, uint16_t acceleration);
    void MoveUntilHit(bool dir, uint32_t speed, int8_t sensitivity);
    void WaitUntilTouch(uint8_t touch_pin);
private:
};