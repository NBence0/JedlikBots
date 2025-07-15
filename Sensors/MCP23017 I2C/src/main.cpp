#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

#define LED_PIN_1 0
#define LED_PIN_2 1
#define LED_PIN_3 2
#define SDA_PIN 36
#define SCL_PIN 35

Adafruit_MCP23X17 mcp;

void setup() {
  Serial.begin(115200);
  Serial.println("MCP23xxx Combo Test!");

  // I2C inicializálás egyedi lábakkal
  Wire.begin(SDA_PIN, SCL_PIN);  // SDA, SCL
  Wire.setClock(1600000);

  // MCP inicializálás az előzőleg inicializált Wire objektummal
  if (!mcp.begin_I2C(0x20, &Wire)) {
    Serial.println("MCP init error.");
    while (1);
  }

  mcp.pinMode(LED_PIN_1, OUTPUT);
  mcp.pinMode(LED_PIN_2, OUTPUT);
  mcp.pinMode(LED_PIN_3, OUTPUT);
}
int i = 0;

void loop() {
  mcp.digitalWrite(LED_PIN_1, LOW);
  mcp.digitalWrite(LED_PIN_2, HIGH);
  mcp.digitalWrite(LED_PIN_3, HIGH);
  delay(200);

  mcp.digitalWrite(LED_PIN_1, HIGH);
  mcp.digitalWrite(LED_PIN_2, LOW);
  delay(200);

  mcp.digitalWrite(LED_PIN_2, HIGH);
  mcp.digitalWrite(LED_PIN_3, LOW);
  delay(200);

  // vissza
  mcp.digitalWrite(LED_PIN_3, HIGH);
  mcp.digitalWrite(LED_PIN_2, LOW);
  delay(200);

  mcp.digitalWrite(LED_PIN_2, HIGH);
  mcp.digitalWrite(LED_PIN_1, LOW);
  delay(200);
}
