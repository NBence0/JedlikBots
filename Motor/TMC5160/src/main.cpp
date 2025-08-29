#include <TMCStepper.h>

// SPI pinek
#define EN_PIN      33
#define CS_PIN      39
#define MOSI_PIN    35
#define MISO_PIN    37
#define SCK_PIN     36

#define R_SENSE 0.075f  

TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE, MOSI_PIN, MISO_PIN, SCK_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(500);
  Serial.println("TMC5160 teszt indul...");

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // driver tiltva induláskor

  driver.begin();
  driver.toff(5);           // Ajánlott érték
  driver.blank_time(24);
  driver.rms_current(600);  // Kezdjünk alacsonyabb árammal a teszthez
  driver.microsteps(16);    // Kezdjünk alacsonyabb mikrolépéssel, ez stabilabb
  
  // =========================================================================
  // KIKAPCSOLJUK a StealthChop-ot a teszt idejére, hogy a nyomatékos
  // SpreadCycle módot használjuk. Ez a legfontosabb változtatás!
  driver.en_pwm_mode(false); 
  // driver.pwm_autoscale(true); // Ezt is kapcsoljuk ki, mert StealthChop-hoz tartozik
  // =========================================================================

  digitalWrite(EN_PIN, LOW); // driver engedélyezése
  delay(100);

  // Velocity mode beállítása
  driver.VMAX(50000);        // Sebesség beállítása
  
  Serial.println("Driver beállítva, mozgás indul...");
}

void loop() {
  static uint32_t last_print = 0;
  if (millis() - last_print > 1000) {
    last_print = millis();
    
    // Olvassuk ki a GSTAT regisztert a könyvtár segítségével
    uint32_t gstat_val = driver.GSTAT();
    bool is_reset = gstat_val & 0b1;
    bool is_drv_err = (gstat_val >> 1) & 0b1;

    Serial.print("GSTAT: ");
    if (is_reset) {
      Serial.print("[RESET!] "); // Ha ez megjelenik, a tápellátás instabil!
    }
    if (is_drv_err) {
      Serial.print("[DRIVER ERROR!] "); // Ha ez megjelenik, komoly hiba van (pl. rövidzár)
    }
    if (!is_reset && !is_drv_err) {
      Serial.print("OK");
    }
    Serial.println();
  }
}