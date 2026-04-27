#include "motor_utils.h"

Encoder baseEnc(BASE_ENCODER_A, BASE_ENCODER_B);

// ── Private state ────────────────────────────────────────────
static long     prevEncoderCount    = 0;
static unsigned long prevTimeUs     = 0;
static float    filteredVelocity    = 0.0f;
static float    prevError           = 0.0f;

// ── Init ─────────────────────────────────────────────────────
void motorInit() {
  pinMode(BASE_MOTOR_PWM1, OUTPUT);
  pinMode(BASE_MOTOR_PWM2, OUTPUT);
}

// ── Getters ──────────────────────────────────────────────────
float getPositionDeg() {
  return -(baseEnc.read() / BASE_COUNTS_PER_ROT) * 360.0f;
}

float getFilteredVelocity() {
  return filteredVelocity;
}

// ── Velocity (call every loop) ───────────────────────────────
void updateVelocity() {
  static unsigned long lastUpdate = 0;
  unsigned long now = micros();
  if (now - lastUpdate < 3000) return;  // update every 3ms

  long newCount  = baseEnc.read();
  float delta    = (float)(newCount - prevEncoderCount);
  float dt       = (now - prevTimeUs) / 1e6f;
  if (dt > 0) {
    float raw      = (delta / BASE_COUNTS_PER_ROT) * 360.0f / dt;
    filteredVelocity = VEL_ALPHA * raw + (1.0f - VEL_ALPHA) * filteredVelocity;
  }

  prevEncoderCount = newCount;
  prevTimeUs       = now;
  lastUpdate       = now;
}

// ── PD controller ────────────────────────────────────────────
void moveToPosition(float target) {
  float error = target - getPositionDeg();
  float pwm   = BASE_KP * error + BASE_KD * (error - prevError);
  prevError   = error;

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

// ── Debug ────────────────────────────────────────────────────
void printDebugInfo(float target) {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 5) return;  // print every 5ms
  lastPrint = millis();

  Serial.print(getFilteredVelocity(), 2);
  Serial.print("  pos: ");
  Serial.print(getPositionDeg(), 2);
  Serial.print("  target: ");
  Serial.println(target, 2);
}