#include <TMCStepper.h>
#include <SPI.h>

// SPI pinek
#define EN_PIN      20
#define CS_PIN      40
#define MOSI_PIN    38
#define MISO_PIN    39
#define SCK_PIN     41
#define LED     10


#define R_SENSE 0.075f  



TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE);

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
  while (!Serial);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // driver engedélyezése



  driver.begin();
  driver.toff(4);
  driver.blank_time(24);
  driver.rms_current(400);
  driver.microsteps(256);
  driver.en_pwm_mode(true);
  driver.pwm_autoscale(true);

  // Velocity mode
  driver.RAMPMODE(1);
  driver.VMAX(50000);
  driver.AMAX(1000);   // gyorsulás
  driver.DMAX(1000);   // lassulás
  /*
  delay(5000);


  for (size_t i = 0; i < 50; i++)
  {
    digitalWrite(LED, LOW);
    delay(300-(i*5));
    digitalWrite(LED, HIGH);
    delay(300-(i*5));
  }
  digitalWrite(LED, LOW);
*/
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