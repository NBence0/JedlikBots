#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define LED_PIN     38
#define LED_COUNT   8      // LED-ek száma

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int waveOffset = 0;

void setup() {
  strip.begin();
  delay(500);
  strip.show();
  delay(500);
}


void colorplay(int r, int g, int b) {
    for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b)); // piros
    strip.show();
    delay(100);
  }
}

void led(int r, int g, int b) {
    for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b)); // piros
    strip.show();
  }
}

uint32_t Wheel(byte pos) {
  if (pos < 85) {
    return strip.Color(pos * 3, 255 - pos * 3, 0);
  } else if (pos < 170) {
    pos -= 85;
    return strip.Color(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
}

void rainbowCycle() {
  int delayval = 1;
  for (int i = 0; i < 256; i++) {
    led(255, i, 0);       // piros → sárga
    delay(delayval);
  }
  for (int i = 0; i < 256; i++) {
    led(255 - i, 255, 0); // sárga → zöld
    delay(delayval);
  }
  for (int i = 0; i < 256; i++) {
    led(0, 255, i);       // zöld → cián
    delay(delayval);
  }
  for (int i = 0; i < 256; i++) {
    led(0, 255 - i, 255); // cián → kék
    delay(delayval);
  }
  for (int i = 0; i < 256; i++) {
    led(i, 0, 255);       // kék → magenta
    delay(delayval);
  }
  for (int i = 0; i < 256; i++) {
    led(255, 0, 255 - i); // magenta → piros
    delay(delayval);
  }
}

void loop() {

  colorplay(255, 0, 0);
  colorplay(0, 255, 0);
  colorplay(0, 0, 255);
  colorplay(255, 255, 0);
  colorplay(255, 0, 255);
  colorplay(0, 255, 255);
  colorplay(255, 255, 255);

  for (int i =0; i < 100; i++) {
    for (int i = 0; i < LED_COUNT; i++) {
      int pos = (i * 256 / LED_COUNT + waveOffset) % 256;
      strip.setPixelColor(i, Wheel(pos));
    }

    strip.show();
    waveOffset = (waveOffset + 5) % 256;
    delay(60);
  }
  strip.clear();
  strip.setPixelColor(1, strip.Color(255, 0, 0));
  strip.show();
  delay(500);
  strip.setPixelColor(2, strip.Color(255, 255, 255));
  strip.show();
  delay(500);
  strip.setPixelColor(3, strip.Color(0, 255, 0));
  strip.show();
  strip.setPixelColor(4, strip.Color(255, 0, 0));
  strip.show();
  delay(500);
  strip.setPixelColor(5, strip.Color(255, 255, 255));
  strip.show();
  delay(500);
  strip.setPixelColor(6, strip.Color(0, 255, 0));
  strip.show();
  strip.setPixelColor(7, strip.Color(255, 0, 0));
  strip.show();
  delay(500);
  strip.setPixelColor(8, strip.Color(255, 255, 255));
  delay(4000);
  strip.clear();
  rainbowCycle();
  }

