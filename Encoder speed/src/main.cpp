#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

long oldPositionBase;
unsigned long oldTimeBase, startTime;
unsigned long timePrint;
float alpha = 0.2;
float filteredVelocity = 0;
float velocityDeg, oldVelocityDeg;
static int staleCounter = 0;

// Driving gear = 20 teeth
// Driven gear = 30 teeth

// Encoder counts per revolution of the motor shaft = 13384
// Encoder counts per revolution of the output shaft = 13384 * (30/20) = 20076

const float countsPerRot = 13384.0; 

void setup() {
  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);

  Serial.begin(9600);

  oldPositionBase = baseEnc.read();
  oldTimeBase = micros();
  delay(4000); 
  startTime = millis();

}

void loop() {

  if (millis() - startTime < 3000) {
    analogWrite(baseMotorPWM1, 100);
    analogWrite(baseMotorPWM2, 0);
  } else {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 0);
  }

  // Velocity update every 10ms
  /*
  static unsigned long lastUpdate = 0;
  if (micros() - lastUpdate >= 10000) { // 10ms = 10000 microseconds

    long newPositionBase = baseEnc.read();
    unsigned long newTimeBase = micros();

    float deltaCounts = newPositionBase - oldPositionBase;
    float deltaTime = newTimeBase - oldTimeBase;
    deltaTime /= 1000000.0; // Convert microseconds to seconds

    float positionDeg = (newPositionBase / (float)countsPerRot) * 360.0;
    float rawVelocity = (deltaCounts / (float)countsPerRot) * 360.0 / deltaTime;
    filteredVelocity = (alpha * rawVelocity) + ((1 - alpha) * filteredVelocity);

    Serial.print(filteredVelocity, 6);
    Serial.println();

    oldPositionBase = newPositionBase;
    oldTimeBase = newTimeBase;
    lastUpdate = micros();
  }
  */

  long newPositionBase = baseEnc.read();
  unsigned long newTimeBase = micros();

  if(newPositionBase != oldPositionBase) {
    float deltaCounts = newPositionBase - oldPositionBase;
    float deltaTime = newTimeBase - oldTimeBase;
    deltaTime /= 1000000.0; // Convert microseconds to seconds

    float positionDeg = (newPositionBase / (float)countsPerRot) * 360.0;
    velocityDeg = (deltaCounts / countsPerRot) * 360.0 / deltaTime;

    oldPositionBase = newPositionBase;
    oldTimeBase = newTimeBase;
  }

  if(micros() - timePrint >= 10000) { // Print every 10ms

    if (velocityDeg == oldVelocityDeg) {
      staleCounter++;
    }else {
      staleCounter = 0; // Reset counter if velocity changes
      oldVelocityDeg = velocityDeg;
    }

    if (staleCounter >= 3) {
      velocityDeg = 0; // Consider it as zero if it hasn't changed for a while
    }

    Serial.print(velocityDeg, 6);
    Serial.println();
    timePrint = micros();
  }

}