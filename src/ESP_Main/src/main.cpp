#include <Arduino.h>
#include <74HC154.h>
#include <hardware_pins.h>

Demux dm(DM_A0pin, DM_A1pin, DM_A2pin, DM_A3pin);

void setup() {
  dm.begin();
}

void loop() {
}