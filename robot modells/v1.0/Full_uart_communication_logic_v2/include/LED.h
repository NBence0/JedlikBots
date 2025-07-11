// File: LED.h
#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
public:
    void begin();
    void handleColor(uint8_t colorCode);
    void off();

private:
    void setColor(uint8_t r, uint8_t g, uint8_t b);
};

#endif