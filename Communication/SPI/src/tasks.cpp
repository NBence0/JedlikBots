// tasks.cpp
#include "tasks.h"
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"

// Itt is hivatkozni kell a globális periféria objektumokra
extern Led led;
extern BNO08xGyro gyro;
extern ColorSensor colorSensor;

void task_system_self_check() {
    Serial.println("Rendszer önellenőrzés indítása...");
    
    // Giroszkóp ellenőrzése
    if (gyro.isConnected()) {
        Serial.println("Giroszkóp: OK");
        led.handleColor(3); // Zöld
    } else {
        Serial.println("Giroszkóp: HIBA");
        led.handleColor(1); // Piros
        return; // Hiba esetén leállhatunk
    }
    delay(500);

    // Itt jöhetne a többi szenzor ellenőrzése is...

    Serial.println("Önellenőrzés kész.");
    led.off();
}

void task_read_all_sensors() {
    // Ez a függvény egyben olvassa az összes szenzort
    // és előkészítheti az adatokat egy nagyobb válaszcsomaghoz.
    // ...
}