#include "flipperLED.h"

void flipperLED::RunSetup() {
  pinMode(B_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);

  delay(50);

  digitalWrite(B_PIN, LOW);
  delay(500);
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, LOW);
  delay(500);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, LOW);
  delay(500);
  digitalWrite(R_PIN, HIGH);
}

void flipperLED::attackLED() {
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
  delay(10);
  digitalWrite(R_PIN, LOW);
}

void flipperLED::sniffLED() {
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
  delay(10);
  digitalWrite(B_PIN, LOW);
}

void flipperLED::offLED() {
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
}
