#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

long oldPositionBase;
unsigned long oldTimeBase;

const float countsPerRot = 20100.0;   // VERIFY THIS

void setup() {
  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);

  Serial.begin(9600);

  oldPositionBase = baseEnc.read();
  oldTimeBase = micros();
}

void loop() {

  // Motor direction switching
  unsigned long t = millis() % 1500;
  if (t < 750) {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 40);
  } else {
    analogWrite(baseMotorPWM1, 40);
    analogWrite(baseMotorPWM2, 0);
  }

  // Velocity update every 10ms
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 1) {

    long newPosition = baseEnc.read();
    unsigned long newTime = micros();

    float deltaCounts = newPosition - oldPositionBase;
    float deltaTime = (newTime - oldTimeBase) / 1000000.0;

    float positionDeg = (newPosition / countsPerRot) * 360.0;
    float velocityDeg = (deltaCounts / countsPerRot) * 360.0 / deltaTime;

    Serial.print("Position: ");
    Serial.print(positionDeg);
    Serial.print(" deg | Velocity: ");
    Serial.print(velocityDeg);
    Serial.println(" deg/s");

    oldPositionBase = newPosition;
    oldTimeBase = newTime;
    lastUpdate = millis();
  }
}