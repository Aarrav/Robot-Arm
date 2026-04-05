#pragma once

float trajectoryGenerator(float finalPos, float initialPos, unsigned long timeElapsed, unsigned long totalTime) {
  if (timeElapsed >= totalTime) return finalPos;
  float t    = (float)timeElapsed / (float)totalTime;
  float step = (3 * t * t) - (2 * t * t * t);  // cubic polynomial
  return initialPos + (finalPos - initialPos) * step;
}