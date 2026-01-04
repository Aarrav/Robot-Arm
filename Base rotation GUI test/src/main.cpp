#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

#define currectSensor 38

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);

long oldPosition  = -999;


void setup() {
  // put your setup code here, to run once:
  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);
  pinMode(baseMotorEncoderA, INPUT);
  pinMode(baseMotorEncoderB, INPUT);
  pinMode(currectSensor, INPUT);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i=0; i<100; i++)
  {
    analogWrite(baseMotorPWM1, 20);
    analogWrite(baseMotorPWM2, 0);
    delay(5);

    long newPosition = baseEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.println(newPosition);
    }
  }


  for(int i=0; i<100; i++)
  {
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 20);
    delay(5);

    long newPosition = baseEnc.read();
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.println(newPosition);
    }
  }
}
