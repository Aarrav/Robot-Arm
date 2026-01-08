#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

#define currentSensor 38
#define baseHallSensor 41

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

long oldPosition  = -999;


void setup() {
  // put your setup code here, to run once:
  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);
  pinMode(baseMotorEncoderA, INPUT);
  pinMode(baseMotorEncoderB, INPUT);
  pinMode(currentSensor, INPUT);
  pinMode(baseHallSensor, INPUT);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i=0; i<500; i++)
  {
    analogWrite(baseMotorPWM1, 40);
    analogWrite(baseMotorPWM2, 0);
    delay(5);

    long newPosition = baseEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      //Serial.println(newPosition);
      Serial.println(analogRead(baseHallSensor));
    }
  }

  for(int i=0; i<500; i++)
  {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 40);
    delay(5);

    long newPosition = baseEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      //Serial.println(newPosition);
      Serial.println(analogRead(baseHallSensor));
    }
  }
}
