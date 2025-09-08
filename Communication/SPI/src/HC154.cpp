#include "HC154.h"

// Konstruktor: pinek eltárolása és inicializálása
HC154::HC154(int a, int b, int c, int d) {
  pinA = a;
  pinB = b;
  pinC = c;
  pinD = d;

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
}

// Csatorna kiválasztása
void HC154::selectChannel(uint8_t channel) {
  if (channel > 15) return;
  digitalWrite(pinA, (channel & 0x01) ? HIGH : LOW);
  digitalWrite(pinB, (channel & 0x02) ? HIGH : LOW);
  digitalWrite(pinC, (channel & 0x04) ? HIGH : LOW);
  digitalWrite(pinD, (channel & 0x08) ? HIGH : LOW);
}


/*#include "HC154.h"

HC154 decoder(2, 3, 4, 5);  // A=2, B=3, C=4, D=5

void setup() {
  // Konstruktor automatikusan inicializálja a pineket
}

void loop() {
  for (uint8_t ch = 0; ch < 16; ch++) {
    decoder.selectChannel(ch);
    delay(300);
  }
}
*/