#pragma once
#include <Robot.h>
#include <Arduino.h>
#include <74HC154.h>
#include <BNO085.h>
#include <hardware_pins.h>
#include <TMC5160.h>
#include <SPI.h>


class Robot {
public:
    TMC5160 _L_Motor;
    TMC5160 _R_Motor;
    BNO085 _bno;
    Robot();
    void begin();
    void ForwawrdCmWithGyro(bool dir, int cm, float speed, float target_angle, int acceleration);

private:
};