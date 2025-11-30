#pragma once
#include <Arduino.h>

class Demux {
public:
    Demux(uint8_t A0Pin, uint8_t A1Pin, uint8_t A2Pin, uint8_t A3Pin);
    void begin();
    void DM_selectChannel(uint8_t ch);
    void writeDemultiplexer(byte channel, int state);

private:
    int _A0Pin, _A1Pin, _A2Pin, _A3Pin;
};