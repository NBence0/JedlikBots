#ifndef ROBOTSERVO_H
#define ROBOTSERVO_H

#include <Arduino.h>
#include <ESP32Servo.h>

class RobotServo {
public:
    RobotServo(); // Konstruktor
    void begin(uint8_t pin, uint8_t startPos = 160);
    

    // Lassított mozgás (az általad írt logika alapján)
    void moveTo(int targetPosition, uint8_t delayTime = 15);

    // Azonnali ugrás egy pozícióra (ha nem kell lassítás)
    void write(uint8_t position);

    // Aktuális pozíció lekérdezése
    int read();

private:
    Servo _servo;  // A belső ESP32Servo objektum
    uint8_t _pin;  // A láb, ahova csatlakoztatva van
    int _currentPos;    
};

#endif