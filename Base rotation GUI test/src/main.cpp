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
String UART_data = "";


void setup() {
  // put your setup code here, to run once:
  Serial7.begin(9600);
  Serial7.setTimeout(100);

  pinMode(baseMotorPWM1, OUTPUT);
  pinMode(baseMotorPWM2, OUTPUT);
  pinMode(baseMotorEncoderA, INPUT);
  pinMode(baseMotorEncoderB, INPUT);
  pinMode(currentSensor, INPUT);
  pinMode(baseHallSensor, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
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
  if (UART_data == "") {
    int baseHallState = digitalRead(baseHallSensor);
    printf("Base Hall Sensor State: %d\n", baseHallState);
    delay(50);
    return;
  }
  printf("Received: '%s'\n", UART_data.c_str());

  if (UART_data == "STOP") {
    printf("Stop\n");
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 0);
  }

  if (UART_data == "J1_PLUS") {
    printf("Moving J1 +\n");
    analogWrite(baseMotorPWM1, 0);
    analogWrite(baseMotorPWM2, 20);
  }

  if (UART_data == "J1_MINUS") {
    printf("Moving J1 -\n");
    analogWrite(baseMotorPWM1, 20);
    analogWrite(baseMotorPWM2, 0);
  }
}
