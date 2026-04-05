#pragma once
#include <Arduino.h>
#include <Encoder.h>

// ── Pin definitions ──────────────────────────────────────────
#define BASE_MOTOR_PWM1    36
#define BASE_MOTOR_PWM2    37
#define BASE_ENCODER_A     39
#define BASE_ENCODER_B     40

// ── Tuning ───────────────────────────────────────────────────
#define BASE_COUNTS_PER_ROT  13384.0f
#define VEL_ALPHA           0.05f
#define BASE_KP                200.0f
#define BASE_KD                 50.0f
#define MAX_PWM           200.0f
#define DEADBAND            0.2f

extern Encoder baseEnc;

void    motorInit();
float   getPositionDeg();
float   getFilteredVelocity();
void    updateVelocity();
void    moveToPosition(float target);
void    printDebugInfo(float target);