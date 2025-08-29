#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
public:
    void handleColor(uint8_t colorCode);

private:
    void led(int r, int g, int b);
};

#endif
