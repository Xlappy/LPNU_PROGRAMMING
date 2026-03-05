#include <Arduino.h>

#define RED_LED 6
#define GREEN_LED 7
#define WHITE_LED 8

void setup() {
  Serial.begin(9600);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);
  Serial.println("--- LED SEQUENCE TEST START ---");
}

void loop() {
  // 1. Red (Pin 6)
  Serial.println("Blinking RED (Pin 6)...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(RED_LED, LOW);
    delay(300);
  }

  // 2. Green (Pin 7)
  Serial.println("Blinking GREEN (Pin 7)...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(GREEN_LED, HIGH);
    delay(300);
    digitalWrite(GREEN_LED, LOW);
    delay(300);
  }

  // 3. White (Pin 8)
  Serial.println("Blinking WHITE (Pin 8)...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(WHITE_LED, HIGH);
    delay(300);
    digitalWrite(WHITE_LED, LOW);
    delay(300);
  }

  delay(1000);
}
