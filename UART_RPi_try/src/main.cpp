#include <Arduino.h>

void setup() {
  Serial8.begin(9600);
  Serial.begin(9600);
  // Set a timeout so the code doesn't hang too long if no newline is received (optional)
  Serial8.setTimeout(50); 
}

void loop() {
  if (Serial8.available() > 0) {
    String incomingString = Serial8.readStringUntil('\n');
    if (incomingString.length() > 0) {
      Serial.print("UART received: ");
      Serial.println(incomingString);
    }
  }

}