#include "motor_utils.h"

// Define constants used only here
const float countsPerRotMotor = 13384.0;
const float alpha = 0.05;

// Variables for tracking state
long oldPositionBase = 0;
unsigned long oldTimeBase = 0;
float currentFilteredVelocity = 0.0;

float getPositionDegMotor() {
    long currentPosition = baseEnc.read();
    return (currentPosition / countsPerRotMotor) * 360.0;
}

float getFilteredVelocityMotor() {
    return currentFilteredVelocity;
}

void updateVelocity() {
    static unsigned long lastUpdate = 0;
    unsigned long currentTime = micros();

    if (currentTime - lastUpdate >= 3000) { // Update every 3 ms
        long newPositionBase = baseEnc.read();
        float deltaCounts = newPositionBase - oldPositionBase;
        float deltaTime = (currentTime - oldTimeBase) / 1000000.0;

        if (deltaTime > 0) {
            float rawVelocityMotor = (deltaCounts / countsPerRotMotor) * 360.0 / deltaTime;
            currentFilteredVelocity = (alpha * rawVelocityMotor) + ((1.0 - alpha) * currentFilteredVelocity);
        }

        oldPositionBase = newPositionBase;
        oldTimeBase = currentTime;
        lastUpdate = currentTime;
    }
}

void printDebugInfo() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 5) {  // Print every 5 ms
        // Serial.print("Position (deg): ");
        Serial.print(getFilteredVelocityMotor(), 2);
        // Serial.print(" | Velocity (deg/s): ");
        Serial.print(" ");
        Serial.print(getPositionDegMotor(), 2);
        Serial.print(" ");
        Serial.println(targetPosition, 2);
        lastPrint = millis();
    }
}

void moveToPosition(float targetPosition) {
    float Kp = 700.0;
    float Kd = 50.0;
    float prevError = 0.0;
    const float maxPWM = 200.0;

    updateVelocity();
    float currentPosition = getPositionDegMotor();
    float error = targetPosition - currentPosition;
    float PWM = Kp * error + Kd * (error - prevError);

    if (PWM > maxPWM) PWM = maxPWM;
    if (PWM < -maxPWM) PWM = -maxPWM;

    if (PWM < 0) {
        analogWrite(baseMotorPWM1, -PWM);
        analogWrite(baseMotorPWM2, 0);
    } else {
        analogWrite(baseMotorPWM1, 0);
        analogWrite(baseMotorPWM2, PWM);
    }

    prevError = error;

    // (TO BE ADDED) Exit condition for the loop can be added here based on desired precision
}

float trajectoryGenerator(float finalPosition, float initialPosition, unsigned long timeElapsed, unsigned long totalTime) {
    if (timeElapsed >= totalTime)
        return finalPosition;

    float t = (float)timeElapsed / (float)totalTime;

    // Cubic Polynomial: s(t) = 3t^2 - 2t^3
    float step = (3 * t * t) - (2 * t * t * t);

    // Interpolate between initial and final
    float targetPosition = initialPosition + (finalPosition - initialPosition) * step;
    return targetPosition; 
}