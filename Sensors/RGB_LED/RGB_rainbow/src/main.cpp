#include <Arduino.h>

const int LED_RED = 3;
const int LED_GREEN = 5;
const int LED_BLUE = 6;

void led(int r, int g, int b);

int i = 0;

void setup() {
  // LED kimenetek inicializálása
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  Serial.begin(9600);
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
    rainbowCycle();
}

void led(int r, int g, int b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}
