#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN     38
#define LED_COUNT   8      // LED-ek száma

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200); // Debug USB
  Serial1.begin(115200, SERIAL_8N1, 20, 21);  // <-- Itt beállíthatod az új UART pineket
  strip.begin();
  delay(500);
  strip.show();
  delay(500); // RX=20, TX=21
}

void led(int r, int g, int b) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.show();
  }
}

void handleColorPacket(uint8_t colorCode) {
  switch (colorCode) {
    case 0x01:  // RED
      led(255, 0, 0);
      break;
    case 0x02:  // GREEN
      led(0, 255, 0);
      break;
    case 0x03:  // BLUE
      led(0, 0, 255);
      break;
    case 0x04:  // YELLOW
      led(255, 255, 0);
      break;
    case 0xFF:  // UNKNOWN
      led(0, 0, 0);
      break;
    default:
      // Ismeretlen kód
      break;
  }
}

void loop() {
  while (Serial1.available()) {
    uint8_t colorCode = Serial1.read();
    Serial.print("Received color code: ");
    Serial.println(colorCode, HEX);
    handleColorPacket(colorCode);
    delay(100); // Kis késleltetés a következő olvasás előtt
  }
}
