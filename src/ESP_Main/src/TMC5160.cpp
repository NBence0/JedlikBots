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

bool TMC5160::begin(uint8_t en_pin) {
    pinMode(en_pin, OUTPUT);
    digitalWrite(en_pin, HIGH);
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
    digitalWrite(en_pin, LOW);
    return true;
}

void TMC5160::disable_motor(uint8_t en_pin) {
    digitalWrite(en_pin, HIGH); // Disable motor driver
}

void TMC5160::stop_motor(uint16_t deceleration) {
    if (deceleration == 0) {
        driver.VMAX(0);
        driver.RAMPMODE(3);
    } else {
        driver.DMAX(deceleration);
        driver.RAMPMODE(1);
        driver.VMAX(0);
    }
}

void TMC5160::rotate_motor(bool direction, uint32_t speed, uint16_t axeleration) {
    driver.AMAX(axeleration);
    if (direction) {
        driver.RAMPMODE(1);
    } else {
        driver.RAMPMODE(2);
    }
    driver.VMAX(speed);
}


uint16_t TMC5160::get_stall_result() {
    return driver.sg_result(); 
}

void TMC5160::set_stallguard(int8_t sensitivity) {
    driver.en_pwm_mode(0); 

    driver.sgt(sensitivity);

    driver.TCOOLTHRS(0xFFFFF); 
}

// még szar
void TMC5160::rotate_steps(bool direction, uint32_t steps, uint32_t speed, uint16_t acceleration, uint16_t deceleration, bool wait) {
    driver.RAMPMODE(0);
    driver.AMAX(acceleration);
    driver.DMAX(deceleration);
    driver.VMAX(speed);
    
    driver.VSTART(0);
    driver.VSTOP(10); 

    int32_t current_pos = driver.XACTUAL();
    
    if (direction) {
        driver.XTARGET(current_pos + steps);
    } else {
        driver.XTARGET(current_pos - steps);
    }

    if (wait) {
        delay(5);
        
        while (!driver.position_reached()) {
            delay(1);
        }
    }
}

bool TMC5160::ismoving() {
    return !driver.position_reached();
}

