#include <Arduino.h>

// --- ESP32-S3 BEMENETEK (Címzés) ---
// Az S3-on ezek BIZTONSÁGOSAK.
const int pinA0 = 10; // LSB
const int pinA1 = 11;
const int pinA2 = 12;
const int pinA3 = 13; // MSB

// --- KIMENETI LÁBAK TÖMBJE ---
int outputPins[16];

// Működési mód
// Ha 74HC154-et szimulálsz (eredeti IC): true  (A kiválasztott 0V, többi 3.3V)
// Ha LED-et hajtasz meg közvetlenül:     false (A kiválasztott 3.3V, többi 0V)
bool activeLowMode = true; 

void setup() {
  // S3-nál az USB Serial néha máshogy viselkedik, de a 115200 oké
  Serial.begin(115200);
  
  // Bemenetek
  pinMode(pinA0, INPUT_PULLDOWN);
  pinMode(pinA1, INPUT_PULLDOWN);
  pinMode(pinA2, INPUT_PULLDOWN);
  pinMode(pinA3, INPUT_PULLDOWN);

  // --- LÁBAK KONFIGURÁLÁSA ---
  // 1. Minden csatornát alaphelyzetben -1-re (kikapcsoltra) állítunk
  for(int i=0; i<16; i++) outputPins[i] = -1;

  // 2. Beállítjuk az általad kért lábakat
  // "kimenetnek meg 15, 16, 17, 18, 8 az első öt a sorban"
  outputPins[0] = 15;
  outputPins[1] = 16;
  outputPins[2] = 17;
  outputPins[3] = 18;
  outputPins[4] = 8;   // S3-on a GPIO 8 biztonságos!

  outputPins[15] = 9; 

  // 3. Fizikai beállítás
  for (int i = 0; i < 16; i++) {
    if (outputPins[i] != -1) {
      pinMode(outputPins[i], OUTPUT);
      // Alapállapot beállítása
      digitalWrite(outputPins[i], activeLowMode ? HIGH : LOW);
    }
  }
}

int lastChannel = -2;

void loop() {
  // Bemenetek olvasása
  int bit0 = digitalRead(pinA0);
  int bit1 = digitalRead(pinA1);
  int bit2 = digitalRead(pinA2);
  int bit3 = digitalRead(pinA3);

  // Cím kiszámítása
  int currentChannel = (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;

  if (currentChannel != lastChannel) {
    
    // Serial monitoron láthatod mi történik
    // (Ha az USB CDC on Boot be van kapcsolva az Arduino menüben)
    // Serial.printf("Cim: %d -> CH: %d\n", currentChannel);

    for (int i = 0; i < 16; i++) {
      if (outputPins[i] != -1) {
        if (i == currentChannel) {
          // AKTÍV
          digitalWrite(outputPins[i], activeLowMode ? LOW : HIGH);
        } else {
          // INAKTÍV
          digitalWrite(outputPins[i], activeLowMode ? HIGH : LOW);
        }
      }
    }
    lastChannel = currentChannel;
  }
}