#include <Arduino.h>
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
    digitalWrite(_A0Pin, ch & 1); // 0001
    digitalWrite(_A1Pin, ch & 2); // 0010
    digitalWrite(_A2Pin, ch & 4); // 0100
    digitalWrite(_A3Pin, ch & 8); // 1000
}

void Demux::writeDemultiplexer(byte channel, int state) {
    if (state == HIGH) {
        DM_selectChannel(15);

    } else {
        DM_selectChannel(channel);
    }
}