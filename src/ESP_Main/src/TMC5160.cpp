#include <TMC5160.h>

TMC5160::TMC5160(uint8_t cs_pin, 
            float r_sense, 
            uint8_t toff, 
            uint8_t blank_time,
            uint8_t rampmode,
            int microstep,
            uint32_t amax,
            uint32_t dmax,
            uint32_t current,
            int vmax,
            bool pwm_mode,
            bool pwm_autoscale)
            :driver(cs_pin, r_sense)
    {
    _toff = toff;
    _blank_time = blank_time;
    _rampmode = rampmode;
    _microstep = microstep;
    _amax = amax;
    _dmax = dmax;
    _current = current;
    _vmax = vmax;
    _pwm_mode = pwm_mode;
    _pwm_autoscale = pwm_autoscale;
}

bool TMC5160::begin() {
    pinMode(TMC_EN, OUTPUT);
    digitalWrite(TMC_EN, HIGH);
    driver.begin();
    driver.toff(_toff);
    driver.blank_time(_blank_time);
    driver.rms_current(_current);
    driver.microsteps(_microstep);

    driver.en_pwm_mode(_pwm_mode);
    driver.pwm_autoscale(_pwm_autoscale);

    driver.RAMPMODE(_rampmode);
    driver.AMAX(_amax);
    driver.DMAX(_dmax);
    driver.VMAX(_vmax);
    digitalWrite(TMC_EN, LOW);
    return true;
}