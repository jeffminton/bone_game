

#include "Arduino.h"


#define LEDPIN 11

void setup() {
    Serial1.begin(9600);
    pinMode(LEDPIN, OUTPUT);
}



void loop() {
    // digitalWrite(LEDPIN, HIGH);
    Serial1.println("Testing");
    // delay(500);
    // digitalWrite(LEDPIN, LOW);
    // delay(500);
}