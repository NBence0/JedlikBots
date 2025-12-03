#pragma once
#include <Arduino.h>
#include <setup.h>
#include <hardware_pins.h>
#include <TMCStepper.h>

class TMC5160 {
public:
    TMC5160Stepper driver;
    TMC5160(uint8_t cs_pin, 
            float r_sense = TMC_R_SENSE, 
            uint8_t toff = TMC_TOFF, 
            uint8_t blank_time = TMC_BLANK_TIME,
            uint8_t rampmode = TMC_RAMPMODE,
            int microstep = TMC_MICROSTEP,
            uint32_t amax = TMC_AMAX,
            uint32_t dmax = TMC_DMAX,
            uint32_t current = TMC_RMS_CURRENT,
            int vmax = TMC_VMAX,
            bool pwm_mode = TMC_EN_PWM_MODE,
            bool pwm_autoscale = TMC_PWM_AUTOSCALE);
    
    bool begin();

private:
    uint32_t _amax, _dmax, _current;
    int _vmax, _microstep;
    uint8_t _cs_pin, _toff, _blank_time, _rampmode;
    bool _pwm_mode, _pwm_autoscale;
    float _r_sense;

};