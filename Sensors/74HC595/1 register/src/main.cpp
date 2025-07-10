//**************************************************************//
//  Name    : shiftOutCode, Predefined Dual Array Style         //
//  Author  : Carlyn Maw, Tom Igoe                              //
//  Date    : 25 Oct, 2006                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                            //

#include <Arduino.h>

int latchPin = 10;
int clockPin = 12;
int dataPin = 11;
byte dataRED;
byte dataGREEN;
byte dataArrayRED[10];
byte dataArrayGREEN[10];

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
int i=0;
int pinState;
pinMode(myClockPin, OUTPUT);
pinMode(myDataPin, OUTPUT);
digitalWrite(myDataPin, 0);
digitalWrite(myClockPin, 0);
for (i=7; i>=0; i--)  {
digitalWrite(myClockPin, 0);

if ( myDataOut & (1<<i) ) {
pinState= 1;
}
else {
pinState= 0;
}

digitalWrite(myDataPin, pinState);
digitalWrite(myClockPin, 1);
digitalWrite(myDataPin, 0);
}
digitalWrite(myClockPin, 0);
}

void blinkAll_2Bytes(int n, int d) {
digitalWrite(latchPin, 0);
shiftOut(dataPin, clockPin, 0);
shiftOut(dataPin, clockPin, 0);
digitalWrite(latchPin, 1);
delay(200);
for (int x = 0; x < n; x++) {
digitalWrite(latchPin, 0);
shiftOut(dataPin, clockPin, 255);
shiftOut(dataPin, clockPin, 255);
digitalWrite(latchPin, 1);
delay(d);
digitalWrite(latchPin, 0);
shiftOut(dataPin, clockPin, 0);
shiftOut(dataPin, clockPin, 0);
digitalWrite(latchPin, 1);
delay(d);
}
}


void setup() {
pinMode(latchPin, OUTPUT);
Serial.begin(9600);
dataArrayRED[0] = 0xFF; //11111111
dataArrayRED[1] = 0xFE; //11111110
dataArrayRED[2] = 0xFC; //11111100
dataArrayRED[3] = 0xF8; //11111000
dataArrayRED[4] = 0xF0; //11110000
dataArrayRED[5] = 0xE0; //11100000
dataArrayRED[6] = 0xC0; //11000000
dataArrayRED[7] = 0x80; //10000000
dataArrayRED[8] = 0x00; //00000000
dataArrayRED[9] = 0xE0; //11100000


dataArrayGREEN[0] = 0xFF; //11111111
dataArrayGREEN[1] = 0x7F; //01111111
dataArrayGREEN[2] = 0x3F; //00111111
dataArrayGREEN[3] = 0x1F; //00011111
dataArrayGREEN[4] = 0x0F; //00001111
dataArrayGREEN[5] = 0x07; //00000111
dataArrayGREEN[6] = 0x03; //00000011
dataArrayGREEN[7] = 0x01; //00000001
dataArrayGREEN[8] = 0x00; //00000000
dataArrayGREEN[9] = 0x07; //00000111
blinkAll_2Bytes(2,200);
}

void loop() {
for (int j = 0; j < 10; j++) {
dataRED = dataArrayRED[j];
dataGREEN = dataArrayGREEN[j];
digitalWrite(latchPin, 0);
shiftOut(dataPin, clockPin, dataGREEN);
shiftOut(dataPin, clockPin, dataRED);
digitalWrite(latchPin, 1);
delay(50); // ennyi időnként küld adatot
}
}
