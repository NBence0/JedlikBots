#ifndef HC154_H
#define HC154_H

#include <Arduino.h>

class HC154 {
  private:
    int pinA;
    int pinB;
    int pinC;
    int pinD;

  public:
    // Konstruktor
    HC154(int a, int b, int c, int d);

    // Csatorna kiválasztása (0..15)
    void selectChannel(uint8_t channel);
};

#endif
