#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

long oldPositionBase;
unsigned long oldTimeBase, startTime;

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
  delay(2000); 
  startTime = millis();

}

void loop() {

  if (millis() - startTime < 1000) {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 20);
  } else {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 0);
  }

  // Velocity update every 10ms
  static unsigned long lastUpdate = 0;
  if (micros() - lastUpdate >= 100000) {

    long newPositionBase = baseEnc.read();
    unsigned long newTimeBase = micros();

    float deltaCounts = newPositionBase - oldPositionBase;
    float deltaTime = (newTimeBase - oldTimeBase) / 100000.0;

    float positionDeg = (newPositionBase / countsPerRot) * 360.0;
    float velocityDeg = (deltaCounts / countsPerRot) * 360.0 / deltaTime;

    // Serial.print("Position: ");
    // Serial.print(positionDeg);
    // Serial.print(" deg | Velocity: ");
    // Serial.print(velocityDeg);
    // Serial.println(" deg/s");

    Serial.print(newPositionBase);
    Serial.print(" , ");
    Serial.println(oldPositionBase);

    oldPositionBase = newPositionBase;
    oldTimeBase = newTimeBase;
    lastUpdate = millis();
  }
}