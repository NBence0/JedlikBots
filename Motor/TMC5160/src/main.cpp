#include <TMCStepper.h>
#include <SPI.h>

// A bekötésnek megfelelő pinek
#define EN_PIN      33
#define CS_PIN      39
#define MOSI_PIN    35
#define MISO_PIN    37
#define SCK_PIN     36

TMC5160Stepper driver = TMC5160Stepper(CS_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Kommunikációs teszt indítása...");

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // Driver letiltva a biztonság kedvéért

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  driver.begin();

  // Teszteljük a kapcsolatot. Ennek 0-t kell visszaadnia siker esetén.
  uint8_t connection_result = driver.test_connection();

  Serial.print("Kommunikációs teszt eredménye: ");
  if (connection_result == 0) {
    Serial.println("OK! A driver válaszol.");
  } else {
    Serial.print("HIBA! Kód: ");
    Serial.println(connection_result);
    Serial.println("Ellenőrizd a bekötést (MOSI/MISO, CS, GND)!");
    while(true) delay(100); // Hiba esetén álljon meg itt
  }

  // Olvassuk ki a GCONF (általános konfigurációs) regisztert
  // Ha a kommunikáció működik, itt egy nem-nulla értéket kell látnunk.
  uint32_t gconf_val = driver.GCONF();
  Serial.print("GCONF regiszter értéke: ");
  Serial.println(gconf_val, HEX); // Kiírjuk hexadecimálisan

  // Olvassuk ki a verzió számot
  uint8_t version = driver.IOIN();
  Serial.print("Driver verzió (IOIN): ");
  Serial.println(version, HEX);


  Serial.println("Teszt vége.");
}

void loop() {
  // A teszthez a loop üres
}