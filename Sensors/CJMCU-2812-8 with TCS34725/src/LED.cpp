#include "LED.h"

LED::LED(int pin, int count) : strip(count, pin, NEO_GRB + NEO_KHZ800) {}

void LED::begin() {
    strip.begin();
    strip.show();
}

void LED::colorplay(int r, int g, int b) {
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}
