// File: led.cpp
#include "LED.h"
#include <Adafruit_NeoPixel.h>
#include <Constans.h>



Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void Led::begin() {
  strip.begin();
  strip.show(); // Inicializálja a szalagot 'off' állapotba
}

void Led::off() {
  setColor(0, 0, 0);
}

void Led::setColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.fill(strip.Color(r, g, b));
  strip.show();
}

void Led::handleColor(uint8_t colorCode) {
    switch (colorCode) {
    case 0:  // 0-ás kóddal jelöljük a kikapcsolást
        off();
        break;
    case 1:  // Red
        setColor(255, 0, 0);
        break;
    case 2:  // Yellow
        setColor(255, 255, 0);
        break;
    case 3:  // Green
        setColor(0, 255, 0);
        break;
    case 4:  // Blue
        setColor(0, 0, 255);
        break;
    case 5:  // Purple
        setColor(128, 0, 128);
        break;
    case 6:  // Unknown - jelöljük fehér fénnyel
        setColor(128, 128, 128);
        break;
    default: // Bármilyen más ismeretlen kód esetén is kapcsoljon ki
        off();
        break;
    }
}