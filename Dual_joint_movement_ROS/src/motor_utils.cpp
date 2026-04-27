#include "motor_utils.h"

Encoder baseEnc(BASE_ENCODER_A, BASE_ENCODER_B);
Encoder shoulderEnc(SHOULDER_ENCODER_A, SHOULDER_ENCODER_B);

// ── Private state ────────────────────────────────────────────
static long          prevBaseCount     = 0;
static long          prevShoulderCount = 0;
static unsigned long prevTimeUs        = 0;
static float         filteredBaseVelocity     = 0.0f;
static float         filteredShoulderVelocity = 0.0f;
static float         prevBaseError     = 0.0f;
static float         prevShoulderError = 0.0f;

// ── Init ─────────────────────────────────────────────────────
void motorInit() {
  pinMode(BASE_MOTOR_PWM1,     OUTPUT);
  pinMode(BASE_MOTOR_PWM2,     OUTPUT);
  pinMode(SHOULDER_MOTOR_PWM1, OUTPUT);
  pinMode(SHOULDER_MOTOR_PWM2, OUTPUT);
}

// ── Position getters ─────────────────────────────────────────
float getBasePositionDeg() {
  return (baseEnc.read() / BASE_COUNTS_PER_ROT) * 360.0f;
}

float getShoulderPositionDeg() {
  return -(shoulderEnc.read() / SHOULDER_COUNTS_PER_ROT) * 360.0f;
}

// ── Velocity getters ─────────────────────────────────────────
float getBaseFilteredVelocity() {
  return filteredBaseVelocity;
}

float getShoulderFilteredVelocity() {
  return filteredShoulderVelocity;
}

// ── Velocity update (call every loop) ────────────────────────
void updateVelocity() {
  static unsigned long lastUpdate = 0;
  unsigned long now = micros();
  if (now - lastUpdate < 3000) return;  // update every 3ms

  long newBaseCount     = baseEnc.read();
  long newShoulderCount = shoulderEnc.read();
  float dt              = (now - prevTimeUs) / 1e6f;

  if (dt > 0) {
    float rawBase = ((float)(newBaseCount - prevBaseCount) 
                    / BASE_COUNTS_PER_ROT) * 360.0f / dt;
    filteredBaseVelocity = VEL_ALPHA * rawBase 
                         + (1.0f - VEL_ALPHA) * filteredBaseVelocity;

    float rawShoulder = ((float)(newShoulderCount - prevShoulderCount) 
                        / SHOULDER_COUNTS_PER_ROT) * 360.0f / dt;
    filteredShoulderVelocity = VEL_ALPHA * rawShoulder 
                             + (1.0f - VEL_ALPHA) * filteredShoulderVelocity;
  }

  prevBaseCount     = newBaseCount;
  prevShoulderCount = newShoulderCount;
  prevTimeUs        = now;
  lastUpdate        = now;
}

// ── PD controllers ───────────────────────────────────────────
void moveBaseToPosition(float target) {
  float error = target - getBasePositionDeg();
  float pwm   = BASE_KP * error + BASE_KD * (error - prevBaseError);
  prevBaseError = error;

  pwm = constrain(pwm, -MAX_PWM, MAX_PWM);
  if (abs(error) < DEADBAND) pwm = 0;

  if (pwm < 0) {
    analogWrite(BASE_MOTOR_PWM1, (int)-pwm);
    analogWrite(BASE_MOTOR_PWM2, 0);
  } else {
    analogWrite(BASE_MOTOR_PWM1, 0);
    analogWrite(BASE_MOTOR_PWM2, (int)pwm);
  }
}

void moveShoulderToPosition(float target) {
  float error = target - getShoulderPositionDeg();
  float pwm   = SHOULDER_KP * error + SHOULDER_KD * (error - prevShoulderError);
  prevShoulderError = error;

  pwm = constrain(pwm, -MAX_PWM, MAX_PWM);
  if (abs(error) < DEADBAND) pwm = 0;

  if (pwm < 0) {
    analogWrite(SHOULDER_MOTOR_PWM1, (int)-pwm);
    analogWrite(SHOULDER_MOTOR_PWM2, 0);
  } else {
    analogWrite(SHOULDER_MOTOR_PWM1, 0);
    analogWrite(SHOULDER_MOTOR_PWM2, (int)pwm);
  }
}

// ── Debug ────────────────────────────────────────────────────
void printDebugInfo(float baseTarget, float shoulderTarget) {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 5) return;
  lastPrint = millis();

  Serial.print("B_vel: ");   Serial.print(getBaseFilteredVelocity(), 2);
  Serial.print("  B_pos: "); Serial.print(getBasePositionDeg(), 2);
  Serial.print("  B_tgt: "); Serial.print(baseTarget, 2);
  Serial.print("  |  ");
  Serial.print("S_vel: ");   Serial.print(getShoulderFilteredVelocity(), 2);
  Serial.print("  S_pos: "); Serial.print(getShoulderPositionDeg(), 2);
  Serial.print("  S_tgt: "); Serial.println(shoulderTarget, 2);
}