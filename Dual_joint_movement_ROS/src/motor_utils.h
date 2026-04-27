#pragma once
#include <Arduino.h>
#include <Encoder.h>

// ── Pin definitions ──────────────────────────────────────────
#define BASE_MOTOR_PWM1       36
#define BASE_MOTOR_PWM2       37
#define BASE_ENCODER_A        39
#define BASE_ENCODER_B        40

#define SHOULDER_MOTOR_PWM1   11
#define SHOULDER_MOTOR_PWM2   12
#define SHOULDER_ENCODER_A    24
#define SHOULDER_ENCODER_B    25

// ── Tuning ───────────────────────────────────────────────────
#define BASE_COUNTS_PER_ROT     13384.0f
#define SHOULDER_COUNTS_PER_ROT 13384.0f
#define VEL_ALPHA               0.05f
#define BASE_KP                 200.0f
#define BASE_KD                  50.0f
#define SHOULDER_KP             200.0f
#define SHOULDER_KD              50.0f
#define MAX_PWM                  60.0f
#define DEADBAND                  0.2f

extern Encoder baseEnc;
extern Encoder shoulderEnc;

void  motorInit();

float getBasePositionDeg();
float getShoulderPositionDeg();
float getBaseFilteredVelocity();
float getShoulderFilteredVelocity();

void  updateVelocity();
void  moveBaseToPosition(float target);
void  moveShoulderToPosition(float target);
void  printDebugInfo(float baseTarget, float shoulderTarget);