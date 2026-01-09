#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37
#define baseMotorEncoderA 39
#define baseMotorEncoderB 40

#define shoulderMotorPWM1 11
#define shoulderMotorPWM2 12
#define shoulderMotorEncoderA 23
#define shoulderMotorEncoderB 24

Encoder baseEnc(baseMotorEncoderA, baseMotorEncoderB);
Encoder shoulderEnc(shoulderMotorEncoderA, shoulderMotorEncoderB);

long oldPositionBase  = -999;
long oldPositionShoulder  = -999;
String UART_data = "";

void setup() {
  Serial7.begin(9600);
  Serial7.setTimeout(100);

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

String readUART() {
  if (Serial7.available() > 0) {
    String result = Serial7.readStringUntil('\n');
    result.trim();
    return result;
  }
  return "";
}


void loop() {
  // put your main code here, to run repeatedly:

  UART_data = readUART();
  if (UART_data != "")
  {
    printf("Received: '%s'\n", UART_data.c_str());

    if (UART_data == "STOP") {
      printf("Stop\n");
      analogWrite(baseMotorPWM1, 0);
      analogWrite(baseMotorPWM2, 0);
      analogWrite(shoulderMotorPWM1, 0);
      analogWrite(shoulderMotorPWM2, 0);
    }

    if (UART_data == "BaseCW") {
      printf("Moving Base Clockwise\n");
      analogWrite(baseMotorPWM1, 0);
      analogWrite(baseMotorPWM2, 20);
    }

    if (UART_data == "BaseCCW") {
      printf("Moving Base Counterclockwise\n");
      analogWrite(baseMotorPWM1, 20);
      analogWrite(baseMotorPWM2, 0);
    }

      if (UART_data == "ShoulderUp") {
      printf("Moving Shoulder Up\n");
      analogWrite(shoulderMotorPWM1, 20);
      analogWrite(shoulderMotorPWM2, 0);
    }

    if (UART_data == "ShoulderDown") {
      printf("Moving Shoulder Down\n");
      analogWrite(shoulderMotorPWM1, 0);
      analogWrite(shoulderMotorPWM2, 15);
    }
  }
}