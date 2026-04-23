#include "Servo.h"

RobotServo::RobotServo() {
    _currentPos = 0;
}

void RobotServo::begin(uint8_t pin, uint8_t startPos) {
    _pin = pin;
    _servo.setPeriodHertz(50);
    _servo.attach(_pin);

    _currentPos = startPos;
    _servo.write(_currentPos);
}

void RobotServo::moveTo(int targetPosition, uint8_t delayTime) {

    if (_currentPos < targetPosition) {
        for (int pos = _currentPos; pos <= targetPosition; pos += 1) {
            _servo.write(pos);
            delay(delayTime);
        }
    } 
    else if (_currentPos > targetPosition) {
        for (int pos = _currentPos; pos >= targetPosition; pos -= 1) {
            _servo.write(pos);
            delay(delayTime);
        }
    }
    _currentPos = targetPosition; 
}

void RobotServo::write(uint8_t position) {
    _servo.write(position);
    _currentPos = position;
}

int RobotServo::read() {
    return _currentPos;
}