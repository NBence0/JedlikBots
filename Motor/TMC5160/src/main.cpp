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

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // driver tiltva induláskor

  driver.begin();
  driver.toff(4);
  driver.blank_time(24);
  driver.rms_current(800);
  driver.microsteps(256); 
  driver.en_pwm_mode(true);
  driver.pwm_autoscale(true);

  // Velocity mode
  driver.RAMPMODE(1);        // 1 = előre velocity mode
  driver.VMAX(50000);        // sebesség (pozitív = előre, RAMPMODE=2 esetén hátra)

  digitalWrite(EN_PIN, LOW); // driver engedélyezése
  Serial.println("TMC5160 velocity mode indul!");
}

void loop() {
  static uint32_t last_time = 0;
  static bool dir = true;

  if (millis() - last_time > 5000) {
    last_time = millis();
    dir = !dir;
    if (dir) {
      driver.RAMPMODE(1);   // előre
      driver.VMAX(60000);
      Serial.println("Előre forog");
    } else {
      driver.RAMPMODE(2);   // hátra
      driver.VMAX(60000);
      Serial.println("Hátra forog");
    }
  }

  // Debug kiírás
  static uint32_t last_print = 0;
  if (millis() - last_print > 1000) {
    last_print = millis();
    Serial.print("XACTUAL = ");
    Serial.println(driver.XACTUAL());
  }
}
