#include <Wire.h>
#include <Adafruit_AS7341.h>

#define SDA_PIN 42
#define SCL_PIN 41

// TCA9548A címe
#define TCA_ADDR 0x70

// Multiplexer csatorna kiválasztása (0–7)
void selectMuxChannel(uint8_t channel) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

// 4 külön AS7341 objektum
Adafruit_AS7341 sensor1;
Adafruit_AS7341 sensor2;
Adafruit_AS7341 sensor3;
Adafruit_AS7341 sensor4;

bool initSensor(Adafruit_AS7341 &sensor, uint8_t channel) {
  selectMuxChannel(channel);
  if (!sensor.begin()) return false;
  sensor.enableLED(true);
  sensor.setATIME(100);
  sensor.setASTEP(999);
  sensor.setGain(AS7341_GAIN_256X);
  return true;
}

void readSensor(Adafruit_AS7341 &sensor, uint8_t channel) {
  selectMuxChannel(channel);
  if (!sensor.readAllChannels()) {
    Serial.print("Error reading sensor on channel ");
    Serial.println(channel);
    return;
  }
  
  Serial.print("Sensor on channel "); Serial.println(channel);
  Serial.print("415nm Bíbor / UV közeli: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print("445nm Kékeslila / Kék: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print("480nm Kék: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print("515nm Zöldes cián: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print("555nm	Zöld: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print("590nm Sárgás-narancs: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print("630nm Piros: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print("680nm Mélyvörös / közel IR: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_680nm_F8));
  Serial.print("Clear: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_CLEAR));
  Serial.print("NIR: "); Serial.println(sensor.getChannel(AS7341_CHANNEL_NIR));
  Serial.println("--------------------------");
}

// Hibakezelés ha nincs szenzor
void halt(const char *msg) {
  Serial.println(msg);
  while (true) delay(1000);
}


void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Minden csatornára szenzor inicializálása
  if (!initSensor(sensor1, 0)) halt("Sensor 1 (ch 0) not found");
  if (!initSensor(sensor2, 1)) halt("Sensor 2 (ch 1) not found");
  //if (!initSensor(sensor3, 2)) halt("Sensor 3 (ch 2) not found");
  //if (!initSensor(sensor4, 3)) halt("Sensor 4 (ch 3) not found");
}


// Kiolvassa és kiírja az értékeket egy adott szenzorról



void loop() {
  readSensor(sensor1, 0);
  delay(5000);
  readSensor(sensor2, 1);
  //readSensor(sensor3, 2);
  //readSensor(sensor4, 3);
  delay(2000);
}

// Egy adott csatornán inicializálja az AS7341-et
