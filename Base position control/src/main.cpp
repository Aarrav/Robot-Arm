#include <Arduino.h>
#include "motor_utils.h"

#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

unsigned long startTime;
float targetPosition = -20.0;

void setup() {
    pinMode(baseMotorPWM1, OUTPUT);
    pinMode(baseMotorPWM2, OUTPUT);
    Serial.begin(9600);

    delay(2000); 
    startTime = millis();
}

void loop() {
  updateVelocity();
  printDebugInfo();
  targetPosition = trajectoryGenerator(20.0, 0.0, millis() - startTime, 150); // Generate target position based on a 150 ms trajectory
  moveToPosition(targetPosition);
  delayMicroseconds(500); // Control loop delay of 500 us 
}