// main.cpp

#include <Arduino.h>
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"
#include "communication.h"
#include "tasks.h"
#include "HC154.h"
#include "Constans.h"
#include "DeviceManager.h"

Led led;
ColorSensor colorSensor;
BMESensor bme;  
BNO08xGyro gyro;
DeviceManager deviceManager(gyro, bme);
//HC154 spiMux(MUX_PIN_A, MUX_PIN_B, MUX_PIN_C, MUX_PIN_D);

void setup() {
    Serial.begin(115200);
    deviceManager.beginAll();

    //led.begin();
    //colorSensor.begin();
    pinMode(DATA_READY_PIN, OUTPUT);
    digitalWrite(DATA_READY_PIN, LOW); // Alaphelyzetben alacsony
    
    setupSpiSlave();
    
    //led.handleColor(2);
    //task_system_self_check();
    
    Serial.println("Setup kész, a fő ciklus indul.");
}

void loop() {
    handleSpiTransaction();
}