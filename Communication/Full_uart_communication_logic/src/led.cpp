#include "LED.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN     38
#define LED_COUNT   8

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void Led::led(int r, int g, int b) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void Led::handleColor(uint8_t colorCode) {
    switch (colorCode) {
    case 1:
        led(255, 0, 0);
        Serial.println("piros");
        break;
    case 2:
        led(255, 255, 0);
        Serial.println("sárga");
        break;
    case 3:
        led(0, 255, 0);
        Serial.println("zöld");
        break;
    case 4:
        led(0, 0, 255);
        Serial.println("kék");
        break;
    case 5:
        led(255, 0, 255);
        Serial.println("lila");
        break;
    default:
        Serial.println("Ismeretlen szín");
        break;
    }
}
