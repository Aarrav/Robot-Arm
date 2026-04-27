#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

#define shoulderMotorPWM1 11
#define shoulderMotorPWM2 12
#define shoulderMotorEncoderA 25
#define shoulderMotorEncoderB 24

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);
Encoder shoulderEnc(shoulderMotorEncoderA, shoulderMotorEncoderB);

long oldPositionBase  = -999;
long oldPositionShoulder  = -999;

void setup() {
  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);
  pinMode(baseMotorEncoderA, INPUT);
  pinMode(baseMotorEncoderB, INPUT);

  pinMode(shoulderMotorPWM1, OUTPUT);
  pinMode(shoulderMotorPWM2, OUTPUT);
  pinMode(shoulderMotorEncoderA, INPUT);
  pinMode(shoulderMotorEncoderB, INPUT);

  Serial.begin(9600);
  Serial.println("Jogger:");
}

void loop() {
  long newPositionBase = baseEnc.read();
  long newPositionShoulder = shoulderEnc.read();

  if (newPositionBase != oldPositionBase || newPositionShoulder != oldPositionShoulder) {
    Serial.print("Base: ");
    Serial.print(newPositionBase);
    Serial.print(" | Shoulder: ");
    Serial.println(newPositionShoulder);

    oldPositionBase = newPositionBase;
    oldPositionShoulder = newPositionShoulder;
  }

  if (millis() < 4000) {
    analogWrite(shoulderMotorPWM1, 30);
    analogWrite(shoulderMotorPWM2, 0);
  }
  else{
    analogWrite(shoulderMotorPWM1, 0);
    analogWrite(shoulderMotorPWM2, 0);
  }
}