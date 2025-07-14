#include <Adafruit_AS7341.h>
#include <Wire.h>

#define SDA_PIN 42
#define SCL_PIN 41

Adafruit_AS7341 as7341;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN); // I2C pin beállítás

  if (!as7341.begin()) {
    Serial.println("AS7341 not found");
    while (1);
  }
  as7341.enableLED(true);  // bekapcsolja a LED-et

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
}

void loop() {
  if (!as7341.readAllChannels()) {
    Serial.println("Error reading all channels!");
    return;
  }

  Serial.print("F1 415nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print("F2 445nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print("F3 480nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print("F4 515nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print("F5 555nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print("F6 590nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print("F7 630nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print("F8 680nm : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  Serial.print("Clear    : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_CLEAR));
  Serial.print("Near IR  : "); Serial.println(as7341.getChannel(AS7341_CHANNEL_NIR));
  Serial.println("");
}
