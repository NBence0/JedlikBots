// main.cpp

#include <Arduino.h>
#include "LED.h"
#include "GYRO.h"
#include "COLOR_SENSOR.h"
#include "communication.h"
#include "tasks.h"
#include "HC154.h"
#include "Constans.h"

Led led;
BNO08xGyro gyro;
ColorSensor colorSensor;
HC154 spiMux(MUX_PIN_A, MUX_PIN_B, MUX_PIN_C, MUX_PIN_D);

void setup() {
    Serial.begin(115200);
    
    //led.begin();
    //colorSensor.begin();
    //gyro.begin();
    
    setupSpiSlave();
    
    //led.handleColor(2);
    //task_system_self_check();
    
    Serial.println("Setup kész, a fő ciklus indul.");
}

void loop() {
    handleSpiTransaction();
}