#include <Arduino.h>
#include <hardware_pins.h>
#include <74HC154.h>


Demux::Demux(uint8_t A0Pin, uint8_t A1Pin, uint8_t A2Pin, uint8_t A3Pin) {
    _A0Pin = A0Pin;
    _A1Pin = A1Pin;
    _A2Pin = A2Pin;
    _A3Pin = A3Pin;
}

void Demux::begin() {
    pinMode(_A0Pin, OUTPUT);
    pinMode(_A1Pin, OUTPUT);
    pinMode(_A2Pin, OUTPUT);
    pinMode(_A3Pin, OUTPUT);
}

void Demux::DM_selectChannel(byte ch) {
    digitalWrite(DM_A0pin, ch & 1); // 0001
    digitalWrite(DM_A1pin, ch & 2); // 0010
    digitalWrite(DM_A2pin, ch & 4); // 0100
    digitalWrite(DM_A3pin, ch & 8); // 1000
}

void Demux::writeDemultiplexer(byte channel, int state) {
    if (state == HIGH) {
        DM_selectChannel(DM_DIAG3_LED3);

    } else {
        DM_selectChannel(channel);
    }
}