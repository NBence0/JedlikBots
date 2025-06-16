#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>

class LED {
public:
    LED(int pin, int count);
    void begin();
    void colorplay(int r, int g, int b);

private:
    Adafruit_NeoPixel strip;
};

#endif
