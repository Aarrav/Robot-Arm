#ifndef MOTOR_UTILS_H
#define MOTOR_UTILS_H

#include <Arduino.h>
#include <Encoder.h>

#define baseMotorPWM1 36
#define baseMotorPWM2 37

// External declarations so main.cpp can see them
extern Encoder baseEnc;
extern float currentFilteredVelocity;
extern float targetPosition;


// Function Prototypes
void updateVelocity();
void printDebugInfo();
float getFilteredVelocityMotor();
void moveToPosition(float targetPosition);
float getPositionDegMotor();
float trajectoryGenerator(float finalPosition, float initialPosition, unsigned long timeElapsed, unsigned long totalTime);

#endif