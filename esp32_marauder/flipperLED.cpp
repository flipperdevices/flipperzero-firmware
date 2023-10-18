#include "flipperLED.h"

void flipperLED::RunSetup() {
  #ifndef LED_OFF
  pinMode(B_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(R_PIN, OUTPUT);

  if (!settings_obj.loadSetting<bool>("EnableLED")) {
    digitalWrite(B_PIN, HIGH);
    digitalWrite(G_PIN, HIGH);
    digitalWrite(R_PIN, HIGH);
    return;
  }
    
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
  #endif
}

void flipperLED::attackLED() {
  #ifndef LED_OFF
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
    
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH); 
  delay(10);
  digitalWrite(R_PIN, LOW);
  #endif
}

void flipperLED::sniffLED() {
  #ifndef LED_OFF
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
    
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
  delay(10);
  digitalWrite(B_PIN, LOW);
  #endif
}

void flipperLED::offLED() {
  #ifndef LED_OFF
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
    
  digitalWrite(B_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(R_PIN, HIGH);
  #endif
}

void flipperLED::main() {
  // do nothing
}