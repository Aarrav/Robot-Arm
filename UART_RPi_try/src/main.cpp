#include <Arduino.h>

void setup() {
  Serial7.begin(9600);
  Serial.begin(9600);
  // Set a timeout so the code doesn't hang too long if no newline is received (optional)
  Serial7.setTimeout(500); 
}

void loop() {
  if (Serial7.available() > 0) {
    String incomingString = Serial7.readStringUntil('\n');

    if (incomingString.length() > 0) {
      Serial.print("UART received: ");
      Serial.println(incomingString);
    }
    else {
      Serial.println("No data received within timeout period.");
    }
  }

}