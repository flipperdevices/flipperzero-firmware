#include "stickcLED.h"
// NB M5Stick C Plus LED is active low, so digitalWrite() calls are inverted
void stickcLED::RunSetup() {
    pinMode(STICKC_LED_PIN, OUTPUT);

if (!settings_obj.loadSetting<bool>("EnableLED")) {
    digitalWrite(STICKC_LED_PIN, HIGH);
    return;
}

delay(50);

  digitalWrite(STICKC_LED_PIN, LOW);
  delay(500);
  digitalWrite(STICKC_LED_PIN, HIGH);
  delay(250);
  digitalWrite(STICKC_LED_PIN, LOW);
  delay(500);
  digitalWrite(STICKC_LED_PIN, HIGH);
  delay(250);
  digitalWrite(STICKC_LED_PIN, LOW);
  delay(500);
  digitalWrite(STICKC_LED_PIN, HIGH);
}

void stickcLED::attackLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
    
  digitalWrite(STICKC_LED_PIN, LOW);
  delay(300);
  digitalWrite(STICKC_LED_PIN, HIGH);
}

void stickcLED::sniffLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
    
  digitalWrite(STICKC_LED_PIN, LOW);
  delay(300);
  digitalWrite(STICKC_LED_PIN, HIGH);
}

void stickcLED::offLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
  
  digitalWrite(STICKC_LED_PIN, HIGH);
}

void stickcLED::main() {
  // do nothing
}
