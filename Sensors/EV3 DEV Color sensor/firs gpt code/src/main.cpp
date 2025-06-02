#include <Arduino.h>

// RGB LED lábak (PWM képes)
#define BLUE_PIN  D1
#define GREEN_PIN D2
#define RED_PIN   D3

// Egyedi fényerősség minden LED-hez
#define RED_BRIGHTNESS   125
#define GREEN_BRIGHTNESS 90    // Zöld gyengébben
#define BLUE_BRIGHTNESS  180   // Kék erősebben

#define SAMPLES 100
#define LED_ON_TIME 5     // ms
#define LED_OFF_TIME 5    // ms

// LED vezérlés + mérések
int measureColor(uint8_t ledPin, uint8_t brightness) {
  int sum = 0;

  for (int i = 0; i < SAMPLES; i++) {
    analogWrite(ledPin, brightness);
    delayMicroseconds(LED_ON_TIME * 1000);

    sum += analogRead(A0);

    analogWrite(ledPin, 0);
    delayMicroseconds(LED_OFF_TIME * 1000);
  }

  return sum / SAMPLES;
}

void setup() {
  Serial.begin(115200);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  analogWriteRange(255);
  analogWriteFreq(1000); // 1 kHz PWM

  Serial.println("Gyors színmérés indul...");
}

void loop() {
  // RGB értékek átlagolása külön fényerővel
  int redVal   = measureColor(RED_PIN, RED_BRIGHTNESS);
  int greenVal = measureColor(GREEN_PIN, GREEN_BRIGHTNESS);
  int blueVal  = measureColor(BLUE_PIN, BLUE_BRIGHTNESS);

  // Szín meghatározás (egyszerű)
  String color = "Ismeretlen";
  if (redVal > greenVal && redVal > blueVal) {
    color = "Piros";
  } else if (greenVal > redVal && greenVal > blueVal) {
    color = "Zöld";
  } else if (blueVal > redVal && blueVal > greenVal) {
    color = "Kék";
  } else if (redVal < 50 && greenVal < 50 && blueVal < 50) {
    color = "Fekete";
  } else if (redVal > 800 && greenVal > 800 && blueVal > 800) {
    color = "Fehér";
  }

  // Eredmény kiírás
  Serial.printf("Red: %d, Green: %d, Blue: %d => Szín: %s\n", redVal, greenVal, blueVal, color.c_str());

  // Kicsi szünet két mérés között
  delay(100);
}
