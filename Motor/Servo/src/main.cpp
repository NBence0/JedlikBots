#include <ESP32Servo.h>

Servo myServo;
int servoPin = 20;

void setup() {
	Serial.begin(115200);

	myServo.setPeriodHertz(50); 
	myServo.attach(servoPin); 
}

void loop() {
	for (int pos = 0; pos <= 180; pos += 1) {
		myServo.write(pos);
		delay(1); 
	}
	
	delay(1000); 

	for (int pos = 180; pos >= 0; pos -= 1) {
		myServo.write(pos);
		delay(1);
	}
	
	delay(1000);
}