#include "flipperLED.h"

void flipperLED::RunSetup() {
  if (!settings_obj.loadSetting<bool>("EnableLED")) {
    return;
  }
  //  TODO: Implement Led
}

void flipperLED::attackLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
  //  TODO: Implement Led
}

void flipperLED::sniffLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
  //  TODO: Implement Led
}

void flipperLED::offLED() {
  if (!settings_obj.loadSetting<bool>("EnableLED"))
    return;
}

void flipperLED::main() {
  // do nothing
}
